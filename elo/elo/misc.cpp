#include "modules.hpp"
#include "csgo.hpp"

#pragma comment(lib, "winmm.lib")

namespace modules
{
	int g_last_hit_count = 0;
	bool hitmarker_init = false;

	void draw_recoil_crosshair()
	{
		if (!g_csgo.local_player)
			return;

		if (!g_items.espDrawCrosshair)
			return;

		auto punch_angle = read<vec3>(g_csgo.local_player + hazedumper::netvars::m_aimPunchAngle);
		auto shots_fired = read<int>(g_csgo.local_player + hazedumper::netvars::m_iShotsFired);

		if (shots_fired > 0)
		{
			vec3 pAng = punch_angle;
			vec3 xhair;

			xhair.x = g_csgo.surface->get_width() / 2 - (g_csgo.surface->get_width() / 90 * pAng.y);
			xhair.y = g_csgo.surface->get_height() / 2 + (g_csgo.surface->get_height() / 90 * pAng.x);

			float xh_size = 4.0f;

			vec3 l, r, t, b;
			l = r = t = b = xhair;
			l.x = l.x - xh_size;
			r.x = r.x + xh_size;
			t.y = t.y - xh_size;
			b.y = b.y + xh_size;

			g_csgo.surface->line(l.x, l.y, r.x, r.y, aero::color(255, 255, 255, 255), 2.0f);
			g_csgo.surface->line(t.x, t.y, b.x, b.y, aero::color(255, 255, 255, 255), 2.0f);
		}
	}

	void bhop()
	{
		if (!g_items.bhop)
			return;

		if (g_csgo.local_player)
		{
			auto pflags = read<uint8_t>(g_csgo.local_player + hazedumper::netvars::m_fFlags);
			if (GetAsyncKeyState(VK_SPACE) && pflags & (1 << 0))
			{
				write<uint32_t>(g_csgo.client + hazedumper::signatures::dwForceJump, 6);
			}
		}
	}

	void trigger()
	{
		if (!g_items.triggerbot)
			return;

		if (g_csgo.local_player)
		{
			auto incross = read<int>(g_csgo.local_player + hazedumper::netvars::m_iCrosshairId);
			if (incross > 0)
			{
				auto entlist = get_entity_list();
				if (entlist)
				{
					auto target = get_entity(entlist, incross);
					if (target)
					{
						if (get_team(target) != g_csgo.local_team)
						{
							if (GetAsyncKeyState(g_items.triggerbotKey) & 0x8000)
							{
								write<bool>(g_csgo.client + hazedumper::signatures::dwForceAttack, true);
								Sleep(20);
								write<bool>(g_csgo.client + hazedumper::signatures::dwForceAttack, false);
							}
						}
					}
				}
			}
		}
	}

	void draw_hitmarker()
	{
		if (!g_items.espDrawHitmarker)
			return;

		if (g_csgo.hitmarker)
		{
			auto delta = util::time::now() - g_csgo.hit_time;
			auto time_since_hit = util::time::to_seconds(delta);
			if (time_since_hit <= 0.5f)
			{
				auto percent = time_since_hit / 0.5f;

				int alpha = (int)(255.0f - (percent * 255.0f));


				auto hitmarker_color = aero::color(255, 255, 255, alpha);
				auto hitmarker_outline = aero::color(0, 0, 0, alpha);

				int line_size = 30;
				int center_padding = 8;

				auto crosshair_x = g_csgo.surface->get_width() / 2;
				auto crosshair_y = g_csgo.surface->get_height() / 2;

				g_csgo.surface->line(
					crosshair_x - line_size / 2, 
					crosshair_y - line_size / 2, 
					crosshair_x - center_padding, 
					crosshair_y - center_padding, 
					hitmarker_color, 
					2.0f);

				g_csgo.surface->line(
					crosshair_x + line_size / 2,
					crosshair_y + line_size / 2,
					crosshair_x + center_padding,
					crosshair_y + center_padding,
					hitmarker_color,
					2.0f);

				g_csgo.surface->line(
					crosshair_x + line_size / 2,
					crosshair_y - line_size / 2,
					crosshair_x + center_padding,
					crosshair_y - center_padding,
					hitmarker_color,
					2.0f);

				g_csgo.surface->line(
					crosshair_x - line_size / 2,
					crosshair_y + line_size / 2,
					crosshair_x - center_padding,
					crosshair_y + center_padding,
					hitmarker_color,
					2.0f);
			}
		}
	}

	DWORD __stdcall process_hitmarker(LPVOID)
	{
		while (true)
		{
			if (!g_csgo.local_player || !is_ingame() || get_health(g_csgo.local_player) <= 0 || 
				(g_items.espDrawHitmarker == 0 && g_items.espHitmarkerSound == 0))
			{
				Sleep(1000);
				g_csgo.hitmarker = false;
				hitmarker_init = false;
				continue;
			}

			if (g_csgo.hitmarker)
			{
				auto delta = util::time::now() - g_csgo.hit_time;
				if (delta > util::time::from_milliseconds(5000))
					g_csgo.hitmarker = false;
			}

			if (!hitmarker_init)
			{
				g_last_hit_count = read<int>(g_csgo.local_player + hazedumper::netvars::m_totalHitsOnServer);
				hitmarker_init = true;
			}

			auto hits_on_server = read<int>(g_csgo.local_player + hazedumper::netvars::m_totalHitsOnServer);
			if (hits_on_server != g_last_hit_count && hits_on_server != 0 &&
				get_health(g_csgo.local_player) > 0)
			{
				//printf("hit\n");SND_NODEFAULT
				g_csgo.hit_time = util::time::now();
				g_csgo.hitmarker = true;

				if (g_items.espHitmarkerSound)
				{
					PlaySoundA("hitmarker.wav", NULL, SND_FILENAME | SND_ASYNC);
				}
				g_last_hit_count = hits_on_server;
			}

			bhop();
			trigger();
			Sleep(10);
		}

		return 1;
	}
	void start_hitmarker()
	{
		g_csgo.surface->add_callback(draw_hitmarker);
		g_csgo.surface->add_callback(draw_recoil_crosshair);

		CreateThread(0, 0, process_hitmarker, 0, 0, 0);
	}
}