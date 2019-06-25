#include <stdio.h>
#include <cairo.h>
#include <pango/pangocairo.h>
#include <stdint.h>
#define NK_BUTTON_TRIGGER_ON_RELEASE 
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#include "nuklear.h"

static PangoFontMap* font_map;
static PangoLayout* layout;
static PangoContext* context;

void silo_launch(cairo_t* ct, nk_context* nk)
{
	const struct nk_command *cmd = 0;
	nk_foreach(cmd, nk) {
    	switch (cmd->type) {
	default: printf("Unhandled cmd: %i\nexiting...", cmd->type); exit(1);
	case NK_COMMAND_NOP: break;
	case NK_COMMAND_CURVE:
		{
			nk_command_curve* cl=(nk_command_curve*) cmd;
			cairo_set_source_rgb(ct, cl->color.r/255.0f, cl->color.g/255.0f, cl->color.b/255.0f); 
			cairo_set_line_width(ct, cl->line_thickness);
			cairo_move_to(ct, cl->begin.x, cl->begin.y);
			cairo_curve_to(ct, cl->ctrl[0].x, cl->ctrl[0].y, cl->ctrl[1].x, cl->ctrl[1].y, cl->end.x, cl->end.y);
			cairo_stroke(ct);
		}break;
	case NK_COMMAND_RECT_MULTI_COLOR:
		{
			nk_command_rect_multi_color* cl=(nk_command_rect_multi_color*) cmd;
			
			cairo_pattern_t *pattern = cairo_pattern_create_mesh();
			cairo_mesh_pattern_begin_patch (pattern);
			cairo_mesh_pattern_move_to(pattern, cl->x, cl->y+cl->h);
			cairo_mesh_pattern_line_to(pattern, cl->x, cl->y);
			cairo_mesh_pattern_line_to(pattern, cl->x+cl->w, cl->y);
			cairo_mesh_pattern_line_to(pattern, cl->x+cl->w, cl->y+cl->h);
			//cairo_mesh_pattern_line_to(pattern, cl->x, cl->y+cl->h);

			cairo_mesh_pattern_set_corner_color_rgba(pattern, 3, 
				cl->right.r/255.0f, cl->right.g/255.0f, cl->right.b/255.0f, cl->right.a/255.0f);
			cairo_mesh_pattern_set_corner_color_rgba(pattern, 2, 
				cl->top.r/255.0f, cl->top.g/255.0f, cl->top.b/255.0f, cl->top.a/255.0f);
			cairo_mesh_pattern_set_corner_color_rgba(pattern, 1, 
				cl->left.r/255.0f, cl->left.g/255.0f, cl->left.b/255.0f, cl->left.a/255.0f);
			cairo_mesh_pattern_set_corner_color_rgba(pattern, 0, 
				cl->bottom.r/255.0f, cl->bottom.g/255.0f, cl->bottom.b/255.0f, cl->bottom.a/255.0f);
			cairo_mesh_pattern_end_patch(pattern);

			cairo_set_source(ct, pattern);
			cairo_rectangle(ct, cl->x, cl->y, cl->w, cl->h);
			cairo_fill(ct);
			cairo_pattern_destroy(pattern);
		}break;
	case NK_COMMAND_ARC_FILLED:
		{
			nk_command_arc_filled* cl=(nk_command_arc_filled*) cmd;
			cairo_set_source_rgb(ct, cl->color.r/255.0f, cl->color.g/255.0f, cl->color.b/255.0f); 
			cairo_move_to(ct, cl->cx, cl->cy);
			cairo_arc(ct, cl->cx, cl->cy, cl->r, cl->a[0], cl->a[1]);
			cairo_fill(ct);			
		}break;
	case NK_COMMAND_ARC:
		{
			nk_command_arc* cl =(nk_command_arc*) cmd;
			cairo_set_source_rgb(ct, cl->color.r/255.0f, cl->color.g/255.0f, cl->color.b/255.0f); 
			cairo_set_line_width(ct, cl->line_thickness);
			cairo_move_to(ct, cl->cx, cl->cy);
			cairo_arc(ct, cl->cx, cl->cy, cl->r, cl->a[0], cl->a[1]);
			cairo_stroke(ct);
		}break;
	case NK_COMMAND_CIRCLE:
		{
			nk_command_circle* cl =(nk_command_circle*) cmd;
			cairo_set_source_rgb(ct, cl->color.r/255.0f, cl->color.g/255.0f, cl->color.b/255.0f); 
			cairo_set_line_width(ct, cl->line_thickness);
			cairo_move_to(ct, cl->x + cl->w/2, cl->y + cl->w/2);
			cairo_arc(ct, cl->x + cl->w/2, cl->y + cl->w/2, cl->w/2, 0, 6.28f);
			cairo_stroke(ct);
		}break;
	case NK_COMMAND_CIRCLE_FILLED:
		{
			nk_command_circle_filled* cl =(nk_command_circle_filled*) cmd;
			cairo_set_source_rgb(ct, cl->color.r/255.0f, cl->color.g/255.0f, cl->color.b/255.0f); 
			cairo_move_to(ct, cl->x + cl->w/2, cl->y + cl->w/2);
			cairo_arc(ct, cl->x + cl->w/2, cl->y + cl->w/2, cl->w/2, 0, 6.28f);
			cairo_fill(ct);
		}break;
	case NK_COMMAND_TEXT:
		{
			nk_command_text* txt =(nk_command_text*) cmd;
			cairo_set_source_rgb(ct, txt->background.r/255.0f, txt->background.g/255.0f, txt->background.b/255.0f); 
			cairo_rectangle(ct, txt->x, txt->y, txt->w, txt->h);
			cairo_fill(ct);
			pango_layout_set_text(layout, (char*)&txt->string, txt->length);
			cairo_set_source_rgb(ct, txt->foreground.r/255.0f, txt->foreground.g/255.0f, txt->foreground.b/255.0f); 
			cairo_move_to(ct, txt->x, txt->y);
			pango_cairo_show_layout(ct, layout);
		}break;
	case NK_COMMAND_SCISSOR:
		{
			nk_command_scissor* sc =(nk_command_scissor*) cmd;
			cairo_reset_clip(ct);
			cairo_rectangle(ct, sc->x, sc->y, sc->w, sc->h);
			cairo_clip(ct);
		}break;
   	case NK_COMMAND_LINE:
        	{
			nk_command_line* cl =(nk_command_line*) cmd;
			cairo_set_source_rgb(ct, cl->color.r/255.0f, cl->color.g/255.0f, cl->color.b/255.0f); 
			cairo_set_line_width(ct, cl->line_thickness);
			cairo_move_to(ct, cl->begin.x, cl->begin.y);
			cairo_line_to(ct, cl->end.x, cl->end.y);
			cairo_stroke(ct);
        	}break;
	case NK_COMMAND_RECT:
	        {
			nk_command_rect* cr =(nk_command_rect*) cmd;
			cairo_set_line_width(ct, cr->line_thickness);
			cairo_set_source_rgb(ct, cr->color.r/255.0f, cr->color.g/255.0f, cr->color.b/255.0f);
			cairo_rectangle(ct, cr->x, cr->y, cr->w, cr->h);
			cairo_stroke(ct);
		}break;
	case NK_COMMAND_RECT_FILLED:
		{
			nk_command_rect_filled* cr =(nk_command_rect_filled*) cmd;
			cairo_set_source_rgb(ct, cr->color.r/255.0f, cr->color.g/255.0f, cr->color.b/255.0f);
			cairo_rectangle(ct, cr->x, cr->y, cr->w, cr->h);
			cairo_fill(ct);
		}break;
	case NK_COMMAND_TRIANGLE:
		{
			nk_command_triangle* cr =(nk_command_triangle*) cmd;
			cairo_set_source_rgb(ct, cr->color.r/255.0f, cr->color.g/255.0f, cr->color.b/255.0f);
			cairo_set_line_width(ct, cr->line_thickness);
			cairo_move_to(ct, cr->a.x, cr->a.y);
			cairo_line_to(ct, cr->b.x, cr->b.y);
			cairo_line_to(ct, cr->c.x, cr->c.y);
			cairo_line_to(ct, cr->a.x, cr->a.y);
			cairo_stroke(ct);
		}break;
	case NK_COMMAND_TRIANGLE_FILLED:
		{
			nk_command_triangle_filled* cr =(nk_command_triangle_filled*) cmd;
			cairo_set_source_rgb(ct, cr->color.r/255.0f, cr->color.g/255.0f, cr->color.b/255.0f);
			cairo_move_to(ct, cr->a.x, cr->a.y);
			cairo_line_to(ct, cr->b.x, cr->b.y);
			cairo_line_to(ct, cr->c.x, cr->c.y);
			cairo_line_to(ct, cr->a.x, cr->a.y);
			cairo_fill(ct);
		}break;
	case NK_COMMAND_POLYGON_FILLED:
		{
			nk_command_polygon_filled* cr =(nk_command_polygon_filled*) cmd;
			cairo_set_source_rgb(ct, cr->color.r/255.0f, cr->color.g/255.0f, cr->color.b/255.0f);
			struct nk_vec2i* pts =(struct nk_vec2i*) &cr->points[0];
			cairo_move_to(ct, pts[0].x, pts[0].y);
			for(int ptindex = 1; ptindex < cr->point_count; ++ptindex) cairo_line_to(ct, pts[ptindex].x, pts[ptindex].y);
			cairo_line_to(ct, pts[0].x, pts[0].y);
			cairo_fill(ct);
		}break;
	case NK_COMMAND_POLYGON:
		{
			nk_command_polygon* cr=(nk_command_polygon*) cmd;
			cairo_set_source_rgb(ct, cr->color.r/255.0f, cr->color.g/255.0f, cr->color.b/255.0f);
			cairo_set_line_width(ct, cr->line_thickness);
			struct nk_vec2i* pts =(struct nk_vec2i*) &cr->points[0];
			cairo_move_to(ct, pts[0].x, pts[0].y);
			for(int ptindex = 1; ptindex < cr->point_count; ++ptindex) cairo_line_to(ct, pts[ptindex].x, pts[ptindex].y);
			cairo_line_to(ct, pts[0].x, pts[0].y);
			cairo_stroke(ct);
		}break;
	case NK_COMMAND_POLYLINE:
		{
			nk_command_polyline* cr =(nk_command_polyline*) cmd;
			cairo_set_source_rgb(ct, cr->color.r/255.0f, cr->color.g/255.0f, cr->color.b/255.0f);
			cairo_set_line_width(ct, cr->line_thickness);
			struct nk_vec2i* pts =(struct nk_vec2i*) &cr->points[0];
			cairo_move_to(ct, pts[0].x, pts[0].y);
			for(int ptindex = 1; ptindex < cr->point_count; ++ptindex) cairo_line_to(ct, pts[ptindex].x, pts[ptindex].y);
			//cairo_line_to(ct, pts[0].x, pts[0].y); // looks like polyline doesn't connect, otherwise would be same
								 // as stroked polygon
			cairo_stroke(ct);
		}break;
	} //end of cmd switch
	} //end of nk_foreach

	return;
}

