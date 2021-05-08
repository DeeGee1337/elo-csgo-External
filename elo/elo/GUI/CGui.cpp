#include "CGui.hpp"
#include "CGuiInput.hpp"
#include "../csgo.hpp"
#include "../Colors.hpp"
#include "icons.hpp"


namespace cgui
{
	DWORD last_update = 0;
	DWORD vis_begin = 0;
	int	 vis_step;
	bool vis_change_in_progress;
	bool vis_fade_in;
	//gui_chr gBuildID[50];
	char* riAnsiConvert(const wchar_t* wstr)
	{
		int i = 0;

		static char tmpBuf[1000];

		while (wstr[i] != '\0')
		{
			tmpBuf[i] = (char)wstr[i];
			++i;
		}
		tmpBuf[i] = 0;

		return tmpBuf;
	}
	void crypt_str(char* str, char key)
	{
		/*	while (*str) {
				*str ^= (char)(key);
				str++;
			}*/
	}
	
	__forceinline DWORD menucol(DWORD col)
	{

		union colconv
		{
			struct
			{
				BYTE b;
				BYTE g;
				BYTE r;
				BYTE a;
			};
			DWORD color;
		};

		colconv c;
		c.color = col;

		if (vis_change_in_progress)
		{
			int step = vis_step;
			if (step > 100)
				step = 100;

			if (step == 0)
				step = 1;

			if (!vis_fade_in)
			{
				//fade out
				c.a = 120 - step;
			}
			else
			{
				//fade in
				c.a = 155 + step;
			}
		}

		return c.color;
	}
	DWORD menucol(BYTE a, BYTE r, BYTE g, BYTE b)
	{
		return menucol(MAKE_ARGB(a, r, g, b));
	}
	void fill_control(int32_t x, int32_t y, int32_t w, int32_t h, bool hovering = false)
	{

		fill_gradient(x, y, w, h, menucol(255, 50, 50, 50), menucol(255, 50, 50, 50), menucol(255, 20, 20, 20), menucol(255, 20, 20, 20));

		if (hovering)
		{
			draw_rect_outline(x, y, w, h, menucol(255, 236, 32, 38));
		}
	}
	void fill_control_active(int32_t x, int32_t y, int32_t w, int32_t h)
	{
		auto c1 = menucol(255, 220, 20, 60);
		auto c2 = menucol(255, 220, 20, 60);

		fill_gradient(x, y, w, h, c1, c1, c2, c2);
	}
	int	calc_width_pct(gui_wnd* wnd, float percent)
	{
		return (int)((wnd->ctrl->width * percent) - 15.0f/*padding content area*/);
	}
	int	calc_height_pct(gui_wnd* wnd, float percent)
	{
		return (int)((wnd->ctrl->heigth * percent) - 20.0f/*padding content area*/);
	}
	gui_wnd* window(gui_wnd* wnd, cgui_str title, int32_t x, int32_t y, int32_t w, int32_t h)
	{
		wnd->ctrl = new_gui_ctrl(wnd, title, ctrl_type::WINDOW, x, y, w, h);
		wnd->active_tab = 0;
		wnd->is_visible = true;
		wnd->str_xor_key = (char)((GetTickCount() % 20) + 0xA);
		wnd->tab_cnt = 0;

		wnd->is_moving = false;

		crypt_str(wnd->ctrl->text, wnd->str_xor_key);

		int32_t tickCount = GetTickCount();

		wnd->colors.main_background = MAKE_ARGB(245, 30, 31, 36);
		wnd->colors.main_border = MAKE_ARGB(255, 45, 45, 45);
		wnd->colors.tab_background = MAKE_ARGB(255, 49, 49, 49);
		wnd->colors.tab_border = MAKE_ARGB(255, 158, 158, 158);

		wnd->colors.active_hovering = MAKE_ARGB(255, 220, 20, 60);
		wnd->colors.title_border = MAKE_ARGB(255, 158, 158, 158);

		wnd->colors.tab_active_background = MAKE_ARGB(255, 27, 27, 27);
		wnd->colors.control_border = MAKE_ARGB(255, 158, 158, 158);

		wnd->colors.text_color = MAKE_ARGB(255, 255, 255, 255);
		wnd->colors.dark_background = MAKE_ARGB(255, 55, 57, 62);
		wnd->colors.content_color = MAKE_ARGB(255, 13, 59, 96);


		for (unsigned int i = 0; i < CGUI_MAX_TABS; i++)
		{
			wnd->tabs[i] = NULL;
		}

		wnd->tab_x_offset = 0;

		return wnd;
	}
	void incr_tab_h(gui_tab* tab, int ofs)
	{
		tab->y_offset += ofs;

		if (tab->grp)
			tab->grp->group_height += ofs;

		if (tab->y_offset > tab->wndheigth)
			tab->wndheigth = tab->y_offset;

	}
	void finish_init(gui_wnd* wnd)
	{
		for (int i = 0; i < wnd->tab_cnt; i++)
		{
			if (wnd->tabs[i])
			{
				auto tab = wnd->tabs[i];


			}
		}
	}

