#include <stdio.h>
#include <cairo.h>
#include <cstring>
#include <cctype>
#include <cstdint>
#include <utility>
#include <vector>
#include <memory>
#include <SDL.h>
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_BUTTON_TRIGGER_ON_RELEASE 
#include "nuklear.h"

bool MainRunning = true;

void silo_launch(cairo_t*, nk_context*);
float silo_get_text_width(nk_handle handle, float height, const char *text, int len);
void silo_init();
int silo_get_font_height();

int main(int argc, char** args)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* MainWindow = SDL_CreateWindow("SRGUI Tests", 0, 0, 1200,720, 0); //SDL_WINDOW_OPENGL
	SDL_Surface* MainWindowSurf = SDL_GetWindowSurface(MainWindow);

	int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, 1200);
	uint8_t* buffer =(uint8_t*) malloc(stride*720);
	
	SDL_Surface* boom = SDL_CreateRGBSurfaceFrom(buffer, 1200, 720, 32, stride, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
	cairo_surface_t* csurf = cairo_image_surface_create_for_data(buffer, CAIRO_FORMAT_ARGB32, 1200, 720, stride);
	cairo_t* ct = cairo_create(csurf);
	cairo_set_source_rgb(ct, 0.5f, 0.5f, 0.5f);
	cairo_paint(ct);

	silo_init();

	nk_context ctx;
	nk_user_font font;
	font.height = silo_get_font_height();
	font.width = &silo_get_text_width;

	nk_init_default(&ctx, (nk_user_font*)&font );

	enum {EASY, HARD};
	static int op = EASY;
	static float value = 0.6f;
	static int i =  20;
	struct nk_colorf color1;

	while( MainRunning )
	{
		SDL_Event event;
		nk_input_begin(&ctx);
		while( SDL_PollEvent(&event) ) 
		{
			switch( event.type )
			{
			case SDL_KEYDOWN:{
				
				}break;
			case SDL_MOUSEMOTION:
				nk_input_motion(&ctx, event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEBUTTONDOWN: // SDL's defines start at 1, but are at least in the same order
				nk_input_button(&ctx, (nk_buttons)(event.button.button-1), event.button.x, event.button.y, 1);
				break;
			case SDL_MOUSEBUTTONUP:
				nk_input_button(&ctx, (nk_buttons)(event.button.button-1), event.button.x, event.button.y, 0);
				break;
			case SDL_WINDOWEVENT:
				if( event.window.event == SDL_WINDOWEVENT_CLOSE )
				{
					MainRunning = false;
				}
				break;
			case SDL_QUIT:
				MainRunning = false;
				break;
			}
		}
		nk_input_end(&ctx);

	if (nk_begin(&ctx, "Show", nk_rect(50, 50, 320, 320),
	    NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE|NK_WINDOW_SCALABLE)) {

	    /* fixed widget pixel width */
	    nk_layout_row_static(&ctx, 30, 80, 1);
	    if (nk_button_label(&ctx, "button")) {
		/* event handling */
			puts("it was clicked");
	    }

	    /* fixed widget window ratio width */
	    nk_layout_row_dynamic(&ctx, 30, 2);
	    if (nk_option_label(&ctx, "easy", op == EASY)) op = EASY;
	    if (nk_option_label(&ctx, "hard", op == HARD)) op = HARD;

	    /* custom widget pixel width */
	    nk_layout_row_begin(&ctx, NK_STATIC, 30, 2);
	    {
		nk_layout_row_push(&ctx, 80);
		nk_label(&ctx, "Volume:", NK_TEXT_LEFT);
		nk_layout_row_push(&ctx, 120);
		nk_slider_float(&ctx, 0, &value, 1.0f, 0.1f);
	    }
	    nk_layout_row_end(&ctx);

			nk_layout_row_dynamic(&ctx, 30, 1);
			if (nk_combo_begin_color(&ctx, nk_rgb_cf(color1), nk_vec2(nk_widget_width(&ctx),400))) {
				nk_layout_row_dynamic(&ctx, 120, 1);
				color1 = nk_color_picker(&ctx, color1, NK_RGBA);
				nk_layout_row_dynamic(&ctx, 25, 1);
				color1.r = nk_propertyf(&ctx, "#R:", 0, color1.r, 1.0f, 0.01f,0.005f);
				color1.g = nk_propertyf(&ctx, "#G:", 0, color1.g, 1.0f, 0.01f,0.005f);
				color1.b = nk_propertyf(&ctx, "#B:", 0, color1.b, 1.0f, 0.01f,0.005f);
				color1.a = nk_propertyf(&ctx, "#A:", 0, color1.a, 1.0f, 0.01f,0.005f);
				nk_combo_end(&ctx);
			}
	}
	nk_end(&ctx);

		cairo_set_source_rgb(ct, 0.5f, 0.5f, 0.5f);
		cairo_paint(ct);
		
		silo_launch(ct, &ctx);
		nk_clear(&ctx);
		SDL_BlitSurface(boom, nullptr, MainWindowSurf, nullptr);

		
		SDL_UpdateWindowSurface(MainWindow);	
	}

	SDL_Quit();
	free(buffer);

	return 0;
}

