#pragma once

#include "aero-overlay/overlay.hpp"
#include "vector.hpp"

namespace modules
{
	void start_glow();
	void start_aimbot();
	void start_hitmarker();
	bool is_overlay_ready();
	void start_visuals();
	void render_menu();
	aero::api_status aero_status();
	float get_fov_crosshair(uintptr_t entity, vec3 bone);
}