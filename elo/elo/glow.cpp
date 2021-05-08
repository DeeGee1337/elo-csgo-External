#include "modules.hpp"
#include "engine.hpp"
#include "csgo.hpp"

namespace modules
{
	void glow_entity(uintptr_t entity)
	{
		auto player = g_csgo.player(entity);
		if (!player)
			return;

		if (player->team == g_csgo.local_team)
			return;

		auto glow_index = read<int>(entity + hazedumper::netvars::m_iGlowIndex);
		if (glow_index == -1)
			return;

		/*auto max_entries = read<int>(g_csgo.client + addr::glow_manager + 0x4);
		if (glow_index > max_entries)
			return;*/

		auto entity_glow_object_ptr = get_glow_object_ptr(g_csgo.glow_manager, glow_index);
		if (!entity_glow_object_ptr)
			return;

		auto entity_glow_object = read<glow_object>(entity_glow_object_ptr);

		if (entity_glow_object.entity != entity)
			return;

		entity_glow_object.full_bloom = false;
		//entity_glow_object.r = entity_glow_object.g = entity_glow_object.b = 0.0f;

		/*	if (get_team(entity) != g_csgo.local_team)
				entity_glow_object.r = 1.0f;
			else
				entity_glow_object.g = 1.0f;*/

				//entity_glow_object.r = 1.0f;
				//entity_glow_object.g = 0.549f;
				//entity_glow_object.b = 0.0f;

		auto sound_emitted = has_sound(entity);


		if (is_visible_trace(entity, {}))
		{
			entity_glow_object.a = (float)g_items.xray2a / 255.0f;
			entity_glow_object.r = (float)g_items.xray2r / 255.0f;
			entity_glow_object.g = (float)g_items.xray2g / 255.0f;
			entity_glow_object.b = (float)g_items.xray2b / 255.0f;
		}
		else
		{
			if(sound_emitted)
			{
				entity_glow_object.a = (float)255.0f / 255.0f;
				entity_glow_object.r = (float)255.0f / 255.0f;
				entity_glow_object.g = (float)255.0f / 255.0f;
				entity_glow_object.b = (float)0.0f;
			}
			else
			{
				if (!g_items.soundGlowOnly)
				{
					entity_glow_object.a = (float)g_items.xray1a / 255.0f;
					entity_glow_object.r = (float)g_items.xray1r / 255.0f;
					entity_glow_object.g = (float)g_items.xray1g / 255.0f;
					entity_glow_object.b = (float)g_items.xray1b / 255.0f;
				}
			}
		}


		//entity_glow_object.a = 0.65f;
		entity_glow_object.render_when_occluded = true;
		entity_glow_object.render_when_unoccluded = false;

		write<glow_object>(entity_glow_object_ptr, entity_glow_object);

	}
	DWORD __stdcall glow_process(LPVOID)
	{
		while (true)
		{
			if (!is_ingame() || !g_csgo.glow_manager || !g_items.espGlow)
			{
				Sleep(1000);
				continue;
			}

			auto entity_list = get_entity_list();
			if (entity_list)
			{
				std::shared_lock<std::shared_timed_mutex> readerLock(g_csgo.entity_list_mtx);
				if (g_csgo.entity_list)
				{
					for (int i = 1; i < max_player_index; i++)
					{
						auto entity = g_csgo.entity_list->get(i);
						if (!entity)
							continue;

						if (entity == g_csgo.local_player)
							continue;

						glow_entity(entity);
					}
				}
			}

			Sleep(0);
		}

	}
	void start_glow()
	{
		CreateThread(0, 0, glow_process, 0, 0, 0);
	}
}