	void set_group(gui_tab* tab, gui_group* grp)
	{
		tab->grp = grp;
	}
	gui_sellist* sellist(gui_tab* tab, cgui_str text, int32_t x, int32_t y, int32_t* state_var, uint32_t maxopt, selchange_callback cb)
	{
		for (unsigned int i = 0; i < CGUI_MAX_LISTCTRLS; i++)
		{
			if (tab->sellists[i] == NULL)
			{
				tab->sellists[i] = new_gui_sellist(tab->wnd, text, x, y, state_var);
				crypt_str(tab->sellists[i]->ctrl->text, tab->wnd->str_xor_key);
				tab->sellists[i]->ctrl->width = 300;
				tab->sellists[i]->ctrl->heigth = 16;
				tab->sellists[i]->cb = cb;
				incr_tab_h(tab, maxopt * 16 + 10);
				return tab->sellists[i];
			}
		}
		return nullptr;
	}
	gui_sellist* auto_sellist(gui_tab* tab, cgui_str text, int32_t* state_var, uint32_t maxopt, selchange_callback cb)
	{
		return sellist(tab, text, tab->x_offset, tab->y_offset, state_var, maxopt, cb);
	}
	gui_group* group(gui_tab* tab, cgui_str text, int32_t x, int32_t y, int32_t w, int32_t h)
	{
		for (unsigned int i = 0; i < CGUI_MAX_GROUPS; i++)
		{
			if (tab->groups[i] == NULL)
			{
				auto group = new_gui_group(tab->wnd, text, x, y, w, h);
				crypt_str(group->ctrl->text, tab->wnd->str_xor_key);
				tab->groups[i] = group;
				tab->x_offset += 10;
				group->group_height = 0;

				set_group(tab, group);
				incr_tab_h(tab, 18);

				return tab->groups[i];
			}
		}
		return nullptr;
	}
	gui_tab* tab(gui_tab_view* tabview, gui_tab* tab, cgui_str text, ID3D11ShaderResourceView** icon)
	{
		uint32_t tab_count = 0;
		auto wnd = tabview->ctrl->wnd;
		tabview->active_tab = 0;

		tab->y_offset = tabview->ctrl->y + CGUI_TABVIEW_TAB_HEIGTH + 12;

		for (unsigned int i = 0; i < ARRAYSIZE(tabview->tabs); i++) {
			tab_count++;
			if (tabview->tabs[i] == NULL) {
				tabview->tabs[i] = tab;
				tabview->tab_cnt++;

				tab->y = 0;
				tab->wnd = wnd;
				tabview->tab_x_offset = 0;

				if (icon)
					tab->icon = icon;
				else
					tab->icon = nullptr;

				set_group(tab, nullptr);
				CGUI_STRCPY(tab->text, text);
				crypt_str(tab->text, wnd->str_xor_key);

				int32_t tab_w = (int32_t)((float)tabview->ctrl->width / (float)(tab_count));

				gui_tab* last_tab = nullptr;
				for (unsigned int x = 0; x < CGUI_MAX_TABVIEW_TABS; x++)
				{
					auto curr_tab = tabview->tabs[x];
					if (!curr_tab)
						break;

					for (int i = 0; i < ARRAYSIZE(curr_tab->buttons); i++)
						curr_tab->buttons[i] = NULL;
					for (int i = 0; i < ARRAYSIZE(curr_tab->checkboxes); i++)
						curr_tab->checkboxes[i] = NULL;
					for (int i = 0; i < ARRAYSIZE(curr_tab->colorslider); i++)
						curr_tab->colorslider[i] = NULL;
					for (int i = 0; i < ARRAYSIZE(curr_tab->comboboxes); i++)
						curr_tab->comboboxes[i] = NULL;
					for (int i = 0; i < ARRAYSIZE(curr_tab->groups); i++)
						curr_tab->groups[i] = NULL;
					for (int i = 0; i < ARRAYSIZE(curr_tab->hotkeys); i++)
						curr_tab->hotkeys[i] = NULL;
					for (int i = 0; i < ARRAYSIZE(curr_tab->slider); i++)
						curr_tab->slider[i] = NULL;
					for (int i = 0; i < ARRAYSIZE(curr_tab->tabviews); i++)
						curr_tab->tabviews[i] = NULL;
					for (int i = 0; i < ARRAYSIZE(curr_tab->sellists); i++)
						curr_tab->sellists[i] = NULL;

					last_tab = curr_tab;
					curr_tab->x = tabview->tab_x_offset;
					curr_tab->heigth = CGUI_TABVIEW_TAB_HEIGTH;
					curr_tab->width = tab_w;

					//tab->width = CGUI_TABVIEW_TAB_WIDTH;
					tabview->tab_x_offset += tab_w;
				}
				if (last_tab)
					last_tab->width += 1;

				return tab;
			}
		}
		return nullptr;
	}
	void process_label(const char* text, gui_tab* tab)
	{
		int w, h;
		cgui::get_text_size(text, w, h);

		if (w > tab->label_max_offset)
			tab->label_max_offset = w + 20;
	}
	gui_tab* tab(gui_wnd* wnd, gui_tab* tab, cgui_str text, ID3D11ShaderResourceView** icon)
	{
		uint32_t tab_count = 0;

		for (unsigned int i = 0; i < CGUI_MAX_TABS; i++)
		{
			tab_count++;

			if (wnd->tabs[i] == NULL)
			{
				tab->label_max_offset = 140;//min offset
				//tab = new_gui_tab(wnd, text);
				CGUI_STRCPY(tab->text, text);
				//tab->y =  -CGUI_TAB_HEIGTH;
				tab->y = -CGUI_TAB_HEIGTH;
				tab->wnd = wnd;
				tab->heigth = CGUI_TAB_HEIGTH;

				//	wnd->tab_x_offset = 0;
				wnd->tabs[i] = tab;
				wnd->tab_cnt++;
				tab->y_offset = 12;
				tab->wndheigth = 12;
				tab->x_offset = 10;

				if (icon)
					tab->icon = icon;
				else
					tab->icon = nullptr;



				for (int i = 0; i < ARRAYSIZE(tab->buttons); i++)
					tab->buttons[i] = NULL;
				for (int i = 0; i < ARRAYSIZE(tab->checkboxes); i++)
					tab->checkboxes[i] = NULL;
				for (int i = 0; i < ARRAYSIZE(tab->colorslider); i++)
					tab->colorslider[i] = NULL;
				for (int i = 0; i < ARRAYSIZE(tab->comboboxes); i++)
					tab->comboboxes[i] = NULL;
				for (int i = 0; i < ARRAYSIZE(tab->groups); i++)
					tab->groups[i] = NULL;
				for (int i = 0; i < ARRAYSIZE(tab->hotkeys); i++)
					tab->hotkeys[i] = NULL;
				for (int i = 0; i < ARRAYSIZE(tab->slider); i++)
					tab->slider[i] = NULL;
				for (int i = 0; i < ARRAYSIZE(tab->tabviews); i++)
					tab->tabviews[i] = NULL;
				for (int i = 0; i < ARRAYSIZE(tab->sellists); i++)
					tab->sellists[i] = NULL;
				for (int i = 0; i < ARRAYSIZE(tab->spacer); i++)
					tab->spacer[i] = NULL;

				int w, h;
				get_text_size(tab->text, w, h);
				tab->x = wnd->tab_x_offset;
				tab->width = w + 40;
				wnd->tab_x_offset += tab->width;

				crypt_str(tab->text, wnd->str_xor_key);
				/*	int32_t tab_w = (int32_t)((float)wnd->ctrl->width / (float)tab_count);

					for (unsigned int x = 0; x < CGUI_MAX_TABS; x++)
					{
						auto curr_tab = wnd->tabs[x];
						if (!curr_tab)
							break;


						curr_tab->x = wnd->tab_x_offset;
						curr_tab->width = tab_w;
						wnd->tab_x_offset += tab_w;
					}*/

				return tab;

			}
		}

		return nullptr;
	}
	gui_checkbox* checkbox(gui_tab* tab, cgui_str text, int32_t x, int32_t y, int32_t* state_var)
	{
		for (unsigned int i = 0; i < CGUI_MAX_CHECKBOXES; i++)
		{
			if (tab->checkboxes[i] == NULL)
			{
				//calc width and heigth

				int w = 0;
				int h = 0;
				get_text_size(text, w, h);

				int32_t chkbox_w = 16;
				int32_t chkbox_h = 16;

				process_label(text, tab);

				tab->checkboxes[i] = new_gui_checkbox(tab->wnd, text, x, y, chkbox_w, chkbox_h, state_var);
				crypt_str(tab->checkboxes[i]->ctrl->text, tab->wnd->str_xor_key);
				incr_tab_h(tab, 26);
				return tab->checkboxes[i];
			}
		}
		return nullptr;
	}
	gui_slider* slider(gui_tab* tab, cgui_str text, int32_t x, int32_t y, int32_t begin, int32_t end, cgui_str unit, int32_t* state_var)
	{
		for (unsigned int i = 0; i < CGUI_MAX_SLIDERS; i++)
		{
			if (tab->slider[i] == NULL)
			{
				auto new_slider = new_gui_slider(tab->wnd, text, x, y, begin, end, unit, state_var);

				crypt_str(new_slider->ctrl->text, tab->wnd->str_xor_key);

				new_slider->ctrl->width = 300;
				new_slider->ctrl->heigth = 18;
				new_slider->percentage_value = 0;
				process_label(text, tab);
				tab->slider[i] = new_slider;
				incr_tab_h(tab, 26);
				return tab->slider[i];
			}
		}
		return nullptr;
	}
	gui_colorslider* colorslider(gui_tab* tab, cgui_str text, int32_t x, int32_t y, uint32_t* state_var)
	{
		for (unsigned int i = 0; i < CGUI_MAX_SLIDERS; i++)
		{
			if (tab->colorslider[i] == NULL)
			{
				int w = 0;
				int h = 0;
				get_text_size(text, w, h);
				process_label(text, tab);
				int32_t color_w = 15 + 21 + w;
				int32_t color_h = 16;

				tab->colorslider[i] = new_gui_colorslider(tab->wnd, text, x, y, color_w, color_h, state_var);
				crypt_str(tab->colorslider[i]->ctrl->text, tab->wnd->str_xor_key);

				incr_tab_h(tab, 26);
				return tab->colorslider[i];
			}
		}
		return nullptr;
	}
	gui_hotkey* hotkey(gui_tab* tab, cgui_str text, int32_t x, int32_t y, int32_t* state_var)
	{
		for (unsigned int i = 0; i < CGUI_MAX_HOTKEYS; i++)
		{
			if (tab->hotkeys[i] == NULL)
			{
				process_label(text, tab);
				tab->hotkeys[i] = new_gui_hotkey(tab->wnd, text, x, y, state_var);
				crypt_str(tab->hotkeys[i]->ctrl->text, tab->wnd->str_xor_key);
				tab->hotkeys[i]->ctrl->width = 300;
				tab->hotkeys[i]->ctrl->heigth = 18;

				incr_tab_h(tab, 26);
				return tab->hotkeys[i];
			}
		}
		return nullptr;
	}
	void spacer(gui_tab* tab, int32_t x, int32_t y, int32_t spacer_top, int32_t spacer_bot)
	{
		for (unsigned int i = 0; i < CGUI_MAX_SPACER; i++)
		{
			if (tab->spacer[i] == NULL)
			{
				tab->spacer[i] = new_spacer(tab->wnd, x, y);
				tab->spacer[i]->ctrl->width = tab->grp ? (tab->grp->ctrl->width - x) : (tab->wnd->ctrl->width - x - 10);
				tab->spacer[i]->ctrl->heigth = spacer_top + spacer_bot + 1;

				incr_tab_h(tab,  spacer_bot + 1);
				return;
			}
		}
	}
	void auto_spacer(gui_tab* tab, int32_t spacer_top, int32_t spacer_bot)
	{
		return spacer(tab, tab->x_offset, tab->y_offset, spacer_top, spacer_bot);
	}
	gui_button* button(gui_tab* tab, cgui_str text, int32_t x, int32_t y, button_callback callback)
	{
		for (unsigned int i = 0; i < CGUI_MAX_BUTTONS; i++)
		{
			if (tab->buttons[i] == NULL)
			{
				process_label(text, tab);
				tab->buttons[i] = new_gui_button(tab->wnd, text, x, y, callback);
				crypt_str(tab->buttons[i]->ctrl->text, tab->wnd->str_xor_key);
				tab->buttons[i]->ctrl->width = 145;
				tab->buttons[i]->ctrl->heigth = 25;

				//incr_tab_h(tab, 38);
				return tab->buttons[i];
			}
		}
		return nullptr;
	}
	gui_combobox* combobox(gui_tab* tab, cgui_str text, int32_t x, int32_t y, const cgui_str* options, uint32_t option_count, int32_t* state_var, uint32_t min_width)
	{
		for (unsigned int i = 0; i < CGUI_MAX_COMBOBOXES; i++)
		{
			if (tab->comboboxes[i] == NULL)
			{
				process_label(text, tab);
				auto combobox = new_gui_combobox(tab->wnd, text, x, y, options, option_count, state_var);

				for (uint32_t i = 0; i < option_count; i++)
				{
					if (i >= CGUI_MAX_COMBOBOX_OPTS)
						break;

					crypt_str(combobox->options[i], tab->wnd->str_xor_key);
				}

				crypt_str(combobox->ctrl->text, tab->wnd->str_xor_key);
				tab->comboboxes[i] = combobox;

				combobox->ctrl->heigth = 18;


				//int32_t longest_option = 0;
				//for (unsigned int i = 0; i < option_count; i++)
				//{
				//	auto & current_opt = options[i];

				//	int h, w;
				//	get_text_size(current_opt, w, h);
				//	if (w > longest_option)
				//		longest_option = w;
				//}

				int32_t text_w, text_h;
				get_text_size(text, text_w, text_h);
				combobox->opt_list_width = 300;// longest_option + 20 + 10;

				if (combobox->opt_list_width < min_width)
					combobox->opt_list_width = min_width;

				combobox->ctrl->width = combobox->opt_list_width;

				incr_tab_h(tab, 26);
				return combobox;
			}
		}
		return nullptr;
	}
	gui_tab_view* tab_view(gui_tab_view* newtabview, gui_tab* tab, cgui_str text, int32_t x, int32_t y, uint32_t w, uint32_t h)
	{
		for (unsigned int i = 0; i < CGUI_MAX_TABVIEWS; i++)
		{
			if (tab->tabviews[i] == NULL)
			{
				init_tabview(newtabview, tab->wnd, text, x, y, w, h);
				crypt_str(newtabview->ctrl->text, tab->wnd->str_xor_key);
				tab->tabviews[i] = newtabview;
				return tab->tabviews[i];
			}
		}
		return nullptr;
	}
	void reset_y(gui_tab* tab)
	{
		tab->y_offset = 12;
	}
	void set_x(gui_tab* tab, int32_t w)
	{
		tab->x_offset = w;
	}
	void sync_x_to_group(gui_tab* tab, gui_group* grp)
	{
		tab->x_offset = grp->ctrl->x + 10;
	}
	gui_checkbox* auto_checkbox(gui_tab* tab, cgui_str text, int32_t* state_var)
	{
		return checkbox(tab, text, tab->x_offset, tab->y_offset, state_var);
	}
	gui_group* auto_group(gui_tab* tab, cgui_str text, int32_t x, int32_t w, int32_t h)
	{
		return group(tab, text, x, tab->y_offset, w, h);
	}
	gui_slider* auto_slider(gui_tab* tab, cgui_str text, int32_t begin, int32_t end, cgui_str unit, int32_t* state_var)
	{
		return slider(tab, text, tab->x_offset, tab->y_offset, begin, end, unit, state_var);
	}
	gui_hotkey* auto_hotkey(gui_tab* tab, cgui_str text, int32_t* state_var)
	{
		return hotkey(tab, text, tab->x_offset, tab->y_offset, state_var);
	}
	gui_combobox* auto_combobox(gui_tab* tab, cgui_str text, const cgui_str* options, uint32_t option_count, int32_t* state_var, uint32_t min_width)
	{
		return combobox(tab, text, tab->x_offset, tab->y_offset, options, option_count, state_var, min_width);
	}
	gui_colorslider* auto_colorslider(gui_tab* tab, cgui_str text, uint32_t* state_var)
	{
		return colorslider(tab, text, tab->x_offset, tab->y_offset, state_var);
	}
	gui_button* auto_button(gui_tab* tab, cgui_str text, button_callback callback)
	{
		return button(tab, text, tab->x_offset, tab->y_offset, callback);
	}

