#include "engine.hpp"
#include "csgo.hpp"
#include "GUI/CGuiInput.hpp"
#include "xorstr.hpp"

namespace modules
{
	vec3 old_punch{};
	uintptr_t g_target;

	namespace detail
	{
		vec3 original_angle{};
		vec3 view_angles{};
		bool view_angles_set = false;
	}


	void clamp(vec3& angle)
	{
		while (angle.x < -180.0f)
			angle.x += 360.0f;

		while (angle.x > 180.0f)
			angle.x -= 360.0f;

		if (angle.x > 89.0f)
			angle.x = 89.0f;

		if (angle.x < -89.0f)
			angle.x = -89.0f;

		while (angle.y < -180.0f)
			angle.y += 360.0f;

		while (angle.y > 180.0f)
			angle.y -= 360.0f;

		angle.z = 0.0f;
	}

	void vector_angles(vec3& forward, vec3* angles)
	{
		float	tmp, yaw, pitch;

		if (forward[1] == 0 && forward[0] == 0)
		{
			yaw = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else
		{
			yaw = (atan2(forward[1], forward[0]) * 180 / PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles->x = pitch;
		angles->y = yaw;
		angles->z = 0;
	}


	vec3 calc_angle(vec3 src, vec3 dst)
	{
		vec3 ang;
		vec3 delta = dst - src;
		delta.normalize_in_place();
		vector_angles(delta, &ang);

		return ang;
	}

	void smooth_angle(vec3& angle, int level)
	{
		auto delta = detail::view_angles - detail::original_angle;
		clamp(delta);

		float factor = 1.0f + (0.2f * (float)level);

		delta.x /= factor;
		delta.y /= factor;

		angle = delta + detail::original_angle;
		clamp(angle);
	}


	
	void set_temp_view_angles(vec3 angle)	
	{
		detail::view_angles = angle;
		detail::view_angles_set = true;
	}




	float get_fov(uintptr_t entity, vec3 bone, const vec3& local_view_pos, const vec3& local_view_angles)
	{
		auto target_angle = calc_angle(local_view_pos, bone);
		auto delta = target_angle - local_view_angles;
		clamp(delta);
		
		return (fabs(delta.x) + fabs(delta.y)) / 2;
	}

	float get_fov_crosshair(uintptr_t entity, vec3 bone)
	{
		vec3 bone2d;
		if (world_to_screen(bone, bone2d))
		{
			return bone2d.distance(
				vec3(g_csgo.surface->get_width() / 2,
					g_csgo.surface->get_height() / 2,
					0.0f)) / 10.0f;
		}

		return 360.0f;
	}

	vec3 get_fov_xy(uintptr_t entity, vec3 bone, const vec3& local_view_pos, const vec3& local_view_angles)
	{
		auto target_angle = calc_angle(local_view_pos, bone);
		auto delta = target_angle - local_view_angles;
		clamp(delta);

		return delta;
	}


	bool validate_target(uintptr_t entity)
	{
		if (entity == g_csgo.local_player)
			return false;

		if (is_dormant(entity))
			return false;

		if (get_health(entity) <= 0)
			return false;

		auto team = get_team(entity);
		if (team == g_csgo.local_team)
			return false;

		if (g_items.aimbotVisibleOnly[0])
		{
			if (!is_visible_trace(entity, {}))
				return false;
		}
		return true;
	}

	uintptr_t find_target(const vec3& local_view_pos, const vec3& local_view_angles, vec3& target_position, float max_fov, int bone)
	{
		uintptr_t best_target = 0;
		float best_fov = 0;


		for (int i = 1; i < max_player_index; i++)
		{
			auto entity = g_csgo.entity_list->get(i);
			if (!entity)
				continue;

			if (g_items.aimbotStayOnTarget[0] && g_csgo.target_lock &&
				entity != g_csgo.target_lock)
			{
				continue;
			}

			if (!validate_target(entity))
			{
				if (entity == g_csgo.target_lock && 
					g_items.aimbotStayOnTarget[0] == 0)
					g_csgo.target_lock = 0;

				continue;
			}
		//	printf("%i %s\n", i, get_player_info(i).m_szPlayerName);

			auto bone_position = get_bone_position(entity, bone);/*
			auto fovxy = get_fov_xy(entity, bone_position, local_view_pos, local_view_angles);*/
			auto fov = get_fov_crosshair(entity, bone_position);

			if (entity == g_csgo.target_lock)
			{
				best_fov = fov;
				best_target = entity;
				target_position = bone_position;
				return best_target;
			}
			
			if (fov > max_fov)
				continue;

			if (!best_target || fov < best_fov)
			{
				best_fov = fov;
				best_target = entity;
				target_position = bone_position;
			}

		}
		return best_target;
	}

	vec3 get_punch_angle()
	{
		return read<vec3>(g_csgo.local_player + 
			hazedumper::netvars::m_aimPunchAngle) * 2.0f;
	}

	int get_shots_fired()
	{
		return read<int>(g_csgo.local_player + hazedumper::netvars::m_iShotsFired);
	}

	void recoil_control(vec3 view, bool manage_delta_punch)
	{
		//standalone
		if (manage_delta_punch && !g_items.rcsStandalone)
		{
			old_punch = { 0,0,0 };
			return;
		}
		//aimbot rcs
		if (!manage_delta_punch && !g_items.rcsAimbot)
		{
			old_punch = { 0,0,0 };
			return;
		}

		auto punch_angle = get_punch_angle();

		int shots_fired = get_shots_fired();
		if (shots_fired > 0)
		{
			if(manage_delta_punch)
				view = view + old_punch;

			view = view - punch_angle;
			clamp(view);
			set_temp_view_angles(view);
		}
		old_punch = punch_angle;
		
	}

	void sanity_check_angles(const vec3& angles)
	{
		if (angles.x > 89.0f || angles.x < -89.0f)
			throw std::runtime_error(xorstr_("tried to set invalid viewangles.x"));

		if (angles.y > 180.0f || angles.y < -180.0f)
			throw std::runtime_error(xorstr_("tried to set invalid viewangles.y"));

		if (angles.z != 0.0f)
			throw std::runtime_error(xorstr_("tried to set invalid roll"));

		if (isnan(angles.x))
			throw std::runtime_error(xorstr_("tried to set nan viewangles.x"));

		if (isnan(angles.y))
			throw std::runtime_error(xorstr_("tried to set nan viewangles.y"));

		if (isinf(angles.x))
			throw std::runtime_error(xorstr_("tried to set inf viewangles.x"));

		if (isinf(angles.y))
			throw std::runtime_error(xorstr_("tried to set inf viewangles.y"));
	}
	float max_fov = 0.0f;

	void draw_fov()
	{
		g_csgo.surface->text(5.0f, 55.0f, g_csgo.verdana_font, aero::color(255, 255, 255, 255), xorstr_("Lock 0x%X"),g_csgo.target_lock);

		bool has_pistol = get_weapon_type(g_csgo.local_weapon) == wpn_category_pistol;
		bool has_sniper = g_csgo.local_weapon == weapon_scout || g_csgo.local_weapon == weapon_awp || g_csgo.local_weapon == weapon_ssg08;
		if (has_pistol)
		{
			g_csgo.surface->text(5.0f, 40.0f, g_csgo.verdana_font, aero::color(255, 255, 255, 255), xorstr_("Pistol Aim"));
		}
		else if(has_sniper)
		{
			g_csgo.surface->text(5.0f, 40.0f, g_csgo.verdana_font, aero::color(255, 255, 255, 255), xorstr_("Sniper Aim"));
		}
		else
		{
			g_csgo.surface->text(5.0f, 40.0f, g_csgo.verdana_font, aero::color(255, 255, 255, 255), xorstr_("Rifle Aim"));
		}
		
		if (g_items.aimbotEnabled[0])
		{
			g_csgo.surface->text(5.0f, 25.0f, g_csgo.verdana_font, aero::color(255, 255, 255, 255), xorstr_("[F1] Aimbot ON"));
		}
		else
		{
			g_csgo.surface->text(5.0f, 25.0f, g_csgo.verdana_font, aero::color(255, 0, 0, 255), xorstr_("[F1] Aimbot OFF"));
		}

		if (cgui::is_key_down_once(VK_F1)) {
			if (g_items.aimbotEnabled[0]) {
				g_items.aimbotEnabled[0] = 0;
			}
			else {
				g_items.aimbotEnabled[0] = 1;
			}
		}


		if (!g_items.aimbotDrawFOV || !g_items.aimbotEnabled[0])
			return;

		auto center_x = g_csgo.surface->get_width() / 2;
		auto center_y = g_csgo.surface->get_height() / 2;

		float width = 2.0f;
		auto fov_color = aero::color(167, 167, 167, 100);
		if (g_target)
		{
			fov_color = aero::color(255, 255, 255, 255);
			width = 2.5f;
		}
		g_csgo.surface->circle(center_x, center_y, (float)max_fov * 10.0f, fov_color, width);
	}
	DWORD __stdcall aimbot_process(LPVOID)
	{
		srand(time(0));

		DWORD sleep_val = 1;
		DWORD tmp_sleep_val = 1;

		while (true)
		{
			uintptr_t target = 0;
			int bone = 8;
			sleep_val = 1;
			if (!g_csgo.local_player || !g_csgo.client_state || !is_ingame())
			{
				Sleep(1000);
				continue;
			}

			if (!g_items.aimbotEnabled[0] && !g_items.rcsStandalone )
			{
				Sleep(100);
				continue;
			}


			bool lbutton_down = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
			if (!lbutton_down)
				lbutton_down = GetAsyncKeyState(VK_LBUTTON) & 1;

			bool has_pistol = get_weapon_type(g_csgo.local_weapon) == wpn_category_pistol;
			bool has_sniper = g_csgo.local_weapon == weapon_scout || g_csgo.local_weapon == weapon_awp || g_csgo.local_weapon == weapon_ssg08;


			detail::view_angles_set = false;
			detail::view_angles = read<vec3>(g_csgo.client_state + hazedumper::signatures::dwClientState_ViewAngles);
			detail::original_angle = detail::view_angles;

			bool manage_delta_punch = true;

			if (g_items.aimbotEnabled[0])
			{
				auto entity_list = get_entity_list();
				if (entity_list)
				{
					std::shared_lock<std::shared_timed_mutex> readerLock(g_csgo.entity_list_mtx);
					if (g_csgo.entity_list)
					{
						auto local_view_pos = get_view_position();

						if (has_pistol)
						{
							max_fov = (float)g_items.aimbotFOV[1];
							sleep_val = 0;
						}
						else if (has_sniper)
						{
							max_fov = (float)g_items.aimbotFOV[2];
							sleep_val = 0;
							if (g_csgo.local_weapon == weapon_awp)
							{
								bone = 7;
							}
						}
						else
						{
							max_fov = (float)g_items.aimbotFOV[0];
							sleep_val = 3;

						}

						vec3 target_position;
						target = find_target(local_view_pos, detail::view_angles, target_position, max_fov, bone);
						if (lbutton_down)
						{
							if (target)
							{
								auto target_angle = calc_angle(local_view_pos, target_position);
								auto delta = target_angle - detail::view_angles;

								clamp(delta);

								target_angle = delta + detail::view_angles;

								auto type = get_weapon_type(g_csgo.local_weapon);
								auto key = VK_LBUTTON;

								set_temp_view_angles(target_angle);
								manage_delta_punch = false;
								g_csgo.target_lock = target;
							}
						}
						else
						{
							//old_punch = { 0,0,0 };
							g_csgo.target_lock = 0;
						}
					}
				}
			}
			if(!has_pistol && !has_sniper)
				recoil_control(detail::view_angles, manage_delta_punch);



			if (detail::view_angles_set)
			{
				clamp(detail::view_angles);

				if (!manage_delta_punch && !has_pistol && !has_sniper)
					smooth_angle(detail::view_angles, 10);


				sanity_check_angles(detail::view_angles);

				//printf("write angle...\n");
				write<vec3>(g_csgo.client_state + hazedumper::signatures::dwClientState_ViewAngles, detail::view_angles);
			}

			g_target = target;
			Sleep(sleep_val);//60fps aimbot
		}

		return 1;
	}

	void start_aimbot()
	{
		g_csgo.surface->add_callback(draw_fov);
		CreateThread(0, 0, aimbot_process, 0, 0, 0);
	}
}