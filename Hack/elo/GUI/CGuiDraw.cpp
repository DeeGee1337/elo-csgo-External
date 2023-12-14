#include "CGuiDraw.hpp"
#include "../Colors.hpp"
#include "../csgo.hpp"

namespace cgui
{
	void draw_img(int32_t x, int32_t y, int32_t w, int32_t h, ID3D11ShaderResourceView* v, DWORD color)
	{
	}

	void fill_gradient(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t colLeft, uint32_t colTop, uint32_t colRight, uint32_t colBot)
	{
		g_csgo.surface->rect((float)x, (float)y, (float)w, (float)h, colLeft);
	}
	void get_text_size(cgui_str text, int32_t& w, int32_t& h)
	{
		float fw, fh;
		g_csgo.verdana_font->get_text_size(text, &fw, &fh);

		w = (int32_t)(fw);
		h = (int32_t)(fh);
	}
	void draw_rect_outline(int32_t x, int32_t y, int32_t w, int32_t h, gui_color color)
	{
		g_csgo.surface->border_box((float)x, (float)y, (float)w, (float)h, 1.0f, color);
		//return nuk_draw_rect_outline(x, y, w, h, color);
	}
	void fill_rect(int32_t x, int32_t y, int32_t w, int32_t h, gui_color color)
	{
		g_csgo.surface->rect((float)x, (float)y, (float)w, (float)h, color);
	}
	void draw_text(int32_t x, int32_t y, cgui_str text, gui_color color, bool shadow)
	{
		g_csgo.surface->text((float)x, (float)y, g_csgo.verdana_font, color, text);
		//render.DrawString(x, y, color, text);
	}
	void draw_text_r(int32_t x, int32_t y, cgui_str text, gui_color color, bool shadow)
	{
		int32_t w, h;
		get_text_size(text, w, h);
		return draw_text(x - w, y, text, color, shadow);
		//render.DrawStringR(x, y, color, text);
	}

	void draw_text_c(int32_t x, int32_t y, cgui_str text, gui_color color, bool shadow)
	{
		int32_t w, h;
		get_text_size(text, w, h);
		return draw_text(x - (w / 2), y - (h / 2), text, color, shadow);
	}
	void draw_line(int32_t x, int32_t y, int32_t x2, int32_t y2, gui_color color, float thickness)
	{
		g_csgo.surface->line((float)x, (float)y, (float)x2, (float)y2, color, thickness);
	}
	/*void fill_text_c(int32_t x, int32_t y, cgui_str text, gui_color color)
	{
		int w, h;
		cgui::get_text_size(text, w, h);
		cgui::fill_rect(x - (w / 2), y - 8, w, 15, g_items.espBackgroundColor);

		draw_text_c(x, y, text, color);
	}*/
}