	bool is_mouse_over_ctrl(gui_ctrl* ctrl)
	{
		if (!ctrl)
			return false;

		int32_t cursor_x;
		int32_t cursor_y;
		get_cursor_pos(cursor_x, cursor_y);

		if (!ctrl->wnd)
			return false;

		auto wnd_x = ctrl->wnd->ctrl->x + ctrl->wnd->tabs[ctrl->wnd->active_tab]->label_max_offset;
		auto wnd_y = ctrl->wnd->ctrl->y;

		if(ctrl->type == ctrl_type::LISTCTRL || ctrl->type == ctrl_type::BUTTON)
			wnd_x = ctrl->wnd->ctrl->x;

		if (cursor_x >= ctrl->x + wnd_x &&
			cursor_x <= ctrl->x + wnd_x + ctrl->width &&
			cursor_y >= ctrl->y + wnd_y &&
			cursor_y <= ctrl->y + wnd_y + ctrl->heigth)
		{
			return true;
		}

		return false;
	}

	int slider_translate(gui_slider* slider, int percent)
	{
		int range = slider->end - slider->begin;
		return slider->begin + (int)((float)((float)range / ((float)slider->ctrl->width)) * (float)percent);
	}
	int slider_retranslate(gui_slider* slider, int relative)
	{
		if (relative < slider->begin)
			relative = slider->begin;

		int range = slider->end - slider->begin;
		int rel = relative - slider->begin;

		return (int)(((float)rel) / ((float)range) * ((float)slider->ctrl->width));
	}

