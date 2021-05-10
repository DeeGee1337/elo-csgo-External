#include "modules.hpp"
#include "csgo.hpp"
#include "GUI/CGuiInput.hpp"
#include "colors.hpp"
#include <atomic>
#include <vector>

namespace modules
{
	aero::api_status g_aero_status;
	std::atomic<bool> g_has_result = false;



	std::vector<int> num_bones =
	{
		0,5,7,8,13,14,15,21,24,25,27,28
	};


	std::vector<std::pair<int, int>> draw_bones =
	{
		{ 0, 5 }, { 5, 13 }, { 13, 14 }, { 14, 15 }, { 5, 7 },
		{ 7, 8 }, { 8, 21 }, { 0, 24 }, { 24, 25 }, { 0, 27 }, { 27, 28 }
	};
	struct BBox
	{
		RECT rc;

		int getWidth()
		{
			return rc.right - rc.left;
		}
		int getHeigth()
		{
			return rc.bottom - rc.top;
		}
		BBox()
		{

		}
		BBox(int x, int y, int width, int heigth)
		{/*
			if (width < 8)
				width = 8;

			if (heigth < 12)
				heigth = 12;*/

			rc.left = x;
			rc.top = y;
			rc.right = rc.left + width;
			rc.bottom = rc.top + heigth;
		}
	};

	BBox getBoundingBox(vec3 headBone, vec3 footCenter)
	{
		vec3 vBox = headBone - footCenter;

		if (vBox.y < 0)
			vBox.y *= -1;


		int boxWidth = (int)(vBox.y / 3.0f);
		int boxX = (int)headBone.x - (boxWidth / 3);
		int boxY = (int)headBone.y;


		return BBox(boxX - (boxWidth / 2), boxY - (boxWidth / 2 / 2), boxWidth + (boxWidth / 2), (int)vBox.y + (boxWidth / 2));
	}

	void draw_health(BBox& box, int health)
	{
		DWORD dwColor;

		if (health > 100)
			health = 100;

		if (health > 75)
			dwColor = 0xFF00FF00;
		else if (health > 40)
			dwColor = 0xFFFF9B00;
		else
			dwColor = 0xFFFF0000;

		int hpBarLength = health * (box.getHeigth()) / 100;

		DWORD dwDrawColor = colors::Black;


		cgui::fill_rect(box.rc.right + 5, box.rc.top, 3, box.getHeigth(), MAKE_ARGB(255, 75, 75, 75));
		cgui::fill_rect(box.rc.right + 5, box.rc.bottom - hpBarLength, 3, hpBarLength, dwColor);


		cgui::draw_rect_outline(box.rc.right + 5 - 1, box.rc.top - 1, 3 + 1, box.getHeigth() + 1, dwDrawColor);
	}

	void draw_armor(BBox& box, int armor)
	{
		DWORD dwColor = colors::DeepSkyBlue;

		if (armor > 100)
			armor = 100;

		if (armor > 75)
			dwColor = MAKE_ARGB(200, 0, 191, 255);
		else if (armor > 40)
			dwColor = MAKE_ARGB(150, 0, 191, 255);
		else
			dwColor = MAKE_ARGB(255, 0, 191, 255);

		int hpBarLength = armor * (box.getHeigth()) / 100;

		DWORD dwDrawColor = colors::Black;

		cgui::fill_rect(box.rc.right + 10, box.rc.top, 3, box.getHeigth(), MAKE_ARGB(255, 75, 75, 75));
		cgui::fill_rect(box.rc.right + 10, box.rc.bottom - hpBarLength, 3, hpBarLength, dwColor);

		cgui::draw_rect_outline(box.rc.right + 10 - 1, box.rc.top - 1, 3 + 1, box.getHeigth() + 1, dwDrawColor);
	}
	
	void sound_esp()
	{
	

		
		
	}

	void render_skel(player_ptr player, uintptr_t entity)
	{

		vec3 vParent, vChild, sParent, sChild;
		for (auto& bone : player->bones)
		{
			if (bone.ptr)
			{
				if (bone.data.flags & 0x100 && (bone.data.parent != -1))
				{
					vParent = get_bone_position(entity, bone.index);
					vChild = get_bone_position(entity, bone.data.parent);



					if (world_to_screen(vParent, sParent) && world_to_screen(vChild, sChild))
					{
						//g_csgo.surface->text(sParent[0], sParent[1], g_csgo.verdana_font, { 255, 255, 0, 255 }, "%i", bone.index);
						g_csgo.surface->line(sParent[0], sParent[1], sChild[0], sChild[1], aero::color(255, 255, 255, 130), 1.5f);
					}

				}
			}
		}
	}

	void draw_filled_box(int x, int y, int w, int h)
	{
		int wlen = (w);
		int hlen = (h);

		cgui::fill_rect(x, y, wlen, hlen, MAKE_ARGB(100, 240, 248, 255));
	}

	void draw_corner_box(int x, int y, int w, int h, DWORD drawColor)
	{
		int wlen = (w / 5);
		int hlen = (h / 5);

		float thickness = 1.0f;

		//left top
		cgui::draw_line(x, y, x + wlen, y, drawColor, thickness);
		cgui::draw_line(x, y, x, y + hlen, drawColor, thickness);

		//right top
		cgui::draw_line(x + w, y, x + w - wlen, y, drawColor, thickness);
		cgui::draw_line(x + w, y, x + w, y + hlen, drawColor, thickness);

		//left bot
		cgui::draw_line(x, y + h, x, y + h - hlen, drawColor, thickness);
		cgui::draw_line(x, y + h, x + wlen, y + h, drawColor, thickness);

		//right bot
		cgui::draw_line(x + w, y + h, x + w - wlen, y + h, drawColor, thickness);
		cgui::draw_line(x + w, y + h, x + w, y + h - hlen, drawColor, thickness);
	}
	const char* ranks[] = { "Unranked", "Silver I", "Silver II", "Silver III", "Silver IV", "Silver Elite", "Silver Elite Master", "Gold Nova I", "Gold Nova II", "Gold Nova III", "Gold Nova Elite", "Master Guardian I", "Master Guardian II", "Master Guardian Elite", "Distinguished Master Guardian", "Legendary Eagle", "Legendary Eagle Master", "Supreme Master First Class", "The Global Elite" };

