#pragma once
#include <cstdint>
#include "../hash.hpp"
#include <string.h>


//#define CGUI_USE_WCHAR
#define CGUI_USE_CHAR

#define CGUI_MAX_KEY_STATES		255
#define CGUI_MAX_CHECKBOXES		300
#define CGUI_MAX_TABS			10
#define CGUI_MAX_GROUPS			100
#define CGUI_MAX_SLIDERS		100
#define CGUI_MAX_SPACER			100
#define CGUI_MAX_HOTKEYS		100
#define CGUI_MAX_COMBOBOXES		300
#define CGUI_MAX_BUTTONS		10
#define CGUI_MAX_TABVIEWS		20
#define CGUI_MAX_TABVIEW_TABS	20
#define CGUI_MAX_LISTCTRLS		20

#define CGUI_MAX_SUBCTRLS	(CGUI_MAX_TABS + CGUI_MAX_GROUPS + CGUI_MAX_SLIDERS + CGUI_MAX_HOTKEYS + CGUI_MAX_TABVIEW_TABS + CGUI_MAX_TABVIEWS + \
							 CGUI_MAX_COMBOBOXES + CGUI_MAX_CHECKBOXES + CGUI_MAX_BUTTONS + CGUI_MAX_LISTCTRLS + CGUI_MAX_SPACER) + 30

#define CGUI_MAX_COMBOBOX_OPTS 30


#define CGUI_TAB_HEIGTH			30
#define CGUI_TAB_WIDTH			80

#define CGUI_TABVIEW_TAB_HEIGTH	20
#define CGUI_TABVIEW_TAB_WIDTH	80

#define CGUI_TITLEBAR_HEIGTH	30


#define CGUI_PADDING_GROUPS		10
#define CGUI_CLIENTAREA_PADDING	10

#ifdef CGUI_USE_CHAR

using gui_chr = char;
using gui_str = char*;
using cgui_str = const char*;

#define CGUI_STRLEN(str) (strlen(str))
#define CGUI_STRCPY(dest, src) (strcpy(dest, src))
#define CGUI_STR(str) str
#define CGUI_SPRINTF(dest, count, fmt, ...) sprintf_s(dest, count, fmt, __VA_ARGS__)
#endif
#ifdef CGUI_USE_WCHAR 

using gui_chr = wchar_t;
using gui_str = wchar_t*;
using cgui_str = const wchar_t*;

#define CGUI_STRLEN(str) (wcslen(str))
#define CGUI_STRCPY(dest, src) (wcscpy(dest, src))
#define CGUI_WIDE(str) L ## str
#define CGUI_STR(str) CGUI_WIDE(str)
#define CGUI_SPRINTF(dest, count, fmt, ...) runtime->stringCbPrintfW(dest, count, fmt, __VA_ARGS__)
#endif
namespace cgui
{
	using gui_color = uint32_t;
	struct gui_checkbox;
	struct gui_key_state;
	struct gui_ctrl;
	struct gui_tab;
	struct gui_wnd;
	struct gui_slider;
	struct gui_combobox;
	struct gui_hotkey;
	struct gui_colorslider;
	struct gui_button;
	struct gui_tab_view;
	struct gui_spacer;

	using button_callback = void(*)();
	using selchange_callback = void(*)(int old_index, int new_index);

	struct gui_key_state
	{
		bool last_state;
		bool is_down;
		bool once;
		unsigned long last_up;
		unsigned long last_down;
	};
	enum class ctrl_type
	{
		WINDOW,
		CHECKBOX,
		SLIDER,
		COLOR_SLIDER,
		COMBO_BOX,
		GROUP,
		HOTKEY_BOX,
		BUTTON,
		TABVIEW,
		LISTCTRL,
		SPACER
	};
	struct gui_ctrl
	{
		gui_chr text[100];
		int32_t x;
		int32_t y;
		int32_t width;
		int32_t heigth;
		ctrl_type type;
		gui_wnd* wnd;
		bool mouse_hover;
	};
	struct gui_spacer
	{
		gui_ctrl* ctrl;

		int spacer_padding_top;
		int spacer_padding_bot;
	};
	struct gui_group
	{
		gui_ctrl* ctrl;
		int group_height;
	};
	struct gui_hotkey
	{
		gui_ctrl* ctrl;
		int32_t* state_var;
		bool is_selecting;
	};
	struct gui_button
	{
		gui_ctrl* ctrl;
		button_callback callback;
	};

