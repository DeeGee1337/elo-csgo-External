#pragma once

#include "CGuiBase.hpp"
#include "CGuiDraw.hpp"
#include "CGuiInput.hpp"
#include "../str_encrypt.hpp"
#include <cstdint>


namespace cgui
{
	//setup
	gui_wnd* window(gui_wnd* wnd, cgui_str title, int32_t x, int32_t y, int32_t w, int32_t h);
	gui_checkbox* checkbox(gui_tab* tab, cgui_str text, int32_t x, int32_t y, int32_t* state_var);
	gui_tab* tab(gui_wnd* wnd, gui_tab* tab, cgui_str text, ID3D11ShaderResourceView** icon = nullptr);
	gui_group* group(gui_tab* tab, cgui_str text, int32_t x, int32_t y, int32_t w, int32_t h);
	gui_slider* slider(gui_tab* tab, cgui_str text, int32_t x, int32_t y, int32_t begin, int32_t end, cgui_str unit, int32_t* state_var);
	gui_hotkey* hotkey(gui_tab* tab, cgui_str text, int32_t x, int32_t y, int32_t* state_var);
	gui_combobox* combobox(gui_tab* tab, cgui_str text, int32_t x, int32_t y, const cgui_str* options, uint32_t option_count, int32_t* state_var, uint32_t min_width = 100);
	gui_colorslider* colorslider(gui_tab* tab, cgui_str text, int32_t x, int32_t y, uint32_t* state_var);
	gui_tab_view* tab_view(gui_tab_view* newtabview, gui_tab* tab, cgui_str text, int32_t x, int32_t y, uint32_t w, uint32_t h);
	gui_tab* tab(gui_tab_view* tabview, gui_tab* tab, cgui_str text, ID3D11ShaderResourceView** icon = nullptr);
	gui_button* button(gui_tab* tab, cgui_str text, int32_t x, int32_t y, button_callback callback);
	gui_sellist* sellist(gui_tab* tab, cgui_str text, int32_t x, int32_t y, int32_t* state_var, uint32_t maxopt, selchange_callback cb = nullptr);
	void spacer(gui_tab* tab, int32_t x, int32_t y, int32_t spacer_top = 10, int32_t spacer_bot = 10);

	int					calc_width_pct(gui_wnd* wnd, float percent);
	int					calc_height_pct(gui_wnd* wnd, float percent);

	//auto
	void				reset_y(gui_tab* tab);
	void				set_x(gui_tab* tab, int32_t w);
	void				sync_x_to_group(gui_tab* tab, gui_group* grp);
	gui_checkbox* auto_checkbox(gui_tab* tab, cgui_str text, int32_t* state_var);
	gui_group* auto_group(gui_tab* tab, cgui_str text, int32_t x, int32_t w, int32_t h);
	gui_slider* auto_slider(gui_tab* tab, cgui_str text, int32_t begin, int32_t end, cgui_str unit, int32_t* state_var);
	gui_hotkey* auto_hotkey(gui_tab* tab, cgui_str text, int32_t* state_var);
	gui_combobox* auto_combobox(gui_tab* tab, cgui_str text, const cgui_str* options, uint32_t option_count, int32_t* state_var, uint32_t min_width = 100);
	gui_colorslider* auto_colorslider(gui_tab* tab, cgui_str text, uint32_t* state_var);
	gui_button* auto_button(gui_tab* tab, cgui_str text, button_callback callback);
	gui_sellist* auto_sellist(gui_tab* tab, cgui_str text, int32_t* state_var, uint32_t maxopt, selchange_callback cb = nullptr);
	void		auto_spacer(gui_tab* tab, int32_t spacer_top = 10, int32_t spacer_bot = 10);
	void				finish_init(gui_wnd* wnd);
	//draw gui
	void update(gui_wnd* wnd);
	void render_gui(gui_wnd* wnd);

	void incr_tab_h(gui_tab* tab, int ofs);
}