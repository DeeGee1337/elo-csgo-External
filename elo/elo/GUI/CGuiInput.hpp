#pragma once

#include "CGuiBase.hpp"

namespace cgui
{
	void init_input();
	void update_input();

	bool is_key_down(int key, unsigned long timeout);
	bool is_key_down(int key);
	bool is_key_down_once(int key);

	void get_cursor_pos(int32_t & x, int32_t & y);
}