float silo_get_text_width(nk_handle handle, float height, const char *text, int len)
{
	int w, h;
	pango_layout_set_attributes(layout, nullptr);
	pango_layout_set_text(layout, text, len);
	pango_layout_get_pixel_size(layout, &w, &h);
	return w;
}

void silo_set_font(const char* fdsc)
{
	PangoFontDescription* font_desc = pango_font_description_from_string(fdsc);
	pango_layout_set_font_description(layout, nullptr);   // pango will silently not make a copy if 
							      // the new fontdesc matches the old one
							      // whether by pointer or deep compare.
							      // this makes sure the old one is deleted
							      // in favor of a new copy
	pango_layout_set_font_description(layout, font_desc);
	pango_font_map_load_font(font_map, context, font_desc);  // most examples do this, but does not appear
							      // to actually be necessary? There's not much
							      // info out there getting detailed on Pango.

	pango_font_description_free(font_desc); 	      // no longer needed, pango makes a copy
	return;
}

int silo_get_font_height()
{
	int w, h;
	pango_layout_set_attributes(layout, nullptr);
	pango_layout_set_text(layout, "Hello", 5);
	pango_layout_get_pixel_size(layout, &w, &h);
	return h;
}

void silo_init()
{
	font_map = pango_cairo_font_map_get_default(); // pango-owned, don't delete
	context = pango_font_map_create_context(font_map);
	layout = pango_layout_new(context);

	silo_set_font("sans 15");
	return;
}