	struct gui_sellist
	{
		gui_ctrl* ctrl;
		int32_t* state_var;
		char		opt[100][30];
		int			optcount;
		selchange_callback cb;
	};
	struct gui_tab
	{
		//ctrls
		gui_chr text[100];
		int32_t x;
		int32_t y;
		int32_t width;
		int32_t heigth;

		int32_t wndheigth;
		hash64_t iconHash;
		void* icon;
		int label_max_offset;
		gui_group* grp;

		gui_checkbox* checkboxes[CGUI_MAX_CHECKBOXES];
		gui_group* groups[CGUI_MAX_GROUPS];
		gui_colorslider* colorslider[CGUI_MAX_SLIDERS];
		gui_slider* slider[CGUI_MAX_SLIDERS];
		gui_hotkey* hotkeys[CGUI_MAX_HOTKEYS];
		gui_combobox* comboboxes[CGUI_MAX_COMBOBOXES];
		gui_button* buttons[CGUI_MAX_BUTTONS];
		gui_tab_view* tabviews[CGUI_MAX_TABVIEWS];
		gui_spacer*	spacer[CGUI_MAX_SPACER];

		gui_sellist* sellists[CGUI_MAX_LISTCTRLS];

		uint32_t y_offset;
		uint32_t x_offset;
		gui_wnd* wnd;
		bool mouse_hover;
	};
	struct gui_tab_view
	{
		gui_tab* tabs[CGUI_MAX_TABVIEW_TABS];
		gui_ctrl* ctrl;
		int			tab_cnt;
		int			active_tab;
		int			tab_x_offset;
	};
	struct gui_slider
	{
		gui_ctrl* ctrl;
		int32_t* state_var;
		int32_t begin;
		int32_t end;
		gui_color fill_color;
		float percentage_value;
		bool is_sliding;
		gui_chr unit[10];
	};
	struct gui_colorslider
	{
		gui_ctrl* ctrl;
		uint32_t* state_var;
		bool		is_open;
	};
	struct gui_checkbox
	{
		gui_ctrl* ctrl;
		int32_t* state_var;
	};
	struct gui_combobox
	{
		gui_ctrl* ctrl;
		bool	  is_open;

		gui_chr options[CGUI_MAX_COMBOBOX_OPTS][100];
		int32_t  option_count;

		uint32_t opt_list_width;

		int32_t* state_var;
		int32_t hover_opt;
	};

	struct gui_colors
	{
		gui_color main_background;
		gui_color main_border;

		gui_color tab_background;
		gui_color tab_border;
		gui_color control_border;
		gui_color tab_active_background;

		gui_color active_hovering;
		gui_color title_border;

		gui_color text_color;
		gui_color dark_background;
		gui_color content_color;
	};

	struct gui_wnd
	{
		gui_ctrl* ctrl;
		char str_xor_key;
		bool is_visible;

		int32_t tab_x_offset;
		gui_tab* tabs[CGUI_MAX_TABS];
		int tab_cnt;
		uint32_t  active_tab;
		gui_colors colors;
		bool is_moving;
		int move_delta_x;
		int move_delta_y;
		int move_cur_x;
		int move_cur_y;
		bool lk_down_once;
	};



	gui_button* new_gui_button(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, button_callback callback);
	gui_ctrl* new_gui_ctrl(gui_wnd* wnd, cgui_str text, ctrl_type type, int32_t x, int32_t y, int32_t w, int32_t h);
	gui_tab* new_gui_tab(gui_wnd* wnd, cgui_str text);
	gui_tab_view* init_tabview(gui_tab_view* tabview, gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t w, int32_t h);
	gui_checkbox* new_gui_checkbox(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t w, int32_t h, int32_t* state_var);
	gui_group* new_gui_group(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t w, int32_t h);
	gui_slider* new_gui_slider(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t begin, int32_t end, cgui_str unit, int32_t* state_var);
	gui_colorslider* new_gui_colorslider(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t* state_var);
	gui_hotkey* new_gui_hotkey(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, int32_t* state_var);
	gui_combobox* new_gui_combobox(gui_wnd* wnd, cgui_str text, int32_t x, int32_t y, const cgui_str* options, uint32_t option_count, int32_t* state_var);
	gui_sellist* new_gui_sellist(gui_wnd* wnd, cgui_str text, int x, int y, int32_t* state_var);

	gui_spacer* new_spacer(gui_wnd* wnd, int32_t x, int32_t y);
}