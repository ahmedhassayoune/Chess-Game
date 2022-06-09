#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#define PLAYER1 0b10000000
#define PLAYER2 0b01000000

#define KING    0b00100000
#define QUEEN   0b00010000
#define BISHOP  0b00001000
#define KNIGHT  0b00000100
#define ROOK    0b00000010
#define PAWN    0b00000001
#define EMPTY   0

#define MAX_POSSIBLE_MOVEMENTS 64

#define MAX_KING_POSSIBLE_MOVEMENTS 8
#define MAX_DIAGONAL_POSSIBLE_MOVEMENTS 7
#define MAX_KNIGHT_POSSIBLE_MOVEMENTS 8
#define MAX_LINE_POSSIBLE_MOVEMENTS 7
#define MAX_PAWN_EAT_MOVEMENTS 2

typedef struct XYTuple {
    int x;
    int y;
} Tuple;

void renderGameState(AppItems *app_items);
void displayAction(AppItems *app_items, int x, int y);
void getPossibleMovements(uint8_t piece, int i, int j);
void renderPossibleMovements(AppItems *app_items, SDL_Rect *renderer_piece_rect);
int validBoardArrayCoords(int i, int j);
int isEnemyTeam(uint8_t piece_1, uint8_t piece_2);
int isPossibleMovement(int x, int y);
void switchCurrentPlayer();
uint8_t getOpponent();
Tuple getKingOfTeam();
int isCheck(Tuple king_position);
int isChessMate();

#endif