	void update_title(gui_wnd* wnd)
	{
		int32_t titlebar_x = wnd->ctrl->x;;
		int32_t titlebar_y = wnd->ctrl->y - CGUI_TITLEBAR_HEIGTH - CGUI_TAB_HEIGTH;
		auto titlebar_w = wnd->ctrl->width;
		auto titlebar_h = CGUI_TITLEBAR_HEIGTH;


		int32_t cursor_x;
		int32_t cursor_y;
		get_cursor_pos(cursor_x, cursor_y);

		bool lbutton_down = is_key_down(VK_LBUTTON);

		if (!wnd->is_moving)
		{
			if (cursor_x >= titlebar_x &&
				cursor_x <= titlebar_x + titlebar_w &&
				cursor_y >= titlebar_y &&
				cursor_y <= titlebar_y + titlebar_h)
			{
				if (lbutton_down)
				{
					wnd->move_delta_x = 0;
					wnd->move_delta_y = 0;
					wnd->is_moving = true;

					wnd->move_cur_x = cursor_x;
					wnd->move_cur_y = cursor_y;
				}
			}
		}
		if (!lbutton_down)
		{
			wnd->is_moving = false;
		}

		if (wnd->is_moving &&
			(cursor_x != wnd->move_cur_x ||
				cursor_y != wnd->move_cur_y))
		{
			wnd->move_delta_x = cursor_x - wnd->move_cur_x;
			wnd->move_delta_y = cursor_y - wnd->move_cur_y;

			wnd->move_cur_x = cursor_x;
			wnd->move_cur_y = cursor_y;


			auto newX = wnd->ctrl->x + wnd->move_delta_x;
			auto newY = wnd->ctrl->y + wnd->move_delta_y;


			wnd->ctrl->x = (newX);
			wnd->ctrl->y = (newY);

			wnd->move_delta_x = cursor_x;
			wnd->move_delta_y = cursor_y;
		}

		if (wnd->ctrl->y + wnd->tabs[wnd->active_tab]->wndheigth + 10 > g_csgo.surface->get_height())
		{
			wnd->ctrl->y = g_csgo.surface->get_height() - (wnd->tabs[wnd->active_tab]->wndheigth + 10);
		}
		if (wnd->ctrl->y < CGUI_TITLEBAR_HEIGTH + CGUI_TAB_HEIGTH)
		{
			wnd->ctrl->y = CGUI_TITLEBAR_HEIGTH + CGUI_TAB_HEIGTH;
		}
		if (wnd->ctrl->x + wnd->ctrl->width > g_csgo.surface->get_width())
		{
			wnd->ctrl->x = g_csgo.surface->get_width() - wnd->ctrl->width;
		}
		if (wnd->ctrl->x < 0)
		{
			wnd->ctrl->x = 0;
		}

	}
	void render_title(gui_wnd* wnd)
	{
		int32_t titlebar_x = wnd->ctrl->x;
		int32_t titlebar_y = wnd->ctrl->y - CGUI_TITLEBAR_HEIGTH - CGUI_TAB_HEIGTH;

		fill_rect(titlebar_x + 1, titlebar_y + 1, wnd->ctrl->width - 2, CGUI_TITLEBAR_HEIGTH - 1 + CGUI_TAB_HEIGTH, menucol(255, 20, 20, 20));
		//draw_rect_outline(titlebar_x, titlebar_y, wnd->ctrl->width, wnd->ctrl->heigth, wnd->colors.title_border);

		//draw_img(titlebar_x + 5, titlebar_y + 1, CGUI_TITLEBAR_HEIGTH - 2, CGUI_TITLEBAR_HEIGTH - 2, icons::g_mcIcon, colors::White);

		//crypt_str(gBuildID, wnd->str_xor_key);
		crypt_str(wnd->ctrl->text, wnd->str_xor_key);

		gui_chr full_title[200];
		CGUI_SPRINTF(full_title, 200, CGUI_STR("%s"), wnd->ctrl->text);

		//crypt_str(gBuildID, wnd->str_xor_key);
		crypt_str(wnd->ctrl->text, wnd->str_xor_key);


		draw_text(titlebar_x + 5 /*+ (CGUI_TITLEBAR_HEIGTH - 2) + 5*//* ((wnd->ctrl->width) / 2)*/, titlebar_y + ((CGUI_TITLEBAR_HEIGTH / 4)), full_title, menucol(255, 220, 20, 60));
	}
	void update_tabview(gui_wnd* wnd, gui_tab_view* tabview)
	{
		int32_t cursor_x;
		int32_t cursor_y;


		get_cursor_pos(cursor_x, cursor_y);

		for (int i = 0; i < tabview->tab_cnt; i++) {
			auto current_tab = tabview->tabs[i];
			if (!current_tab)
				break;

			int tab_x = wnd->ctrl->x + tabview->ctrl->x + current_tab->x;
			int tab_y = wnd->ctrl->y + tabview->ctrl->y + current_tab->y;

			if (cursor_x >= tab_x &&
				cursor_x <= (tab_x + current_tab->width) &&
				cursor_y >= tab_y &&
				cursor_y <= (tab_y + current_tab->heigth))
			{
				current_tab->mouse_hover = true;
			}
			else
			{
				current_tab->mouse_hover = false;
			}

			if (current_tab->mouse_hover)
			{
				if (wnd->lk_down_once)
				{
					tabview->active_tab = i;
				}
			}
		}
	}
	void update_tabs(gui_wnd* wnd)
	{
		for (unsigned int i = 0; i < CGUI_MAX_TABS; i++)
		{
			auto current_tab = wnd->tabs[i];
			if (current_tab)
			{
				int32_t cursor_x;
				int32_t cursor_y;

				get_cursor_pos(cursor_x, cursor_y);

				int tab_x = wnd->ctrl->x + current_tab->x;
				int tab_y = wnd->ctrl->y + current_tab->y;

				if (cursor_x >= tab_x &&
					cursor_x <= (tab_x + current_tab->width) &&
					cursor_y >= tab_y &&
					cursor_y <= (tab_y + current_tab->heigth))
				{
					current_tab->mouse_hover = true;
				}
				else
				{
					current_tab->mouse_hover = false;
				}

				if (current_tab->mouse_hover)
				{
					if (wnd->lk_down_once)
					{
						wnd->active_tab = i;
					}
				}
			}
		}
	}
	void render_tabs(gui_wnd* wnd)
	{
		int total_w = 0;
		//render tab frame
		for (unsigned int i = 0; i < CGUI_MAX_TABS; i++)
		{
			auto current_tab = wnd->tabs[i];
			if (current_tab)
			{
				int tab_x = wnd->ctrl->x + current_tab->x + 1;
				int tab_y = wnd->ctrl->y + current_tab->y;

				crypt_str(current_tab->text, wnd->str_xor_key);
				int32_t text_w;
				int32_t text_h;
				get_text_size(current_tab->text, text_w, text_h);

				int32_t tab_w = current_tab->width;

				if (total_w + tab_w >= wnd->ctrl->width - 2)
				{
					tab_w = wnd->ctrl->width - total_w - 2;
				}

				total_w += tab_w;
				auto tab_background = wnd->colors.tab_background;
				if (i == wnd->active_tab || current_tab->mouse_hover)
				{
					fill_gradient(tab_x, tab_y, tab_w, current_tab->heigth, menucol(MAKE_ARGB(255, 53, 53, 53)), menucol(MAKE_ARGB(255, 53, 53, 53)), menucol(MAKE_ARGB(255, 25, 25, 25)), menucol(MAKE_ARGB(255, 25, 25, 25)));

				}
				else
					fill_rect(tab_x, tab_y, tab_w, current_tab->heigth, menucol(MAKE_ARGB(255, 20, 20, 20)));
				{
					//fill_gradient(tab_x, tab_y, tab_w, current_tab->heigth, MAKE_ARGB(255, 66, 66, 66), MAKE_ARGB(255, 66, 66, 66), MAKE_ARGB(255, 53, 53, 53), MAKE_ARGB(255, 53, 53, 53));
				}
				if (i == wnd->active_tab)
				{
					fill_rect(tab_x, tab_y + CGUI_TAB_HEIGTH - 2, tab_w, 2, menucol(MAKE_ARGB(255, 220, 20, 60)));
				}
				else
				{
					fill_rect(tab_x, tab_y + CGUI_TAB_HEIGTH - 2, tab_w, 2, menucol(MAKE_ARGB(255, 20, 20, 20)));
				}
				//draw_rect_outline(tab_x, tab_y, tab_w, current_tab->heigth, wnd->colors.main_border);

				auto iconW = 18;
				auto iconH = 18;

				auto iconX = tab_x + 5;
				auto iconY = tab_y + (current_tab->heigth / 2);

				/*	if (current_tab->icon)
					{
						draw_img(iconX, iconY - 10, iconW, iconH, *((ID3D11ShaderResourceView**) current_tab->icon), colors::White);
					}
					*/

				draw_text_c(tab_x + (current_tab->width / 2), tab_y + (current_tab->heigth / 2), current_tab->text, menucol(MAKE_ARGB(255, 255, 255, 255)));
				crypt_str(current_tab->text, wnd->str_xor_key);
			}
			else break;
		}
	}

