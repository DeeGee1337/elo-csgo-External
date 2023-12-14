#include <Windows.h>
#include "CGuiInput.hpp"
#include "../csgo.hpp"

namespace cgui
{
	gui_key_state key_states[CGUI_MAX_KEY_STATES]; 
	HWND targetWnd;

	void init_input()
	{
		for (int i = 0; i < CGUI_MAX_KEY_STATES; i++)
		{
			key_states[i].is_down = false;
			key_states[i].last_state = false;
			key_states[i].last_up = 0;
			key_states[i].last_down = 0;
			key_states[i].once = false;
		}
	}

	void report_key_up(int key)
	{
		if (key >= CGUI_MAX_KEY_STATES)
			return;

		key_states[key].last_state = key_states[key].is_down;
		key_states[key].is_down = false;
		key_states[key].last_up = GetTickCount();

	}

	void report_key_down(int key)
	{
		if (key >= CGUI_MAX_KEY_STATES)
			return;

		key_states[key].last_state = key_states[key].is_down;


		if (!key_states[key].last_state)
		{
			key_states[key].is_down = true;
			key_states[key].once = false;
		}	
		
		key_states[key].last_down = GetTickCount();
	}

	bool is_key_down(int key, unsigned long timeout)
	{
		if (key >= CGUI_MAX_KEY_STATES)
			return false;
			
		bool res = key_states[key].is_down &&
			(GetTickCount() > key_states[key].last_down + timeout ||
				!key_states[key].last_state);

		return res;
	}

	bool is_key_down(int key)
	{
		if (key >= CGUI_MAX_KEY_STATES)
			return false;

		bool res = key_states[key].is_down;

		return res;
	}
	bool is_key_down_once(int key)
	{
		if (is_key_down(key))
		{
			if (!key_states[key].once)
			{
				key_states[key].once = true;
				return true;
			}
		}
		return false;
	}
	void update_input()
	{
		for (int i = 0; i < CGUI_MAX_KEY_STATES; i++)
		{
			auto key_state = (GetAsyncKeyState(i) & 0x8000);
			if(!key_state)
				key_state = (GetAsyncKeyState(i) & 1);
			
			key_state ? report_key_down(i) : report_key_up(i);
		}
	}
	void get_cursor_pos(int32_t & x, int32_t & y)
	{
		
		POINT cursor_pos;
		
		GetCursorPos(&cursor_pos);
		ScreenToClient(g_csgo.window, &cursor_pos);

		x = cursor_pos.x;
		y = cursor_pos.y;
	}
}