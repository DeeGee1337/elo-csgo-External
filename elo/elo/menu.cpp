#include "modules.hpp"
#include "GUI/CGui.hpp"
#include "csgo.hpp"
#include "menu_items.hpp"
#include "xorstr.hpp"

namespace modules
{
	bool		gui_is_init = false;


	cgui::gui_wnd gui_wnd;
	cgui::gui_tab aimbot_tab;
	cgui::gui_tab esp_tab;
	cgui::gui_tab item_tab;
	cgui::gui_tab color_tab;
	cgui::gui_tab misc_tab;
	cgui::gui_tab settings_tab;
	cgui::gui_tab radar_tab;

	void load_defaults()
	{
		g_items.aimbotEnabled[0] = 1;
		g_items.aimbotFOV[0] = 5;
		g_items.aimbotFOV[1] = 3;
		g_items.aimbotFOV[2] = 6;
		g_items.espSkeleton = 1;
		g_items.espBox = 1;
		g_items.espHealth = 1;
		g_items.espArmor = 1;
		g_items.espPlayers = 1;
		//g_items.espName = 1;
		g_items.aimbotDrawFOV = 1;
		g_items.espGlow = 1;
		g_items.rcsAimbot = 1;
		g_items.rcsStandalone = 1;
		g_items.aimbotVisibleOnly[0] = 1;
		g_items.espDrawHitmarker = 1;
		g_items.espHitmarkerSound = 1;
		g_items.espDrawCrosshair = 1; 
		g_items.aimbotSmooth[0] = 15;
		g_items.soundGlowOnly = 0;

		g_items.xray1a = 176;
		g_items.xray1r = 24;
		g_items.xray1g = 239;
		g_items.xray1b = 232;

		g_items.triggerbotKey = 5;
		g_items.triggerbot = 1;
		g_items.bhop = 1;

		g_items.xray2a = 238;
		g_items.xray2r = 224;
		g_items.xray2g = 0;
		g_items.xray2b = 0;
		g_items.aimbotStayOnTarget[0] = 1;
		g_items.espLegit = 0;

	}
	void init_gui()
	{
		const cgui_str boneopt[] = { (xorstr_("Head")),(xorstr_("Neck")),(xorstr_("Chest")), (xorstr_("Spine")) };
		const cgui_str skelopt[] = { (xorstr_("Disabled")),(xorstr_("Normal")),(xorstr_("Thick")) };
		const cgui_str radarposopt[] = { (xorstr_("Top Left")),(xorstr_("Top Right")), (xorstr_("Bottom Left")),(xorstr_("Bottom Right")),(xorstr_("Center")) };
		const cgui_str xhairopt[] = { (xorstr_("Disabled")),(xorstr_("White")),(xorstr_("Black")) };
		const cgui_str rarityopt[] = { (xorstr_("All")),(xorstr_("Rare")) };

		load_defaults();



		cgui::window(&gui_wnd, CGUI_STR(xorstr_("elusion braucht CHEATS!")), 500, 350, 490, 600);//+60

		const auto half_width = cgui::calc_width_pct(&gui_wnd, 0.5f);
		const auto full_height = cgui::calc_height_pct(&gui_wnd, 1.0f);
		const auto full_width = cgui::calc_width_pct(&gui_wnd, 1.0f) - 5;

		const auto x_begin = CGUI_CLIENTAREA_PADDING;
		const auto x_center = CGUI_CLIENTAREA_PADDING + half_width + CGUI_PADDING_GROUPS;


		//	----------------------------- Aimbot -----------------------------
		cgui::tab(&gui_wnd, &aimbot_tab, CGUI_STR(xorstr_("Aimbot")), 0);
		auto& curr_tab = aimbot_tab;

		cgui::auto_checkbox(&curr_tab, CGUI_STR(xorstr_("Activate")), &g_items.aimbotEnabled[0]);
		cgui::auto_checkbox(&curr_tab, CGUI_STR(xorstr_("RCS Aimbot")), &g_items.rcsAimbot);
		cgui::auto_checkbox(&curr_tab, CGUI_STR(xorstr_("RCS Standalone")), &g_items.rcsStandalone);
		cgui::auto_checkbox(&curr_tab, CGUI_STR(xorstr_("Visible Check")), &g_items.aimbotVisibleOnly[0]);
		cgui::auto_checkbox(&curr_tab, CGUI_STR(xorstr_("No Auto Switch")), &g_items.aimbotStayOnTarget[0]);
		cgui::auto_checkbox(&curr_tab, CGUI_STR(xorstr_("Draw FOV")), &g_items.aimbotDrawFOV);
		cgui::auto_slider(&curr_tab, CGUI_STR(xorstr_("Rifle FOV")), 0, 180, xorstr_("degrees"), &g_items.aimbotFOV[0]);
		cgui::auto_slider(&curr_tab, CGUI_STR(xorstr_("Pistol FOV")), 0, 180, xorstr_("degrees"), &g_items.aimbotFOV[1]);
		cgui::auto_slider(&curr_tab, CGUI_STR(xorstr_("Sniper FOV")), 0, 180, xorstr_("degrees"), &g_items.aimbotFOV[2]);
		cgui::auto_slider(&curr_tab, CGUI_STR(xorstr_("Smoothing")), 0, 100, xorstr_("percent"), &g_items.aimbotSmooth[0]);

		cgui::auto_spacer(&curr_tab);
		cgui::auto_checkbox(&curr_tab, CGUI_STR(xorstr_("Triggerbot")), &g_items.triggerbot);
		cgui::auto_hotkey(&curr_tab, CGUI_STR(xorstr_("Trigger Key")), &g_items.triggerbotKey);

		cgui::auto_spacer(&curr_tab);
		cgui::auto_combobox(&curr_tab, CGUI_STR(xorstr_("Bone 1")), boneopt, ARRAYSIZE(boneopt), &g_items.aimbotBone[0]);
		cgui::auto_hotkey(&curr_tab, CGUI_STR(xorstr_("Key 1")), &g_items.aimbotKey[0]);

		cgui::auto_combobox(&curr_tab, CGUI_STR(xorstr_("Bone 2")), boneopt, ARRAYSIZE(boneopt), &g_items.aimbotBone[1]);
		cgui::auto_hotkey(&curr_tab, CGUI_STR(xorstr_("Key 2")), &g_items.aimbotKey[1]);



		//	----------------------------- Visuals -----------------------------
		cgui::tab(&gui_wnd, &esp_tab, CGUI_STR(xorstr_("Visuals")), 0);


		auto player_group = cgui::auto_group(&esp_tab, CGUI_STR(xorstr_("Player")), x_begin, full_width, 250);
		cgui::sync_x_to_group(&esp_tab, player_group);
		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Player ESP")), &g_items.espPlayers);
		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Legit ESP")), &g_items.espLegit);
		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Player Glow")), &g_items.espGlow);
		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Legit Glow")), &g_items.soundGlowOnly);
		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Show Team")), &g_items.espPlayersTeam);
		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Name")), &g_items.espName);
		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("2D Box")), &g_items.espBox);

		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("2D Box filled")), &g_items.espBoxFilled); //test

		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Health")), &g_items.espHealth);

		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Armor")), &g_items.espArmor);

		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Skeleton")), &g_items.espSkeleton);
	

		esp_tab.y_offset += 15;
		const auto misc_group = cgui::auto_group(&esp_tab, CGUI_STR(xorstr_("Misc")), x_begin, full_width, 270);
		cgui::sync_x_to_group(&esp_tab, misc_group);
		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Recoil Crosshair")), &g_items.espDrawCrosshair);
		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Hitmarker")), &g_items.espDrawHitmarker);
		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Hitmarker Sound")), &g_items.espHitmarkerSound);
		cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Bunnyhop")), &g_items.bhop);
		/*cgui::auto_checkbox(&esp_tab, CGUI_STR(xorstr_("Radarhack")), &g_items.radarhack);*/

		cgui::auto_slider(&esp_tab, CGUI_STR(xorstr_("Glow Visible A")), 0, 255, " ", &g_items.xray1a);
		cgui::auto_slider(&esp_tab, CGUI_STR(xorstr_("Glow Visible R")), 0, 255, " ", &g_items.xray1r);
		cgui::auto_slider(&esp_tab, CGUI_STR(xorstr_("Glow Visible G")), 0, 255, " ", &g_items.xray1g);
		cgui::auto_slider(&esp_tab, CGUI_STR(xorstr_("Glow Visible B")), 0, 255, " ", &g_items.xray1b);

		cgui::auto_slider(&esp_tab, CGUI_STR(xorstr_("Glow Invisible A")), 0, 255, " ", &g_items.xray2a);
		cgui::auto_slider(&esp_tab, CGUI_STR(xorstr_("Glow Invisible R")), 0, 255, " ", &g_items.xray2r);
		cgui::auto_slider(&esp_tab, CGUI_STR(xorstr_("Glow Invisible G")), 0, 255, " ", &g_items.xray2g);
		cgui::auto_slider(&esp_tab, CGUI_STR(xorstr_("Glow Invisible B")), 0, 255, " ", &g_items.xray2b);
	}

	DWORD input_backup = 0;
	static int frames = 0;
	static std::time_t starttime = 0;
	static bool first = TRUE;
	static float fps = 0.0f;

	void render_menu()
	{

		if (first)
		{
			frames = 0;
			starttime = util::time::now();
			first = FALSE;
		}
		else
		{
			frames++;
			auto delta = util::time::to_seconds(util::time::now() - starttime);
			if (delta  > 0.25 && frames > 10)
			{
				fps = (double)frames / (delta);
				starttime = util::time::now();
				frames = 0;
			}
		}

		g_csgo.surface->text(5.f, 5.f, g_csgo.verdana_font, 0xFFFFFFFF, xorstr_("elo-neger [%.2f FPS]"), fps);


		if (!gui_is_init)
		{
			init_gui();
			gui_is_init = true;
		}

		auto entity_list = get_entity_list();
		if (entity_list)
		{
			auto weapon = get_entity(entity_list, read<int>(g_csgo.local_player + hazedumper::netvars::m_hActiveWeapon) & 0xFFF);
			if (weapon)
			{
				auto weapon_id = read<uint16_t>(weapon + hazedumper::netvars::m_iItemDefinitionIndex);
			}
		}


		cgui::update_input();
		cgui::render_gui(&gui_wnd);

		if (gui_wnd.is_visible)
		{
			/*auto input_system = read_pointer(g_csgo.client + addr::input_system);
			if (input_system)
			{

			}*/
		}
	}
}