	void render_group(gui_wnd* wnd, gui_group* group)
	{
		int w, h;

		auto group_x = wnd->ctrl->x + group->ctrl->x;
		auto group_y = wnd->ctrl->y + group->ctrl->y;

		auto group_w = group->ctrl->width;
		auto group_h = group->group_height;

		crypt_str(group->ctrl->text, wnd->str_xor_key);
		get_text_size(group->ctrl->text, w, h);

		//auto border_color = group->ctrl->mouse_hover ? group->ctrl->wnd->colors.text_color : group->ctrl->wnd->colors.hover_color;
		auto border_color = menucol(MAKE_ARGB(255, 0, 0, 0));
		//fill_gradient(group_x, group_y, group_w, group_h, MAKE_ARGB(180, 2, 43, 87), MAKE_ARGB(180, 1, 17, 35), MAKE_ARGB(180, 2, 43, 87), MAKE_ARGB(180, 1, 17, 35));
		draw_line(group_x, group_y, group_x + 4, group_y, border_color, 0.5f);


		draw_line(group_x + 8 + w + 3, group_y, group_x + group_w, group_y, border_color, 0.5f);

		draw_line(group_x, group_y, group_x, group_y + group_h, border_color, 0.5f);
		draw_line(group_x, group_y + group_h, group_x + group_w, group_y + group_h, border_color, 0.5f);
		draw_line(group_x + group_w, group_y + group_h, group_x + group_w, group_y, border_color, 0.5f);

		draw_text(group_x + 8, group_y - (h / 2), group->ctrl->text, menucol(wnd->colors.text_color));
		crypt_str(group->ctrl->text, wnd->str_xor_key);
	}
	void update_checkbox(gui_wnd* wnd, gui_checkbox* curr_checkbox)
	{
		if (is_mouse_over_ctrl(curr_checkbox->ctrl))
		{
			//CORE_LOG_EX("OVER CHECK");
			if (wnd->lk_down_once)
			{
				int old = *curr_checkbox->state_var;

				*curr_checkbox->state_var = *curr_checkbox->state_var ? 0 : 1;

				//CORE_LOG_EX("DOWN CHECK old %i new %i", old, *curr_checkbox->state_var);
			}
		}
	}
	void render_label(gui_wnd* wnd,cgui::gui_ctrl* ctrl, int x, int y, char* text)
	{

		draw_text(x, y, text,ctrl->mouse_hover ? menucol(255, 236, 32, 38) : menucol(wnd->colors.text_color));
	}
	void render_checkbox(gui_wnd* wnd, gui_checkbox* curr_checkbox)
	{
		auto color = wnd->colors.active_hovering;

		auto chk_x = wnd->ctrl->x + curr_checkbox->ctrl->x;
		auto chk_y = wnd->ctrl->y + curr_checkbox->ctrl->y;

		auto chk_fieldsize = curr_checkbox->ctrl->heigth;



		crypt_str(curr_checkbox->ctrl->text, wnd->str_xor_key);

		int tw, th;
		cgui::get_text_size(curr_checkbox->ctrl->text, tw, th);

		render_label(wnd, curr_checkbox->ctrl, chk_x, chk_y, curr_checkbox->ctrl->text);

		if (*curr_checkbox->state_var) {
			//draw_text(chk_x + 3 + wnd->tabs[wnd->active_tab]->label_max_offset, chk_y, "X", curr_checkbox->ctrl->wnd->colors.content_color);

			//fill_control_active(chk_x +3 + wnd->tabs[wnd->active_tab]->label_max_offset, chk_y + 3, chk_fieldsize - 6, chk_fieldsize - 6);
			fill_control_active(chk_x + wnd->tabs[wnd->active_tab]->label_max_offset, chk_y, chk_fieldsize, chk_fieldsize);
		}
		else
		{
			fill_control(chk_x + wnd->tabs[wnd->active_tab]->label_max_offset, chk_y, chk_fieldsize, chk_fieldsize, curr_checkbox->ctrl->mouse_hover);
		}
		//	draw_rect_outline(chk_x, chk_y, chk_fieldsize, chk_fieldsize, curr_checkbox->ctrl->mouse_hover ? wnd->colors.active_hovering : wnd->colors.control_border);

		crypt_str(curr_checkbox->ctrl->text, wnd->str_xor_key);
	}
	void update_slider(gui_wnd* wnd, gui_slider* curr_slider)
	{
		if (curr_slider->is_sliding)
		{
			/*if (!is_mouse_over_ctrl(curr_slider->ctrl))
				curr_slider->is_sliding = false;*/

			if (!is_key_down(VK_LBUTTON))
				curr_slider->is_sliding = false;
		}
		else
		{
			if (is_mouse_over_ctrl(curr_slider->ctrl))
			{
				if (is_key_down_once(VK_RBUTTON))
					*curr_slider->state_var = curr_slider->begin;

				if (wnd->lk_down_once)
					curr_slider->is_sliding = true;
			}
		}

		if (curr_slider->is_sliding)
		{
			int32_t cursor_x;
			int32_t cursor_y;
			get_cursor_pos(cursor_x, cursor_y);

			auto slider_x = wnd->ctrl->x + curr_slider->ctrl->x + wnd->tabs[wnd->active_tab]->label_max_offset;

			int slider_delta_x = cursor_x - slider_x;

			if (slider_delta_x < 0)
				slider_delta_x = 0;

			if (slider_delta_x > curr_slider->ctrl->width)
				slider_delta_x = curr_slider->ctrl->width;

			//	curr_slider->percentage_value = slider_delta_x;
			*curr_slider->state_var = slider_translate(curr_slider, slider_delta_x);
		}
	}
	void base_update(gui_ctrl* ctrl)
	{
		if (!ctrl)
			return;

		if (is_mouse_over_ctrl(ctrl))
		{
			ctrl->mouse_hover = true;
		}
		else
		{
			ctrl->mouse_hover = false;
		}
	}
	void update_colorslider(gui_wnd* wnd, gui_colorslider* curr_slider)
	{
		auto new_col = *curr_slider->state_var;

		if (!curr_slider->is_open)
		{
			if (is_mouse_over_ctrl(curr_slider->ctrl))
			{
				if (wnd->lk_down_once)
				{
					curr_slider->is_open = true;
				}
			}
		}
		else
		{
			if (cgui::is_key_down(VK_ESCAPE))
			{
				curr_slider->is_open = false;
				return;
			}

			auto cSliderX = wnd->ctrl->x + wnd->ctrl->width + 5;
			auto cSliderY = wnd->ctrl->y - CGUI_TITLEBAR_HEIGTH - CGUI_TAB_HEIGTH;
			auto cSliderW = 176 * 2;
			auto cSliderH = wnd->ctrl->heigth + CGUI_TITLEBAR_HEIGTH + CGUI_TAB_HEIGTH;

			int32_t cX, cY;
			cgui::get_cursor_pos(cX, cY);

			if (cX <= cSliderX || cX >= cSliderX + cSliderW ||
				cY <= cSliderY || cY >= cSliderY + cSliderH)
			{
				if (wnd->lk_down_once)
					curr_slider->is_open = false;
			}
		}

	}

