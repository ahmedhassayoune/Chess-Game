#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "chess.h"
#include "game_logic.h"

void freeVar(void* ptr);

int main(int argc, char **argv)
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL; 
	SDL_Surface *chess_pieces_surface = NULL;
	SDL_Texture *chess_pieces_texture = NULL;
	SDL_Rect darea;

	AppItems *app_items = malloc(sizeof(AppItems));

	app_items->window = &window;
	app_items->renderer = &renderer;
	app_items->chess_pieces_surface = &chess_pieces_surface;
	app_items->chess_pieces_texture = &chess_pieces_texture;
	app_items->darea = &darea;

	// SDL init
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		errx(EXIT_FAILURE, "Failed to initialize SDL -> %s.\n", SDL_GetError());
	
	// Chess window creation
	window = SDL_CreateWindow("Chess game", 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			600, 600,
			SDL_WINDOW_RESIZABLE);
	if(window == NULL)
		errx(EXIT_FAILURE, "Failed to create SDL window -> %s.\n", SDL_GetError());

	// Renderer creation
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
												SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		SDL_DestroyWindow(window);
		errx(EXIT_FAILURE, "Failed to create SDL renderer -> %s.\n", SDL_GetError());
	}

	chess_pieces_surface = getImageSurface(CHESS_PIECES_SPRITE_PATH);
	chess_pieces_texture = getImageTexture(renderer, chess_pieces_surface);
	SDL_RenderGetViewport(renderer, &darea);

	renderGameState(app_items);

	SDL_Event e;
	int quit = 0;
	while(!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch(e.type)
			{
				case SDL_WINDOWEVENT:
				{
					if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						SDL_RenderGetViewport(renderer, &darea);
						renderGameState(app_items);
					}
					break;
				}
				case SDL_MOUSEBUTTONDOWN:
				{
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						displayAction(app_items, e.button.x, e.button.y);
					}
					break;
				}
				case SDL_QUIT:
					quit = 1;
					break;

				default: {}
			}
		}
	}

	freeVar(app_items);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}

void freeVar(void* ptr)
{
	free(ptr);
	ptr = NULL;
}