#include "CGuiBase.hpp"
#include <Windows.h>

namespace cgui
{

	uint32_t		curr_tabs;
	gui_tab			gui_tabs[CGUI_MAX_TABS];

	uint32_t		curr_checkbox;
	gui_checkbox	gui_checkboxes[CGUI_MAX_CHECKBOXES];

	uint32_t		curr_gui_ctrls;
	gui_ctrl		gui_ctrls[CGUI_MAX_SUBCTRLS];

	uint32_t		curr_gui_groups;
	gui_group		gui_groups[CGUI_MAX_GROUPS];

	uint32_t		curr_gui_sliders;
	gui_slider		gui_sliders[CGUI_MAX_SLIDERS];

	uint32_t		curr_gui_colorsliders;
	gui_colorslider gui_colorsliders[CGUI_MAX_SLIDERS];

	uint32_t		curr_gui_hotkeys;
	gui_hotkey		gui_hotkeys[CGUI_MAX_HOTKEYS];

	uint32_t		curr_gui_comboboxes;
	gui_combobox	gui_comboboxes[CGUI_MAX_COMBOBOXES];

	uint32_t		curr_gui_tabviews;
	gui_tab_view	gui_tab_views[CGUI_MAX_TABVIEWS];

	uint32_t		curr_gui_buttons;
	gui_button		gui_butons[CGUI_MAX_BUTTONS];

	uint32_t		curr_gui_spacer;
	gui_spacer		gui_spacers[CGUI_MAX_SPACER];

	uint32_t		curr_gui_sellists;
	gui_sellist		gui_sellists[CGUI_MAX_LISTCTRLS];

	gui_ctrl* new_gui_ctrl(gui_wnd* wnd, cgui_str text, ctrl_type type, int32_t x, int32_t y, int32_t w, int32_t h)
	{
		auto new_control = &gui_ctrls[curr_gui_ctrls++];

		if (text)
			CGUI_STRCPY(new_control->text, text);

		new_control->type = type;
		new_control->x = x;
		new_control->y = y;
		new_control->width = w;
		new_control->heigth = h;
		new_control->wnd = wnd;
		new_control->mouse_hover = false;

		return new_control;
	}
	gui_tab* new_gui_tab(gui_wnd* wnd, cgui_str text)
	{
		auto new_tab = &gui_tabs[curr_tabs++];


		CGUI_STRCPY(new_tab->text, text);
		new_tab->wnd = wnd;

		for (unsigned int i = 0; i < CGUI_MAX_CHECKBOXES; i++)
		{
			new_tab->checkboxes[i] = NULL;
		}

		return new_tab;
	}
	gui_checkbox* new_gui_checkbox(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t w, int32_t h, int32_t* state_var)
	{
		auto new_checkbox = &gui_checkboxes[curr_checkbox++];

		new_checkbox->ctrl = new_gui_ctrl(wnd, text, ctrl_type::CHECKBOX, x, y, w, h);
		new_checkbox->state_var = state_var;

		return new_checkbox;
	}

	gui_group* new_gui_group(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t w, int32_t h)
	{
		auto new_group = &gui_groups[curr_gui_groups++];

		new_group->ctrl = new_gui_ctrl(wnd, text, ctrl_type::GROUP, x, y, w, h);

		return new_group;
	}
	gui_slider* new_gui_slider(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t begin, int32_t end, cgui_str unit, int32_t* state_var)
	{
		auto new_slider = &gui_sliders[curr_gui_sliders++];

		new_slider->ctrl = new_gui_ctrl(wnd, text, ctrl_type::SLIDER, x, y, 0, 0);

		new_slider->begin = begin;
		new_slider->end = end;
		new_slider->state_var = state_var;
		new_slider->fill_color = wnd->colors.content_color;

		CGUI_STRCPY(new_slider->unit, unit);
		return new_slider;
	}

	gui_colorslider* new_gui_colorslider(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t* state_var)
	{
		auto new_slider = &gui_colorsliders[curr_gui_colorsliders++];

		new_slider->ctrl = new_gui_ctrl(wnd, text, ctrl_type::COLOR_SLIDER, x, y, w, h);
		new_slider->state_var = state_var;


		return new_slider;
	}
	gui_hotkey* new_gui_hotkey(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t* state_var)
	{
		auto new_hotkey = &gui_hotkeys[curr_gui_hotkeys++];

		new_hotkey->ctrl = new_gui_ctrl(wnd, text, ctrl_type::HOTKEY_BOX, x, y, 0, 0);
		new_hotkey->state_var = state_var;

		return new_hotkey;
	}
	gui_button* new_gui_button(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, button_callback callback)
	{
		auto new_button = &gui_butons[curr_gui_buttons++];

		new_button->ctrl = new_gui_ctrl(wnd, text, ctrl_type::BUTTON, x, y, 0, 0);
		new_button->callback = callback;

		return new_button;
	}
	gui_spacer* new_spacer(gui_wnd* wnd, int32_t x, int32_t y)
	{
		auto new_spacer = &gui_spacers[curr_gui_spacer++];

		new_spacer->ctrl = new_gui_ctrl(wnd, nullptr, ctrl_type::SPACER, x, y, 0, 0);

		return new_spacer;
	}
	gui_tab_view* init_tabview(gui_tab_view* tabview, gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t w, int32_t h)
	{
		tabview->ctrl = new_gui_ctrl(wnd, text, ctrl_type::TABVIEW, x, y, w, h);
		tabview->tab_cnt = 0;
		tabview->active_tab = 0;

		for (int i = 0; i < ARRAYSIZE(tabview->tabs); i++) {
			tabview->tabs[i] = nullptr;
		}

		return tabview;
	}
	gui_combobox* new_gui_combobox(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, const cgui_str* options, uint32_t option_count, int32_t* state_var)
	{
		auto new_combobox = &gui_comboboxes[curr_gui_comboboxes++];

		new_combobox->ctrl = new_gui_ctrl(wnd, text, ctrl_type::COMBO_BOX, x, y, 0, 0);
		new_combobox->is_open = false;
		new_combobox->state_var = state_var;
		new_combobox->option_count = option_count;
		new_combobox->hover_opt = -1;

		for (uint32_t i = 0; i < option_count; i++)
		{
			if (i >= CGUI_MAX_COMBOBOX_OPTS)
				break;

			CGUI_STRCPY(new_combobox->options[i], options[i]);
		}

		return new_combobox;
	}

	gui_sellist* new_gui_sellist(gui_wnd* wnd, cgui_str text, int x, int y, int32_t* state_var)
	{
		auto new_list = &gui_sellists[curr_gui_comboboxes++];

		new_list->ctrl = new_gui_ctrl(wnd, text, ctrl_type::LISTCTRL, x, y, 0, 0);
		new_list->state_var = state_var;
		return new_list;
	}
}