	void render_slider(gui_wnd* wnd, gui_slider* curr_slider)
	{
		crypt_str(curr_slider->ctrl->text, wnd->str_xor_key);
		auto slider_x = wnd->ctrl->x + curr_slider->ctrl->x;
		auto slider_y = wnd->ctrl->y + curr_slider->ctrl->y;



		fill_control(wnd->tabs[wnd->active_tab]->label_max_offset + slider_x, slider_y, curr_slider->ctrl->width, curr_slider->ctrl->heigth, curr_slider->ctrl->mouse_hover);

		auto percentage_val = slider_retranslate(curr_slider, *curr_slider->state_var);
		//g_direct3D.fillRect(getPositionX(), getPositionY(), 100, 15,getWindow()->getBackgroundColor());



		//draw_rect_outline(slider_x, slider_y , curr_slider->ctrl->width, curr_slider->ctrl->heigth, curr_slider->ctrl->mouse_hover ? wnd->colors.active_hovering : wnd->colors.control_border);
		if (percentage_val) {
			fill_control_active(wnd->tabs[wnd->active_tab]->label_max_offset + slider_x, slider_y + 1, percentage_val, curr_slider->ctrl->heigth - 2);
		}
		/*fill_rect(slider_x + percentage_val - 2, slider_y + 1 , 5, 3, MAKE_ARGB(255,0,0,0));
		fill_rect(slider_x + percentage_val - 2 + 1, slider_y + 1, 3, curr_slider->ctrl->heigth, MAKE_ARGB(255, 0, 0, 0));
		fill_rect(slider_x + percentage_val - 2, slider_y + 1 + curr_slider->ctrl->heigth, 5, 3, MAKE_ARGB(255, 0, 0, 0));*/

		//draw_text_c(slider_x + (100 / 2), slider_y + (curr_slider->ctrl->heigth / 4) - 3, final_value, MAKE_ARGB(255,255,255,255));

		gui_chr final_value[255];
		if (curr_slider->unit[0]) {
			CGUI_SPRINTF(final_value, 255, CGUI_STR("%i %s"), *curr_slider->state_var, curr_slider->unit);
		}
		else {

			CGUI_SPRINTF(final_value, 255, CGUI_STR("%i"), *curr_slider->state_var);
		}
		render_label(wnd,curr_slider->ctrl, slider_x, slider_y, curr_slider->ctrl->text);
		draw_text(wnd->tabs[wnd->active_tab]->label_max_offset + slider_x + 4, slider_y, final_value, menucol(wnd->colors.text_color));

		crypt_str(curr_slider->ctrl->text, wnd->str_xor_key);
	}
	void render_colorslider(gui_wnd* wnd, gui_colorslider* curr_slider)
	{
		auto color = wnd->colors.active_hovering;
		
		auto chk_x = wnd->ctrl->x + curr_slider->ctrl->x + wnd->tabs[wnd->active_tab]->label_max_offset;
		auto chk_y = wnd->ctrl->y + curr_slider->ctrl->y;

		auto chk_fieldsize = curr_slider->ctrl->heigth;

		fill_rect(chk_x, chk_y, chk_fieldsize, chk_fieldsize, menucol(*curr_slider->state_var));


		if(curr_slider->ctrl->mouse_hover)
			draw_rect_outline(chk_x, chk_y, chk_fieldsize, chk_fieldsize, menucol(wnd->colors.active_hovering));

		crypt_str(curr_slider->ctrl->text, wnd->str_xor_key);

		int tw, th;
		cgui::get_text_size(curr_slider->ctrl->text, tw, th);

		//draw_text(wnd->ctrl->x + curr_slider->ctrl->x, chk_y + (curr_slider->ctrl->heigth / 2) - (th / 2), curr_slider->ctrl->text, menucol(wnd->colors.text_color));
		render_label(wnd, curr_slider->ctrl, wnd->ctrl->x + curr_slider->ctrl->x, chk_y + (curr_slider->ctrl->heigth / 2) - (th / 2), curr_slider->ctrl->text);

		if (curr_slider->is_open)
		{
			auto cSliderX = wnd->ctrl->x + wnd->ctrl->width - 2;
			auto cSliderY = wnd->ctrl->y - CGUI_TITLEBAR_HEIGTH - CGUI_TAB_HEIGTH + 1;
			auto cSliderW = 176 * 2;
			auto cSliderH = (wnd->tabs[wnd->active_tab]->wndheigth + CGUI_TITLEBAR_HEIGTH + CGUI_TAB_HEIGTH + 10) / 2;

			auto cPaletteX = cSliderX + 1;
			auto cPaletteY = cSliderY + CGUI_TITLEBAR_HEIGTH + 1;
			auto cPaletteW = cSliderW - 2;
			auto cPaletteH = cSliderH - CGUI_TITLEBAR_HEIGTH - 2;

			fill_rect(cSliderX, cSliderY, cSliderW, CGUI_TITLEBAR_HEIGTH + 1, menucol(255, 20, 20, 20));
			//draw_rect_outline(cSliderX, cSliderY, cSliderW, cSliderH, MAKE_ARGB(255, 0, 155, 230));

			char fullTxt[500];
			CGUI_SPRINTF(fullTxt, 500, "%s", curr_slider->ctrl->text);

			struct colstruct
			{
				union
				{
					struct
					{
						BYTE b;
						BYTE g;
						BYTE r;
						BYTE a;
					};
					DWORD col;
				};
			};

			colstruct cs;
			cs.col = *curr_slider->state_var;

			char rgbbuf[100];
			CGUI_SPRINTF(rgbbuf, 100, "(%i,%i,%i,%i)", (int) cs.a, (int)cs.r , (int)cs.g, (int)cs.b);

			//if(!vis_change_in_progress || vis_fade_in)
				//nuk_draw_color_picker(cPaletteX, cPaletteY, cPaletteW, cPaletteH, curr_slider->state_var, wnd->lk_down_once);

			draw_text(cSliderX + 25, cSliderY + (CGUI_TITLEBAR_HEIGTH / 4), fullTxt, menucol(255, 220, 20, 60));
			draw_text_r(cSliderX + cSliderW - 5, cSliderY +(CGUI_TITLEBAR_HEIGTH / 4), rgbbuf, menucol(255, 220, 20, 60)),

			fill_rect(cSliderX + 5, cSliderY + 8, 16, 16, menucol(*curr_slider->state_var));
		}
		crypt_str(curr_slider->ctrl->text, wnd->str_xor_key);

	}
	void update_hotkeybox(gui_wnd* wnd, gui_hotkey* curr_hotkey)
	{
		if (!curr_hotkey->is_selecting)
		{
			if (is_mouse_over_ctrl(curr_hotkey->ctrl))
			{
				if (wnd->lk_down_once)
				{
					curr_hotkey->is_selecting = true;
				}
				if (cgui::is_key_down_once(VK_RBUTTON))
					*curr_hotkey->state_var = 0;
			}
		}
		else
		{
			for (unsigned int i = 1; i < CGUI_MAX_KEY_STATES; i++)
			{
				if (is_key_down_once(i))
				{
					if (i == VK_ESCAPE)
						i = 0;

					*curr_hotkey->state_var = i;
					curr_hotkey->is_selecting = false;
					break;
				}
			}
		}
	}
	void render_hotkeybox(gui_wnd* wnd, gui_hotkey* curr_hotkey)
	{
		crypt_str(curr_hotkey->ctrl->text, wnd->str_xor_key);
		auto ctrl_x = wnd->ctrl->x + curr_hotkey->ctrl->x + wnd->tabs[wnd->active_tab]->label_max_offset;
		auto ctrl_y = wnd->ctrl->y + curr_hotkey->ctrl->y;

		fill_control(ctrl_x, ctrl_y, curr_hotkey->ctrl->width, curr_hotkey->ctrl->heigth);

		if (curr_hotkey->ctrl->mouse_hover)
			draw_rect_outline(ctrl_x, ctrl_y, curr_hotkey->ctrl->width, curr_hotkey->ctrl->heigth, wnd->colors.active_hovering);

		gui_chr txbuffer[255];
		if (!curr_hotkey->is_selecting)
		{
			if (*curr_hotkey->state_var != 0)
			{
				int scan = MapVirtualKey((UINT)*curr_hotkey->state_var, (UINT)0);

				// Build the generic lparam to be used for WM_KEYDOWN/WM_KEYUP/WM_CHAR
				LPARAM lparam = 0x00000001 | (LPARAM)(scan << 16);         // Scan code, repeat=1
																		   //lparam = lparam | 0x01000000;       // Extended code if required

				wchar_t wkey_name[200];
				gui_chr key_name[200];

				if (GetKeyNameTextW((LONG)lparam, wkey_name, (int)ARRAYSIZE(wkey_name)))
				{
					auto key_w = riAnsiConvert(wkey_name);
					strcpy(key_name, key_w);
				}
				else
				{
					CGUI_SPRINTF(key_name, 200, CGUI_STR("Key %i"), *curr_hotkey->state_var);
				}
#ifdef CGUI_USE_CHAR
#else
				wchar_t* key_name = &wkey_name[0];
#endif
				CGUI_SPRINTF(txbuffer, 200, CGUI_STR("%s"), key_name);
			}
			else
			{
				CGUI_SPRINTF(txbuffer, 200, CGUI_STR("None"));
			}
		}
		else
		{
			CGUI_SPRINTF(txbuffer, 200, CGUI_STR("Choose Key"));
		}
		char totalBuffer[500];
		CGUI_SPRINTF(totalBuffer, 500, "%s", txbuffer);

		render_label(wnd, curr_hotkey->ctrl, ctrl_x + 4, ctrl_y, totalBuffer);

		draw_text(wnd->ctrl->x + curr_hotkey->ctrl->x, wnd->ctrl->y + curr_hotkey->ctrl->y, curr_hotkey->ctrl->text, menucol(wnd->colors.text_color));
		crypt_str(curr_hotkey->ctrl->text, wnd->str_xor_key);
	}
	void update_combobox(gui_wnd* wnd, gui_combobox* curr_combobox)
	{
		int chkbox_w = curr_combobox->opt_list_width;
		int chkbox_h = curr_combobox->ctrl->heigth;

		int curr_item_x = curr_combobox->ctrl->x + wnd->ctrl->x;
		int curr_item_y = curr_combobox->ctrl->y + wnd->ctrl->y;
		int curr_item_w = curr_combobox->opt_list_width;
		int curr_item_h = curr_combobox->ctrl->heigth;


		auto chkbox_opth = 18;

		int item_list_x = curr_item_x + wnd->tabs[wnd->active_tab]->label_max_offset;
		int item_list_y = curr_item_y + curr_item_h;
		int item_list_w = curr_item_w;
		int item_list_h = chkbox_opth * curr_combobox->option_count;

		int32_t cursor_x, cursor_y;
		get_cursor_pos(cursor_x, cursor_y);

		if (curr_combobox->is_open)
		{
			if (cursor_x >= item_list_x && cursor_x <= (item_list_x + item_list_w))
			{
				//choose one
				if (cursor_y >= item_list_y && cursor_y <= (item_list_y + item_list_h))
				{
					int y_check = cursor_y - item_list_y;
					int hit_item = (int)((float)y_check / (float)chkbox_opth);
					curr_combobox->hover_opt = hit_item;

					if (curr_combobox->hover_opt >= curr_combobox->option_count)
						curr_combobox->hover_opt = curr_combobox->option_count - 1;

				}
				else
				{
					curr_combobox->hover_opt = -1;
				}
			}

			if (wnd->lk_down_once)
			{
				if (curr_combobox->hover_opt == -1)
				{
					curr_combobox->is_open = false;
				}
				else
				{
					if (cursor_x >= item_list_x && cursor_x <= (item_list_x + item_list_w))
					{
						//choose one
						if (cursor_y >= item_list_y && cursor_y <= (item_list_y + item_list_h))
						{
							*curr_combobox->state_var = curr_combobox->hover_opt;
						}
					}
					curr_combobox->is_open = false;
				}
			}
		}
		else
		{
			curr_combobox->hover_opt = -1;
			if (is_mouse_over_ctrl(curr_combobox->ctrl))
			{
				if (wnd->lk_down_once)
				{
					curr_combobox->is_open = true;
				}
			}
		}
	}
	void render_combo_arrow(int x, int y, int w, int h, gui_color color)
	{
		for (int i = 0; i < h; i++) {
			fill_rect(x + i, y + i, w - (i * 2), 1, color);
		}
	}

