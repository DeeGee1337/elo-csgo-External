#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <cstdint>
#include <chrono>
#include <thread>
#include <optional>
#include <cassert>
#include "engine.hpp"
#include "modules.hpp"
#include "csgo.hpp"
#include "bsp_parser.hpp"
#include "xorstr.hpp"
csgo g_csgo;

bool bootstrap()
{

	printf(xorstr_("waiting for csgo...\n"));

	do
	{
		auto pid = get_process_id(L"csgo.exe");
		if (pid.has_value())
		{
			g_csgo.process_id = pid.value();
			break;
		}

		Sleep(1000);
	} while (true);

	g_csgo.process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, g_csgo.process_id);
	if (g_csgo.process_handle == nullptr)
	{
		printf(xorstr_("failed to open process\n"));
		return false;
	}

	printf(xorstr_("waiting for csgo window...\n"));

	do
	{
		g_csgo.window = FindWindowA("Valve001", "Counter-Strike: Global Offensive - Direct3D 9");
		if (g_csgo.window)
			break;

		Sleep(1000);
	} while (!g_csgo.window);

	printf(xorstr_("initializing modules...\n"));


	while (!g_csgo.client)
	{
		g_csgo.client = get_module_base(L"client.dll");
		if (g_csgo.client)
			break;

		printf(xorstr_("failed to find client.dll\n"));
		Sleep(1000);
	}

	while (!g_csgo.engine)
	{
		g_csgo.engine = get_module_base(L"engine.dll");
		if (g_csgo.engine)
			break;

		printf(xorstr_("failed to find engine.dll\n"));
		Sleep(1000);
	}


	printf(xorstr_("csgo pid: %i\n"), g_csgo.process_id);
	printf(xorstr_("csgo wnd: 0x%p\n"), g_csgo.window);
	printf(xorstr_("client.dll @ 0x%x\n"), g_csgo.client);
	printf(xorstr_("engine.dll @ 0x%x\n"), g_csgo.engine);

	while (!g_csgo.local_player)
	{
		g_csgo.local_player = read_pointer(g_csgo.client + hazedumper::signatures::dwLocalPlayer);
		Sleep(100);
	}

	printf(xorstr_("local_player @ 0x%x\n"), g_csgo.local_player);
	g_csgo.client_state = get_client_state_ptr();

	printf(xorstr_("client_state @ 0x%x\n"), g_csgo.client_state);
	return true;
}

void handle_game_end()
{
	if (g_csgo.ingame)
	{
		std::lock_guard<std::shared_timed_mutex> lock(g_csgo.entity_list_mtx);
		g_csgo.local_player = 0;
		g_csgo.client_state = 0;
		g_csgo.glow_manager = 0;
		g_csgo.user_info = 0;
		g_csgo.entity_list_ptr = 0;

		{

			if (g_csgo.entity_list)
				delete g_csgo.entity_list;

			g_csgo.player_by_index.clear();
			g_csgo.player_by_ptr.clear();
			g_csgo.entity_list = nullptr;
		}

		g_csgo.ingame = false;
	}
}

void read_basic_pointer()
{
	g_csgo.entity_list_ptr = get_entity_list();
	g_csgo.local_player = read_pointer(g_csgo.client + hazedumper::signatures::dwLocalPlayer);
	if (g_csgo.local_player)
	{
		g_csgo.local_team = get_team(g_csgo.local_player);
		auto entity_list = g_csgo.entity_list_ptr;
		if (entity_list)
		{
			auto weapon = get_entity(entity_list, read<int>(g_csgo.local_player + hazedumper::netvars::m_hActiveWeapon) & 0xFFF);
			if (weapon)
			{
				g_csgo.local_weapon = (int)read<uint16_t>(weapon + hazedumper::netvars::m_iItemDefinitionIndex);
			}
		}

	}

	g_csgo.glow_manager = read_pointer(g_csgo.client + hazedumper::signatures::dwGlowObjectManager);

	g_csgo.client_state = get_client_state_ptr();
	if (g_csgo.client_state)
	{
		auto network_string_table = read_pointer(g_csgo.client_state + hazedumper::signatures::dwClientState_PlayerInfo);
		if (network_string_table)
		{

			auto network_string_dict = read_pointer(network_string_table + 0x40);
			if (network_string_dict)
			{
				g_csgo.user_info = read_pointer(network_string_dict + 0xC);
			}
		}
	}

}

bool player_init(player_ptr player, uintptr_t entity, int index)
{
	player->ptr = entity;
	player->index = index;
	player->team = get_team(entity);
	player->health = get_health(entity);

	if (is_dormant(entity))
		return false;

	if (player->health <= 0)
		return false;

	player->info = get_player_info(index);
	player->head = get_bone_position(entity, 8);
	player->weapon_id = -1;

	auto weapon = get_entity(g_csgo.entity_list_ptr, read<int>(entity + hazedumper::netvars::m_hActiveWeapon) & 0xFFF);
	if (weapon)
	{
		player->weapon_id = (int)read<uint16_t>(weapon + hazedumper::netvars::m_iItemDefinitionIndex);

	}

	studiohdr_t hdr;
	auto hdr_ptr = get_studio_hdr(entity, hdr);
	if (hdr_ptr)
	{
		for (int i = 0; i < hdr.numbones; i++)
		{
			msstudiobone_t bone;
			auto bone_ptr = get_studio_bone(hdr_ptr, hdr, i, bone);
			if (bone_ptr)
			{
				player->bones.push_back({ i , bone_ptr, bone });
			}
		}
	}

	return true;
}

