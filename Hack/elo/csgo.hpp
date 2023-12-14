#pragma once

#include <shared_mutex>
#include <windows.h>
#include "engine.hpp"
#include "aero-overlay/overlay.hpp"
#include "time.hpp"
#include "menu_items.hpp"
#include "GUI/CGuiDraw.hpp"
#include "GUI/CGuiInput.hpp"
#include "bsp_parser.hpp"
#include <memory>

struct csgo
{
	int local_team = 0;
	DWORD process_id = 0;
	bool ingame = false;
	uintptr_t client = 0;
	uintptr_t engine = 0;
	HWND window = nullptr;
	uintptr_t user_info = 0;
	aero::surface_ptr surface;
	uintptr_t glow_manager = 0;
	uintptr_t local_player = 0;
	uintptr_t client_state = 0;
	aero::font_ptr verdana_font;
	HANDLE process_handle = nullptr;
	entitylist_t* entity_list = nullptr;
	std::unique_ptr<aero::overlay> overlay;
	std::shared_timed_mutex entity_list_mtx;
	std::shared_timed_mutex skeleton_mtx;
	uintptr_t entity_list_ptr = 0;
	int local_weapon = -1;
	bool hitmarker = false;
	uintptr_t target_lock = 0;
	std::time_t hit_time = 0;
	float view_matrix[16];
	std::unordered_map<uintptr_t, player_ptr> player_by_ptr;
	std::unordered_map<int, player_ptr> player_by_index;
	std::unordered_map<uintptr_t, skeleton_ptr> skeletons;
	std::unique_ptr<rn::bsp_parser> bsp;

	skeleton_ptr skeleton(uintptr_t entity)
	{
		std::shared_lock<std::shared_timed_mutex> lock(skeleton_mtx);

		auto iter = skeletons.find(entity);
		if (iter == skeletons.end())
			return nullptr;

		return iter->second;
	}

	player_ptr player(uintptr_t entity_ptr)
	{
		auto iter = player_by_ptr.find(entity_ptr);
		if (iter == player_by_ptr.end())
			return nullptr;

		return iter->second;
	}
	player_ptr player_id(int index)
	{
		auto iter = player_by_ptr.find(index);
		if (iter == player_by_ptr.end())
			return nullptr;

		return iter->second;
	}
};

extern csgo g_csgo;