	void render_combobox(gui_wnd* wnd, gui_combobox* curr_combobox)
	{
		crypt_str(curr_combobox->ctrl->text, wnd->str_xor_key);
		int chkbox_w = curr_combobox->opt_list_width;
		int chkbox_h = curr_combobox->ctrl->heigth;

		int curr_item_x = curr_combobox->ctrl->x + wnd->ctrl->x + wnd->tabs[wnd->active_tab]->label_max_offset;
		int curr_item_y = curr_combobox->ctrl->y + wnd->ctrl->y;
		int curr_item_w = curr_combobox->opt_list_width;
		int curr_item_h = curr_combobox->ctrl->heigth;

		auto chkbox_opth = 18;
		int item_list_x = curr_item_x;
		int item_list_y = curr_item_y + curr_item_h + 2;
		int item_list_w = curr_item_w;
		int item_list_h = chkbox_opth * curr_combobox->option_count;

		bool hlight = curr_combobox->ctrl->mouse_hover || curr_combobox->is_open;

		fill_control(curr_item_x, curr_item_y, curr_item_w, curr_item_h);
		if (hlight)
			draw_rect_outline(curr_item_x, curr_item_y, curr_item_w, curr_item_h, menucol(wnd->colors.active_hovering));

		render_combo_arrow(curr_item_x + 4 + curr_item_w - 25, curr_item_y + 6, 12, 7, hlight ? menucol(wnd->colors.active_hovering) : menucol(wnd->colors.control_border));


		crypt_str(curr_combobox->options[*curr_combobox->state_var], wnd->str_xor_key);
		char fullText[500];
		CGUI_SPRINTF(fullText, 500, "%s", curr_combobox->options[*curr_combobox->state_var]);

		draw_text(curr_item_x + 4, curr_item_y + (curr_item_h / 4) - 4, fullText, menucol(wnd->colors.text_color));
		crypt_str(curr_combobox->options[*curr_combobox->state_var], wnd->str_xor_key);
		if (curr_combobox->is_open) {
			fill_control(item_list_x, item_list_y, item_list_w, item_list_h);
			//draw_rect_outline(item_list_x, item_list_y, item_list_w, item_list_h, wnd->colors.control_border);

			for (int i = 0; i < curr_combobox->option_count; i++) {
				if (curr_combobox->hover_opt >= 0 && curr_combobox->hover_opt == i) {
					fill_control_active(item_list_x + 1, item_list_y + (chkbox_opth * i), chkbox_w - 2, chkbox_opth);

				}
				/*if (i != (curr_combobox->option_count - 1))
				{
					draw_line(item_list_x + 1,
						item_list_y + (chkbox_opth * (i + 1)),
						item_list_x + chkbox_w,
						item_list_y + (chkbox_opth * (i + 1)),
						wnd->colors.active_hovering);
				}*/
				//draw_rect_outline(item_list_x + 1, item_list_y + (chkbox_opth * i), chkbox_w - 1, chkbox_opth, MAKE_ARGB(255, 20, 20, 20));
				crypt_str(curr_combobox->options[i], wnd->str_xor_key);
				draw_text(item_list_x + 2, item_list_y + (chkbox_opth * i) + (chkbox_opth / 6) - 2, curr_combobox->options[i], menucol(wnd->colors.text_color));
				crypt_str(curr_combobox->options[i], wnd->str_xor_key);
			}
		}
		//draw_text(curr_item_x + 6 + curr_item_w + 5, curr_item_y + (curr_item_h / 4) - 4, curr_combobox->ctrl->text,wnd->colors.text_color);
		render_label(wnd, curr_combobox->ctrl, curr_combobox->ctrl->x + wnd->ctrl->x, curr_combobox->ctrl->y + wnd->ctrl->y, curr_combobox->ctrl->text);
		//draw_text(curr_combobox->ctrl->x + wnd->ctrl->x, curr_combobox->ctrl->y + wnd->ctrl->y, curr_combobox->ctrl->text, menucol(wnd->colors.text_color));
		crypt_str(curr_combobox->ctrl->text, curr_combobox->ctrl->wnd->str_xor_key);
	}
	void update_button(gui_wnd* wnd, gui_button* curr_button)
	{
		if (curr_button->ctrl->mouse_hover)
		{
			if (wnd->lk_down_once)
			{
				curr_button->callback();
			}
		}
	}
	void render_spacer(gui_wnd* wnd, gui_spacer* curr_spacer)
	{
		auto ctrl_x = wnd->ctrl->x + curr_spacer->ctrl->x;
		auto ctrl_y = wnd->ctrl->y + curr_spacer->ctrl->y;

		draw_line(
			ctrl_x,
			ctrl_y + curr_spacer->spacer_padding_top, 
			ctrl_x + curr_spacer->ctrl->width, 
			ctrl_y + curr_spacer->spacer_padding_top, 
			colors::Black, 
			1.0f);
	}
	void render_button(gui_wnd* wnd, gui_button* curr_button)
	{
		crypt_str(curr_button->ctrl->text, wnd->str_xor_key);
		auto ctrl_x = wnd->ctrl->x + curr_button->ctrl->x;
		auto ctrl_y = wnd->ctrl->y + curr_button->ctrl->y;

		if(curr_button->ctrl->mouse_hover)
			fill_control_active(ctrl_x, ctrl_y, curr_button->ctrl->width, curr_button->ctrl->heigth);
		else
			fill_control(ctrl_x, ctrl_y, curr_button->ctrl->width, curr_button->ctrl->heigth);


		if(curr_button->ctrl->mouse_hover)
			draw_rect_outline(ctrl_x, ctrl_y, curr_button->ctrl->width, curr_button->ctrl->heigth, menucol( wnd->colors.active_hovering));
		
		
		draw_text_c(ctrl_x + (curr_button->ctrl->width / 2), ctrl_y + (curr_button->ctrl->heigth / 2) , curr_button->ctrl->text, menucol(wnd->colors.text_color));
		crypt_str(curr_button->ctrl->text,
			wnd->str_xor_key);
	}
	void render_tabview(gui_wnd* wnd, gui_tab_view* tabview)
	{
		auto ctrl_x = wnd->ctrl->x + tabview->ctrl->x;
		auto ctrl_y = wnd->ctrl->y + tabview->ctrl->y;
		auto ctrl_w = tabview->ctrl->width;
		auto ctrl_h = tabview->ctrl->heigth;

		draw_rect_outline(ctrl_x, ctrl_y, ctrl_w, ctrl_h, wnd->colors.main_border);

		for (int i = 0; i < ARRAYSIZE(tabview->tabs); i++) {
			auto& curr_tab = tabview->tabs[i];
			if (curr_tab) {
				auto tab_background = curr_tab->mouse_hover ? wnd->colors.control_border : wnd->colors.tab_background;
				if (i == tabview->active_tab)
					tab_background = wnd->colors.tab_active_background;

				int tab_x = ctrl_x + curr_tab->x;
				int tab_y = ctrl_y + curr_tab->y;
				int tab_w = curr_tab->width;
				int tab_h = curr_tab->heigth;

				fill_rect(tab_x, tab_y, tab_w, tab_h, tab_background);
				draw_rect_outline(tab_x, tab_y, tab_w, tab_h, wnd->colors.main_border);

				crypt_str(curr_tab->text, wnd->str_xor_key);
				draw_text_c(tab_x + (tab_w / 2), tab_y + (tab_h / 6), curr_tab->text, MAKE_ARGB(255, 0, 0, 0));
				crypt_str(curr_tab->text, wnd->str_xor_key);
			}
		}

	}
	void render_sellist(gui_wnd* wnd, gui_sellist* list)
	{
		auto ctrl_x = list->ctrl->wnd->ctrl->x + list->ctrl->x;
		auto ctrl_y = list->ctrl->wnd->ctrl->y + list->ctrl->y;

		//crypt_str(list->ctrl->text, list->ctrl->wnd->str_xor_key);

		int ofs_box_h = 0;//20;
		int opt_h = 16;
		//draw_text(ctrl_x, ctrl_y, list->ctrl->text, list->ctrl->wnd->colors.text_color);

		int list_w = list->ctrl->width;
		int list_h = (opt_h * list->optcount);

		fill_control(ctrl_x, ctrl_y + ofs_box_h, list_w, list_h);

		if(list->ctrl->mouse_hover)
			draw_rect_outline(ctrl_x, ctrl_y + ofs_box_h, list_w, list_h, menucol(wnd->colors.active_hovering));

		for (int i = 0; i < list->optcount; i++)
		{
			if (i == (*list->state_var))
				fill_control_active(ctrl_x + 1, ctrl_y + ofs_box_h + (i * opt_h), list_w - 2, opt_h);


			draw_text(ctrl_x + 2, ctrl_y + ofs_box_h + (i * opt_h), list->opt[i], menucol(list->ctrl->wnd->colors.text_color));
		}

		//crypt_str(list->ctrl->text, list->ctrl->wnd->str_xor_key);
	}
	void update_sellist(gui_wnd* wnd, gui_sellist* list)
	{
		if (wnd->lk_down_once)
		{
			int ofs_box_h = 0;//20;
			int opt_h = 16;
			auto ctrl_x = list->ctrl->wnd->ctrl->x + list->ctrl->x;
			auto ctrl_y = list->ctrl->wnd->ctrl->y + list->ctrl->y + ofs_box_h;

			int list_w = list->ctrl->width;
			int list_h = (opt_h * list->optcount);

			list->ctrl->heigth = list_h + ofs_box_h;

			int32_t x, y;
			get_cursor_pos(x, y);

			if (x >= ctrl_x && x <= ctrl_x + list_w &&
				(y >= ctrl_y) && y <= ctrl_y + list_h)
			{
				auto deltaY = (y - ctrl_y) / opt_h;
				if (deltaY != *list->state_var)
				{
					if (list->cb)
						list->cb(*list->state_var, deltaY);
				}
				*list->state_var = deltaY;
			}
		}
	}
	void render_controls(gui_wnd* wnd, gui_tab* active_tab)
	{
		for (unsigned int i = 0; i < CGUI_MAX_TABVIEWS; i++)
		{
			auto curr_tabview = active_tab->tabviews[i];
			if (curr_tabview)
			{
				render_tabview(wnd, curr_tabview);
				auto tabview_active_tab = curr_tabview->tabs[curr_tabview->active_tab];
				if (tabview_active_tab)
				{
					render_controls(wnd, tabview_active_tab);
				}
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_GROUPS; i++)
		{
			auto curr_group = active_tab->groups[i];
			if (curr_group)
			{
				render_group(wnd, curr_group);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_CHECKBOXES; i++)
		{
			auto curr_group = active_tab->checkboxes[i];
			if (curr_group)
			{
				render_checkbox(wnd, curr_group);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_SLIDERS; i++)
		{
			auto curr_slider = active_tab->slider[i];
			if (curr_slider)
			{
				render_slider(wnd, curr_slider);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_SLIDERS; i++)
		{
			auto curr_slider = active_tab->colorslider[i];
			if (curr_slider)
			{
				render_colorslider(wnd, curr_slider);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_HOTKEYS; i++)
		{
			auto curr_hotkey = active_tab->hotkeys[i];
			if (curr_hotkey)
			{
				render_hotkeybox(wnd, curr_hotkey);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_BUTTONS; i++)
		{
			auto curr_button = active_tab->buttons[i];
			if (curr_button)
			{
				render_button(wnd, curr_button);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_LISTCTRLS; i++)
		{
			auto curr_list = active_tab->sellists[i];
			if (curr_list)
			{
				render_sellist(wnd, curr_list);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_SPACER; i++)
		{
			auto curr_list = active_tab->spacer[i];
			if (curr_list)
			{
				render_spacer(wnd, curr_list);
			}
			else break;
		}
		for (unsigned int i = CGUI_MAX_COMBOBOXES; i-- > 0;)
		{
			auto curr_combobox = active_tab->comboboxes[i];
			if (curr_combobox)
			{
				render_combobox(wnd, curr_combobox);
			}
		}

	}

	void update_controls(gui_wnd* wnd, gui_tab* active_tab)
	{
		for (unsigned int i = 0; i < CGUI_MAX_COMBOBOXES; i++)
		{
			auto curr_combobox = active_tab->comboboxes[i];
			if (curr_combobox)
			{
				bool was_open = curr_combobox->is_open;

				base_update(curr_combobox->ctrl);
				update_combobox(wnd, curr_combobox);

				if (curr_combobox->is_open)
				{
					if (!was_open)
					{
						for (unsigned int y = 0; y < CGUI_MAX_COMBOBOXES; y++)
						{
							auto combo = active_tab->comboboxes[y];
							if (combo && curr_combobox != combo)
							{
								combo->is_open = false;
							}
						}
					}
					return;
				}
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_TABVIEWS; i++)
		{
			auto curr_tabview = active_tab->tabviews[i];
			if (curr_tabview)
			{
				base_update(curr_tabview->ctrl);
				update_tabview(wnd, curr_tabview);
				update_controls(wnd, curr_tabview->tabs[curr_tabview->active_tab]);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_GROUPS; i++)
		{
			auto curr_group = active_tab->groups[i];
			if (curr_group)
			{
				base_update(curr_group->ctrl);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_CHECKBOXES; i++)
		{
			auto curr_group = active_tab->checkboxes[i];
			if (curr_group)
			{
				base_update(curr_group->ctrl);
				update_checkbox(wnd, curr_group);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_SLIDERS; i++)
		{
			auto curr_slider = active_tab->slider[i];
			if (curr_slider)
			{
				base_update(curr_slider->ctrl);
				update_slider(wnd, curr_slider);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_SLIDERS; i++)
		{
			auto curr_slider = active_tab->colorslider[i];
			if (curr_slider)
			{
				base_update(curr_slider->ctrl);
				update_colorslider(wnd, curr_slider);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_HOTKEYS; i++)
		{
			auto curr_hotkey = active_tab->hotkeys[i];
			if (curr_hotkey)
			{
				base_update(curr_hotkey->ctrl);
				update_hotkeybox(wnd, curr_hotkey);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_LISTCTRLS; i++)
		{
			auto curr_list = active_tab->sellists[i];
			if (curr_list)
			{
				base_update(curr_list->ctrl);
				update_sellist(wnd, curr_list);
			}
			else break;
		}
		for (unsigned int i = 0; i < CGUI_MAX_BUTTONS; i++)
		{
			auto curr_button = active_tab->buttons[i];
			if (curr_button)
			{
				base_update(curr_button->ctrl);
				update_button(wnd, curr_button);
			}
			else break;
		}



	}
	void update(gui_wnd* wnd)
	{
		update_tabs(wnd);
		update_title(wnd);
		update_controls(wnd, wnd->tabs[wnd->active_tab]);
	}
	void render_wnd(gui_wnd* wnd)
	{
		//render base menu layout
		fill_rect(wnd->ctrl->x + 1, wnd->ctrl->y, wnd->ctrl->width - 2, wnd->tabs[wnd->active_tab]->wndheigth + 10, menucol(wnd->colors.main_background));
		//draw_rect_outline(wnd->ctrl->x, wnd->ctrl->y - CGUI_TAB_HEIGTH - CGUI_TITLEBAR_HEIGTH, wnd->ctrl->width, wnd->ctrl->heigth + CGUI_TAB_HEIGTH + CGUI_TITLEBAR_HEIGTH, MAKE_ARGB(255, 220, 20, 60));
	}
	void render_gui(gui_wnd* wnd)
	{

		if (is_key_down_once(VK_HOME))
		{
			if (!vis_change_in_progress)
			{
				vis_change_in_progress = true;
				vis_step = 0;
				vis_fade_in = !wnd->is_visible;
				auto now = GetTickCount();
				vis_begin = now;

				if (vis_fade_in)
					wnd->is_visible = true;
			}

		}
		if (vis_change_in_progress)
		{
			auto now = GetTickCount();
			auto delta = now - vis_begin;

			//how much pct of 1000 
			auto pct = (int)((delta / 300.0f) * 100.0f);
			vis_step = pct;


			if (vis_step >= 100)
			{
				if (!vis_fade_in)
					wnd->is_visible = !wnd->is_visible;
				vis_change_in_progress = false;
			}
		}

		if (!wnd->is_visible)
			return;

		wnd->lk_down_once = cgui::is_key_down_once(VK_LBUTTON);

		update(wnd);



		render_title(wnd);
		render_wnd(wnd);
		render_tabs(wnd);

		render_controls(wnd, wnd->tabs[wnd->active_tab]);


		//int32_t cursor_x;
		//int32_t cursor_y;
		//get_cursor_pos(cursor_x, cursor_y);


		//fill_rect(cursor_x - 2, cursor_y - 2, 4, 4, wnd->colors.title_background);
		//draw_rect_outline(cursor_x - 2, cursor_y - 2, 4, 4, MAKE_ARGB(255, 0, 0, 0));
	}

}