void read_entities()
{
	if (g_csgo.entity_list_ptr)
	{
		entitylist_t* ent_list = new entitylist_t(g_csgo.entity_list_ptr, 65);
		std::unordered_map<uintptr_t, player_ptr> new_players_by_ptr;
		std::unordered_map<int, player_ptr> new_players_by_idx;

		for (int i = 1; i < ent_list->size; i++)
		{
			auto entity = ent_list->get(i);
			if (!entity)
				continue;

			if (entity == g_csgo.local_player)
				continue;

			player_ptr player;
			player.reset(new player_t());

			if (player_init(player, entity, i))
			{
				new_players_by_ptr.insert({ entity , player });
				new_players_by_idx.insert({ i , player });
			}
		}


		auto old_list = g_csgo.entity_list;
		{
			std::lock_guard<std::shared_timed_mutex> lock(g_csgo.entity_list_mtx);
			g_csgo.entity_list = ent_list;
			g_csgo.player_by_ptr = new_players_by_ptr;
			g_csgo.player_by_index = new_players_by_idx;
		}
		delete old_list;
	}
}

void cache_update()
{
	if (!is_ingame())
	{
		handle_game_end();
		Sleep(1000);
		return;
	}

	read_basic_pointer();
	read_entities();
}

std::string old_map_path{};
DWORD __stdcall visible_cache_update(LPVOID)
{
	while (true)
	{
		if (!is_ingame() || !g_csgo.local_player)
		{
			Sleep(1000);
			continue;
		}

		auto game_dir = get_game_path();
		auto map_path = get_map_path();

		if (map_path != old_map_path) {
			printf(xorstr_("loading map %s%s\n"), game_dir.c_str(), map_path.c_str());

			g_csgo.bsp = std::make_unique<rn::bsp_parser>();
			if (!g_csgo.bsp->load_map(game_dir, map_path))
			{
				printf(xorstr_("failed to load map\n"));
				getchar();
				return 1;
			}
			old_map_path = map_path;
			printf(xorstr_("map loaded successfully\n"));
		}


		auto local_origin = get_view_position();

		std::unordered_map<uintptr_t, skeleton_ptr> new_skel_map;
		{
			std::shared_lock<std::shared_timed_mutex> readerLock(g_csgo.entity_list_mtx);
			if (!g_csgo.entity_list)
			{
				Sleep(1000);
				continue;
			}
			for (int i = 0; i < g_csgo.entity_list->size; i++)
			{
				auto entity = g_csgo.entity_list->get(i);
				if (!entity)
					continue;


				skeleton_ptr skeleton;
				skeleton.reset(new skeleton_t());
				skeleton->ptr = entity;
				skeleton->player_index = i;
				skeleton->is_visible = g_csgo.bsp->is_visible(local_origin, get_bone_position(entity, 8));;

				//studiohdr_t hdr;
				//auto hdr_ptr = get_studio_hdr(entity, hdr);
				//if (hdr_ptr)
				//{
				//	for (int y = 0; y < hdr.numbones; y++)
				//	{
				//		msstudiobone_t bone;
				//		auto bone_ptr = get_studio_bone(hdr_ptr, hdr, y, bone);
				//		if (bone_ptr)
				//		{
				//			if (bone.flags & 0x100 && (bone.parent != -1))
				//			{
				//				skeleton_t::bone_t skeleton_bone;
				//				skeleton_bone.data = bone;
				//				skeleton_bone.index = y;
				//				skeleton_bone.ptr = bone_ptr;
				//				skeleton_bone.position = get_bone_position(entity, skeleton_bone.index);
				//				skeleton_bone.is_visible = g_csgo.bsp->is_visible(local_origin, skeleton_bone.position);

				//				if (skeleton_bone.is_visible)
				//					skeleton->is_visible = true;

				//				skeleton->bones.push_back(skeleton_bone);
				//			}
				//		}
				//	}
				//}

				new_skel_map.insert({ entity, skeleton });
			}
		}
		{

			std::lock_guard<std::shared_timed_mutex> lock(g_csgo.skeleton_mtx);
			g_csgo.skeletons = new_skel_map;
		}

		Sleep(100);
	}
	return 1;
}

int main(int argc, char** argv)
{
	system(xorstr_("title ELO-NEGER"));
	printf(xorstr_("        _         \n"));
	printf(xorstr_("      >(.)__      \n"));
	printf(xorstr_("______ (___/  ENTE\n\n\n"));
	system(xorstr_("Color 0C"));

	if (!bootstrap())
	{
		getchar();
		return 1;
	}

	modules::start_visuals();

	while (!modules::is_overlay_ready())
		Sleep(200);

	auto status = modules::aero_status();
	if (status != aero::api_status::success) 
	{
		printf(xorstr_("failed to initialize overlay"));
		getchar();
		return 1;
	}

	printf(xorstr_("waiting for game initialization...\n"));

	do 
	{
		Sleep(1000);
	} while (!is_ingame());

	CreateThread(0, 0, visible_cache_update, 0, 0, 0);


	printf(xorstr_("starting modules...\n"));

	modules::start_glow();
	modules::start_aimbot();
	modules::start_hitmarker();

	while (true)
	{
		cache_update();
		Sleep(300);
	}


	return 1;
}