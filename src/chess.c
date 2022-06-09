#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <math.h>

#include "chess.h"
#include "game_logic.h"

// Board colors
SDL_Color WHITE_COLOR = { .r = 0xFF, .g = 0xFF, .b = 0xFF };
SDL_Color BLACK_COLOR = { .r = 0, .g = 0, .b = 0 };
SDL_Color OUTLINE_COLOR = { .r = 0x78, .g = 0xA7, .b = 0x02 };
SDL_Color SELECTION_COLOR = { .r = 0xF6, .g = 0xF6, .b = 0x69 };
SDL_Color POSSIBLE_MOVEMENTS_COLOR = { .r = 0xFF, .g = 0, .b = 0xFE };
SDL_Color PLAYER_1_COLOR = { .r = 0xEE, .g = 0xEE, .b = 0xD2 };
SDL_Color PLAYER_2_COLOR = { .r = 0x7D, .g = 0x94, .b = 0x5D };

// Rectangles of each P1 piece in the sprite
SDL_Rect P1_KING = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = 0, .y = 0};
SDL_Rect P1_QUEEN = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = PIECE_SPRITE_SIZE, .y = 0};
SDL_Rect P1_BISHOP = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = PIECE_SPRITE_SIZE * 2, .y = 0};
SDL_Rect P1_KNIGHT = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = PIECE_SPRITE_SIZE * 3, .y = 0};
SDL_Rect P1_ROOK = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = PIECE_SPRITE_SIZE * 4, .y = 0};
SDL_Rect P1_PAWN = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = PIECE_SPRITE_SIZE * 5, .y = 0};

// Rectangles of each P2 piece in the sprite
SDL_Rect P2_KING = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = 0, .y = PIECE_SPRITE_SIZE};
SDL_Rect P2_QUEEN = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = PIECE_SPRITE_SIZE, .y = PIECE_SPRITE_SIZE};
SDL_Rect P2_BISHOP = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = PIECE_SPRITE_SIZE * 2, .y = PIECE_SPRITE_SIZE};
SDL_Rect P2_KNIGHT = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = PIECE_SPRITE_SIZE * 3, .y = PIECE_SPRITE_SIZE};
SDL_Rect P2_ROOK = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = PIECE_SPRITE_SIZE * 4, .y = PIECE_SPRITE_SIZE};
SDL_Rect P2_PAWN = {.h = PIECE_SPRITE_SIZE, .w = PIECE_SPRITE_SIZE, .x = PIECE_SPRITE_SIZE * 5, .y = PIECE_SPRITE_SIZE};

SDL_Surface* getImageSurface(char *path)
{
    SDL_Surface *image_surface;

    image_surface = IMG_Load(path);
    if (!image_surface)
        errx(EXIT_FAILURE, "Failed to load %s: %s", path, IMG_GetError());

    return image_surface;
}

SDL_Texture* getImageTexture(SDL_Renderer *renderer, SDL_Surface *image_surface)
{
    SDL_Texture *image_texture;

    image_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
    if (!image_texture)
        errx(EXIT_FAILURE, "Failed to create texture of image surface: %s", SDL_GetError());

    return image_texture;
}

void setRenderDrawColor(SDL_Renderer *renderer, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
}

void drawChessBoard(AppItems *app_items)
{
    SDL_Renderer *renderer = *(app_items->renderer);
    SDL_Rect *darea = app_items->darea;

    // Getting a square for the drawing area
    if (darea->w < darea->h)
    {
        darea->y = darea->h / 2 - darea->w / 2;
        darea->h = darea->w;
    }
    else if (darea->w > darea->h)
    {
        darea->x = darea->w / 2 - darea->h / 2;
        darea->w = darea->h;
    }

    int step = darea->w / NB_EDGE_SQUARES - 1;

    SDL_Rect rect = 
    {
        .w = step,
        .h = step,
        .x = darea->x,
        .y = darea->y
    };
    
    // Color the whole board with the player 1 color
    setRenderDrawColor(renderer, PLAYER_1_COLOR);
    SDL_RenderFillRect(renderer, darea);

    // Overwrite the small squares with the player 2 color
    setRenderDrawColor(renderer, PLAYER_2_COLOR);
    for(int row = 0; row < NB_EDGE_SQUARES; row++)
    {
        for(int col = (row + 1)%2; col < NB_EDGE_SQUARES; col += 2)
        {
            rect.x = darea->x + col * (step + 1);
            SDL_RenderFillRect(renderer, &rect);
        }
        rect.y += step + 1;
    }
}

void renderChessPieces(AppItems *app_items, uint8_t game_state[NB_EDGE_SQUARES][NB_EDGE_SQUARES])
{
    for(int i = 0; i < NB_EDGE_SQUARES; i++)
    {
        for(int j = 0; j < NB_EDGE_SQUARES; j++)
        {
            if(game_state[i][j] != EMPTY)
                renderPiece(app_items, game_state[i][j], j, i);
        }
    }
}

void renderPiece(AppItems *app_items, uint8_t piece, int x, int y)
{
    SDL_Renderer *renderer = *(app_items->renderer);
    SDL_Texture *chess_pieces_texture = *(app_items->chess_pieces_texture);
    SDL_Rect *darea = app_items->darea;

    int square_size = darea->w / NB_EDGE_SQUARES - 1;

    SDL_Rect renderer_piece_rect = 
    {
        .h = square_size,
        .w = square_size,
        .x = darea->x + x * (square_size + 1),
        .y = darea->y + y * (square_size + 1)
    };

    if ((piece & PLAYER1) == PLAYER1)
    {
        switch(piece & 0b00111111)
        {
            case KING:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P1_KING, &renderer_piece_rect);
                break;
            case QUEEN:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P1_QUEEN, &renderer_piece_rect);
                break;
            case BISHOP:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P1_BISHOP, &renderer_piece_rect);
                break;
            case KNIGHT:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P1_KNIGHT, &renderer_piece_rect);
                break;
            case ROOK:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P1_ROOK, &renderer_piece_rect);
                break;
            case PAWN:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P1_PAWN, &renderer_piece_rect);
                break;
            default:
                break;
        }
    }
    else
    {
        switch(piece & 0b00111111)
        {
            case KING:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P2_KING, &renderer_piece_rect);
                break;
            case QUEEN:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P2_QUEEN, &renderer_piece_rect);
                break;
            case BISHOP:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P2_BISHOP, &renderer_piece_rect);
                break;
            case KNIGHT:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P2_KNIGHT, &renderer_piece_rect);
                break;
            case ROOK:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P2_ROOK, &renderer_piece_rect);
                break;
            case PAWN:
                SDL_RenderCopy(renderer, chess_pieces_texture, &P2_PAWN, &renderer_piece_rect);
                break;
            default:
                break;
        }
    }

}

void removeWindowIndex(AppItems *app_items, int *x, int *y)
{
    SDL_Renderer *renderer = *(app_items->renderer);
    
    SDL_Rect renderer_rect;
    SDL_RenderGetViewport(renderer, &renderer_rect);

    if (renderer_rect.w < renderer_rect.h)
    {
        *y -= renderer_rect.h / 2 - renderer_rect.w / 2;
    }
    else if (renderer_rect.w > renderer_rect.h)
    {
        *x -= renderer_rect.w / 2 - renderer_rect.h / 2;
    }
}

int withinBoardSurface(int x, int y, int board_size)
{
    return (x >= 0 && x < board_size) && (y >= 0 && y < board_size);
}

int boardToArrayIndex(int coordinate, int board_size)
{
    return (int)floor((float)NB_EDGE_SQUARES * (float)coordinate / (float)board_size);
}