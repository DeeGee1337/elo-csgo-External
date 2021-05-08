#pragma once

#include "CGuiBase.hpp"
#include <D3D11.h>

namespace cgui
{
	void get_text_size(cgui_str text, int32_t& w, int32_t& h);
	void draw_rect_outline(int32_t x, int32_t y, int32_t w, int32_t h, gui_color color);
	void fill_rect(int32_t x, int32_t y, int32_t w, int32_t h, gui_color color);
	void draw_text(int32_t x, int32_t y, cgui_str text, gui_color color, bool shadow = false);
	void draw_text_r(int32_t x, int32_t y, cgui_str text, gui_color color, bool shadow = false);
	void draw_text_c(int32_t x, int32_t y, cgui_str text, gui_color color, bool shadow = false);
	void draw_line(int32_t x, int32_t y, int32_t x2, int32_t y2, gui_color color, float thickness = 1.0f);
	void fill_text_c(int32_t x, int32_t y, cgui_str text, gui_color color);

	void fill_gradient(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t colLeft, uint32_t colTop, uint32_t colRight, uint32_t colBot);
	void draw_img(int32_t x, int32_t y, int32_t w, int32_t h, ID3D11ShaderResourceView* v, DWORD color = 0);
}