	const char* player_rank(int index) 
	{
		unsigned rankIndex = read_pointer(read_pointer(g_csgo.client + 0x31d2810) + 0x1A84 + (index * 4));
		if (rankIndex > sizeof(ranks) / sizeof(ranks[0]))
			return nullptr;
		return ranks[rankIndex];
	}
	void render_players()
	{
		
		if (!g_csgo.local_player)
			return;

		if (!g_items.espPlayers)
			return;

		std::shared_lock<std::shared_timed_mutex> readerLock(g_csgo.entity_list_mtx);
		if (g_csgo.entity_list)
		{
			for (int i = 1; i < max_player_index; i++)
			{
				auto entity = g_csgo.entity_list->get(i);
				if (!entity)
					continue;

				auto player = g_csgo.player(entity);
				if (!player)
					continue;

				auto hp = get_health(entity);
				if (hp <= 0)
					continue;

				auto armor = get_armor(entity);
				if (armor <= 0)
					continue;

				if (player->team == g_csgo.local_team)
					continue;

				auto sound_emitted = has_sound(entity);
				if (!sound_emitted && !is_visible_trace(entity, {}))
				{
					if (g_items.espLegit)
						continue;
				}

				if (g_items.espSkeleton)
				{
					render_skel(player, entity);
				}			

				auto head_3d = get_bone_position(entity, 8);
				auto foot_3d = get_bone_position(entity, 1);

				vec3 head, foot;
				if (world_to_screen(head_3d, head) && world_to_screen(foot_3d, foot))
				{
					head.y = head.y - (head.y / 150.0f);
					foot.y = foot.y + (foot.y / 150.0f);
					auto bbox = getBoundingBox(head, foot);
					if (g_items.espBox)
					{

						int w = bbox.getWidth();
						int h = bbox.getHeigth();

						auto color = colors::DeepSkyBlue;

						if (entity == g_csgo.target_lock)
							color = colors::WhiteSmoke;

						/*draw_corner_box(bbox.rc.left - 1, bbox.rc.top - 1, w + 2, h + 2, colors::Black);
						draw_corner_box(bbox.rc.left + 1, bbox.rc.top + 1, w - 2, h - 2, colors::Black);*/
						draw_corner_box(bbox.rc.left, bbox.rc.top, w, h, color);
					}
					if (g_items.espBoxFilled)
					{
						int w = bbox.getWidth();
						int h = bbox.getHeigth();

						draw_filled_box(bbox.rc.left, bbox.rc.top, w, h);
					}
					if (g_items.espHealth)
					{					
						draw_health(bbox, hp);
					}
					if (g_items.espArmor)
					{
						draw_armor(bbox, armor);
					}
					
					auto fov = modules::get_fov_crosshair(entity, get_bone_position(entity, 8));

					

					//g_csgo.surface->text(bbox.rc.right + 20, bbox.rc.top + bbox.getHeigth() / 2,g_csgo.verdana_font,aero::color(255,255,0,255), "fov %f", fov);
					if (g_items.espName)
					{
						auto r = player_rank(player->index);
						if(r)
						cgui::draw_text_c(bbox.rc.left + (bbox.getWidth() / 2), bbox.rc.top - 10, r, colors::WhiteSmoke);
						cgui::draw_text_c(bbox.rc.left + (bbox.getWidth() / 2), bbox.rc.top - 20, player->info.m_szPlayerName, colors::WhiteSmoke);

					}
					cgui::draw_text_c(bbox.rc.left + (bbox.getWidth() / 2), bbox.rc.bottom + 12, get_weapon_name(player->weapon_id).c_str(), colors::WhiteSmoke);
				}
			}
		}
	}

	DWORD __stdcall process_visuals(LPVOID argument)
	{
		g_has_result = false;

		{
			g_csgo.overlay = std::make_unique<aero::overlay>();

			g_aero_status = g_csgo.overlay->attach(g_csgo.process_id);
			if (g_aero_status != aero::api_status::success)
			{
				g_has_result = true;
				return 1;
			}

			g_csgo.surface = g_csgo.overlay->get_surface();
			g_csgo.verdana_font = g_csgo.surface->add_font("verdana_normal", "Verdana", 12.0f);

			if (!g_csgo.surface)
				throw std::runtime_error("cant create surface");

			if (!g_csgo.verdana_font)
				throw std::runtime_error("failed to create font");
		}
		g_has_result = true;

		g_csgo.surface->add_callback(render_menu);
		g_csgo.surface->add_callback(render_players);
		g_csgo.surface->add_callback(sound_esp);


		while (g_csgo.overlay->message_loop()) {
			if (g_csgo.surface->begin_scene()) {
				g_csgo.surface->end_scene();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(2));
			read_memory(g_csgo.client + hazedumper::signatures::dwViewMatrix, &g_csgo.view_matrix[0], sizeof(g_csgo.view_matrix));
		}


		return 1;
	}

	aero::api_status aero_status()
	{
		return g_aero_status;
	}

	bool is_overlay_ready()
	{
		return g_has_result;
	}

	void start_visuals()
	{
		LoadLibraryA("user32.dll");

		CreateThread(0, 0, process_visuals, 0, 0, 0);
	}
}
