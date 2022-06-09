#ifndef CHESS_H
#define CHESS_H

#define NB_EDGE_SQUARES 8
#define CHESS_PIECES_SPRITE_PATH "images/1024px-Chess_Pieces_Sprite.png"
#define PIECE_SPRITE_SIZE 170

typedef struct App_items {
    SDL_Window **window;
    SDL_Renderer **renderer;
    SDL_Surface **chess_pieces_surface;
    SDL_Texture **chess_pieces_texture;
    SDL_Rect *darea;
} AppItems;

extern SDL_Color WHITE_COLOR;
extern SDL_Color BLACK_COLOR;
extern SDL_Color OUTLINE_COLOR;
extern SDL_Color SELECTION_COLOR;
extern SDL_Color POSSIBLE_MOVEMENTS_COLOR;
extern SDL_Color PLAYER_1_COLOR;
extern SDL_Color PLAYER_2_COLOR;

SDL_Surface *getImageSurface(char *path);
SDL_Texture* getImageTexture(SDL_Renderer *renderer, SDL_Surface *image_surface);
void setRenderDrawColor(SDL_Renderer *renderer, SDL_Color color);
void drawChessBoard(AppItems *app_items);
void renderChessPieces(AppItems *app_items, uint8_t game_state[NB_EDGE_SQUARES][NB_EDGE_SQUARES]);
void renderPiece(AppItems *app_items, uint8_t piece, int x, int y);
void removeWindowIndex(AppItems *app_items, int *x, int *y);
int withinBoardSurface(int x, int y, int board_size);
int boardToArrayIndex(int coordinate, int board_size);

#endif