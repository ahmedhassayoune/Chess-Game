#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "chess.h"
#include "game_logic.h"

uint8_t game_state[NB_EDGE_SQUARES][NB_EDGE_SQUARES] =
{
    {ROOK | PLAYER2, KNIGHT | PLAYER2, BISHOP | PLAYER2, QUEEN | PLAYER2, KING | PLAYER2, BISHOP | PLAYER2, KNIGHT | PLAYER2, ROOK | PLAYER2},
    {PAWN | PLAYER2, PAWN | PLAYER2, PAWN | PLAYER2, PAWN | PLAYER2, PAWN | PLAYER2, PAWN | PLAYER2, PAWN | PLAYER2, PAWN | PLAYER2},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {PAWN | PLAYER1, PAWN | PLAYER1, PAWN | PLAYER1, PAWN | PLAYER1, PAWN | PLAYER1, PAWN | PLAYER1, PAWN | PLAYER1, PAWN | PLAYER1},
    {ROOK | PLAYER1, KNIGHT | PLAYER1, BISHOP | PLAYER1, QUEEN | PLAYER1, KING | PLAYER1, BISHOP | PLAYER1, KNIGHT | PLAYER1, ROOK | PLAYER1}
};

uint8_t piece_action_info[] = 
{
    0,          // Ready to move?
    0,          // Selected piece
    0,          // Piece x coordinate
    0           // Piece y coordinate
};

Tuple possible_movements[MAX_POSSIBLE_MOVEMENTS];

Tuple king_movements[MAX_KING_POSSIBLE_MOVEMENTS] =
{
    {.x = -1, .y = -1}, // diag1
    {.x = -1, .y = 0},  // horz1
    {.x = -1, .y = 1},  // diag4
    {.x = 0, .y = 1},   // vert2
    {.x = 1, .y = -1},  // diag2
    {.x = 1, .y = 0},   // horz2
    {.x = 1, .y = 1},   // diag3
    {.x = 0, .y = -1}   // vert1
};

Tuple knight_movements[MAX_KNIGHT_POSSIBLE_MOVEMENTS] =
{
    {.x = -2, .y = -1},
    {.x = -2, .y = 1},
    {.x = 2, .y = -1},
    {.x = 2, .y = 1},
    {.x = -1, .y = -2},
    {.x = 1, .y = -2},
    {.x = -1, .y = 2},
    {.x = 1, .y = 2}
};

Tuple diagonal_ul_movements[MAX_DIAGONAL_POSSIBLE_MOVEMENTS] =
{
    // Diag upper left
    {.x = -1, .y = -1},
    {.x = -2, .y = -2},
    {.x = -3, .y = -3},
    {.x = -4, .y = -4},
    {.x = -5, .y = -5},
    {.x = -6, .y = -6},
    {.x = -7, .y = -7},
};

Tuple diagonal_ur_movements[MAX_DIAGONAL_POSSIBLE_MOVEMENTS] =
{
    // Diag upper right
    {.x = 1, .y = -1},
    {.x = 2, .y = -2},
    {.x = 3, .y = -3},
    {.x = 4, .y = -4},
    {.x = 5, .y = -5},
    {.x = 6, .y = -6},
    {.x = 7, .y = -7}
};

Tuple diagonal_bl_movements[MAX_DIAGONAL_POSSIBLE_MOVEMENTS] =
{
    // Diag bottom left
    {.x = -1, .y = 1},
    {.x = -2, .y = 2},
    {.x = -3, .y = 3},
    {.x = -4, .y = 4},
    {.x = -5, .y = 5},
    {.x = -6, .y = 6},
    {.x = -7, .y = 7}
};

Tuple diagonal_br_movements[MAX_DIAGONAL_POSSIBLE_MOVEMENTS] =
{
    // Diag bottom right
    {.x = 1, .y = 1},
    {.x = 2, .y = 2},
    {.x = 3, .y = 3},
    {.x = 4, .y = 4},
    {.x = 5, .y = 5},
    {.x = 6, .y = 6},
    {.x = 7, .y = 7}
};

Tuple line_uv_movements[MAX_LINE_POSSIBLE_MOVEMENTS] =
{
    // Upper vertical
    {.x = 0, .y = -1},
    {.x = 0, .y = -2},
    {.x = 0, .y = -3},
    {.x = 0, .y = -4},
    {.x = 0, .y = -5},
    {.x = 0, .y = -6},
    {.x = 0, .y = -7},
};

Tuple line_rh_movements[MAX_LINE_POSSIBLE_MOVEMENTS] =
{
    // Right horizontal
    {.x = 1, .y = 0},
    {.x = 2, .y = 0},
    {.x = 3, .y = 0},
    {.x = 4, .y = 0},
    {.x = 5, .y = 0},
    {.x = 6, .y = 0},
    {.x = 7, .y = 0},
};

Tuple line_bv_movements[MAX_LINE_POSSIBLE_MOVEMENTS] =
{
    // Bottom vertical
    {.x = 0, .y = 1},
    {.x = 0, .y = 2},
    {.x = 0, .y = 3},
    {.x = 0, .y = 4},
    {.x = 0, .y = 5},
    {.x = 0, .y = 6},
    {.x = 0, .y = 7}
};

Tuple line_lh_movements[MAX_LINE_POSSIBLE_MOVEMENTS] =
{
    // Left horizontal
    {.x = -1, .y = 0},
    {.x = -2, .y = 0},
    {.x = -3, .y = 0},
    {.x = -4, .y = 0},
    {.x = -5, .y = 0},
    {.x = -6, .y = 0},
    {.x = -7, .y = 0}
};

Tuple pawn_p1_movements[1] =
{
    {.x = 0, .y = -1}
};

Tuple pawn_p1_eat_movements[2] =
{
    {.x = -1, .y = -1},
    {.x = 1, .y = -1}
};

Tuple pawn_p2_movements[1] =
{
    {.x = 0, .y = 1}
};

Tuple pawn_p2_eat_movements[2] =
{
    {.x = -1, .y = 1},
    {.x = 1, .y = 1}
};

Tuple king_p1 = {.x = 4, .y = 7};
Tuple king_p2 = {.x = 4, .y = 0};

int current_player = 0b10000000;
int is_check = 0;

void renderGameState(AppItems *app_items)
{
    SDL_Renderer *renderer = *(app_items->renderer);
    SDL_Rect *darea = app_items->darea;

    // Clear the rendering surface with the specified color
    setRenderDrawColor(renderer, WHITE_COLOR);
    SDL_RenderClear(renderer);

    // Draw the chess board
    drawChessBoard(app_items);

    // Render the chess pieces from the game state array
    renderChessPieces(app_items, game_state);

    // Color the board outline
    setRenderDrawColor(renderer, BLACK_COLOR);
    SDL_RenderDrawRect(renderer, darea);

    SDL_RenderPresent(renderer);
}

void displayAction(AppItems *app_items, int x, int y)
{
    SDL_Renderer *renderer = *(app_items->renderer);
    SDL_Rect *darea = app_items->darea;

    int square_size = darea->w / NB_EDGE_SQUARES - 1;

    removeWindowIndex(app_items, &x, &y);

    if (withinBoardSurface(x, y, darea->w))
    {
        x = boardToArrayIndex(x, darea->w);
        y = boardToArrayIndex(y, darea->w);

        if (piece_action_info[0]) // if ready to move
        {
            if (isPossibleMovement(x, y))
            {
                // Pawn promotion
                if((y == 0 && game_state[piece_action_info[3]][piece_action_info[2]] == (PLAYER1 | PAWN)) ||
                    (y == 7 && game_state[piece_action_info[3]][piece_action_info[2]] == (PLAYER2 | PAWN)))
                {
                    char promotion_piece = 'a';

                    printf("Choose the promotion piece that will replace your pawn.\nEnter Q for Queen, R for Rook, B for Bishop and K for Knight:\n");
                    fscanf(stdin, "%c", &promotion_piece);

                    while (promotion_piece != 'Q' && promotion_piece != 'q' && 
                            promotion_piece != 'R' && promotion_piece != 'r' &&
                            promotion_piece != 'B' && promotion_piece != 'b' &&
                            promotion_piece != 'K' && promotion_piece != 'k')
                    {
                        printf("Wrong entry.\nEnter Q for Queen, R for Rook, B for Bishop and K for Knight:\n");
                        
                        fscanf(stdin, "%c", &promotion_piece);
                    }
                    
                    switch (promotion_piece)
                    {
                    case 'Q':
                    case 'q':
                        game_state[y][x] = current_player | QUEEN;
                        break;
                    case 'R':
                    case 'r':
                        game_state[y][x] = current_player | ROOK;
                        break;
                    case 'B':
                    case 'b':
                        game_state[y][x] = current_player | BISHOP;
                        break;
                    default: // Knight
                        game_state[y][x] = current_player | KNIGHT;
                        break;
                    }
                }
                else
                {
                    game_state[y][x] = piece_action_info[1];
                    if ((piece_action_info[1] & KING) == KING)
                    {
                        if(current_player == PLAYER1)
                        {
                            king_p1.x = x;
                            king_p1.y = y;
                        }
                        else
                        {
                            king_p2.x = x;
                            king_p2.y = y;
                        }
                    }
                }
                game_state[piece_action_info[3]][piece_action_info[2]] = EMPTY;
                switchCurrentPlayer();

                if (isCheck(getKingOfTeam()))
                {
                    if (isChessMate(current_player))
                    {
                        printf("END!\n");
                        if (current_player == PLAYER1)
                            printf("Black won!\n");
                        else
                            printf("White won!\n");

                        exit(0);
                    }
                }
            }
            piece_action_info[0] = 0;
            renderGameState(app_items);
        }
        else if (game_state[y][x] != EMPTY && ((game_state[y][x] & current_player) == current_player))
        {
            renderGameState(app_items);

            piece_action_info[1] = game_state[y][x];
            piece_action_info[2] = x;
            piece_action_info[3] = y;

            possible_movements[0].y = -1;
            possible_movements[0].x = -1;

            SDL_Rect renderer_piece_rect = 
            {
                .h = square_size,
                .w = square_size,
                .x = darea->x + x * (square_size + 1),
                .y = darea->y + y * (square_size + 1)
            };

            // Outline the selected piece with the given color
            setRenderDrawColor(renderer, SELECTION_COLOR);
            SDL_RenderDrawRect(renderer, &renderer_piece_rect);

            // Outline the possible movements with the given color
            getPossibleMovements(game_state[y][x], y, x);
            renderPossibleMovements(app_items, &renderer_piece_rect);
            
            SDL_RenderPresent(renderer);
        }
    }
    else
    {
        printf("ERROR -> MOUSE CLICK IS NOT WITHIN BOARD SURFACE\n");
    }
}

void getPossibleMovements(uint8_t piece, int i, int j)
{
    int k = 0;

    switch(piece & 0b00111111)
    {
        case KING:
        {
            for(int n = 0; n < MAX_KING_POSSIBLE_MOVEMENTS; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + king_movements[n].y, j + king_movements[n].x)
                    && ((game_state[i + king_movements[n].y][j + king_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + king_movements[n].y][j + king_movements[n].x]))))
                {
                    possible_movements[k].y = i + king_movements[n].y;
                    possible_movements[k].x = j + king_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = KING | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = KING | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = KING | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                }
            }

            break;
        }
        case QUEEN:
        {
            int diag1 = 0,
                diag2 = 0,
                diag3 = 0,
                diag4 = 0,
                line1 = 0,
                line2 = 0,
                line3 = 0,
                line4 = 0;
            
            for(int n = 0; n < MAX_DIAGONAL_POSSIBLE_MOVEMENTS && !diag1; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS
                    && validBoardArrayCoords(i + diagonal_ul_movements[n].y, j + diagonal_ul_movements[n].x)
                    && ((game_state[i + diagonal_ul_movements[n].y][j + diagonal_ul_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + diagonal_ul_movements[n].y][j + diagonal_ul_movements[n].x]))))
                {
                    possible_movements[k].y = i + diagonal_ul_movements[n].y;
                    possible_movements[k].x = j + diagonal_ul_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = QUEEN | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + diagonal_ul_movements[n].y][j + diagonal_ul_movements[n].x] != EMPTY)
                        diag1 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_DIAGONAL_POSSIBLE_MOVEMENTS && !diag2; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + diagonal_ur_movements[n].y, j + diagonal_ur_movements[n].x)
                    && ((game_state[i + diagonal_ur_movements[n].y][j + diagonal_ur_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + diagonal_ur_movements[n].y][j + diagonal_ur_movements[n].x]))))
                {
                    possible_movements[k].y = i + diagonal_ur_movements[n].y;
                    possible_movements[k].x = j + diagonal_ur_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = QUEEN | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + diagonal_ur_movements[n].y][j + diagonal_ur_movements[n].x] != EMPTY)
                        diag2 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_DIAGONAL_POSSIBLE_MOVEMENTS && !diag3; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + diagonal_bl_movements[n].y, j + diagonal_bl_movements[n].x)
                    && ((game_state[i + diagonal_bl_movements[n].y][j + diagonal_bl_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + diagonal_bl_movements[n].y][j + diagonal_bl_movements[n].x]))))
                {
                    possible_movements[k].y = i + diagonal_bl_movements[n].y;
                    possible_movements[k].x = j + diagonal_bl_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = QUEEN | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + diagonal_bl_movements[n].y][j + diagonal_bl_movements[n].x] != EMPTY)
                        diag3 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_DIAGONAL_POSSIBLE_MOVEMENTS && !diag4; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + diagonal_br_movements[n].y, j + diagonal_br_movements[n].x)
                    && ((game_state[i + diagonal_br_movements[n].y][j + diagonal_br_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + diagonal_br_movements[n].y][j + diagonal_br_movements[n].x]))))
                {
                    possible_movements[k].y = i + diagonal_br_movements[n].y;
                    possible_movements[k].x = j + diagonal_br_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = QUEEN | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + diagonal_br_movements[n].y][j + diagonal_br_movements[n].x] != EMPTY)
                        diag4 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_LINE_POSSIBLE_MOVEMENTS && !line1; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + line_uv_movements[n].y, j + line_uv_movements[n].x)
                    && ((game_state[i + line_uv_movements[n].y][j + line_uv_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + line_uv_movements[n].y][j + line_uv_movements[n].x]))))
                {
                    possible_movements[k].y = i + line_uv_movements[n].y;
                    possible_movements[k].x = j + line_uv_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = QUEEN | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + line_uv_movements[n].y][j + line_uv_movements[n].x] != EMPTY)
                        line1 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_LINE_POSSIBLE_MOVEMENTS && !line2; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + line_rh_movements[n].y, j + line_rh_movements[n].x)
                    && ((game_state[i + line_rh_movements[n].y][j + line_rh_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + line_rh_movements[n].y][j + line_rh_movements[n].x]))))
                {
                    possible_movements[k].y = i + line_rh_movements[n].y;
                    possible_movements[k].x = j + line_rh_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = QUEEN | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + line_rh_movements[n].y][j + line_rh_movements[n].x] != EMPTY)
                        line2 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_LINE_POSSIBLE_MOVEMENTS && !line3; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + line_bv_movements[n].y, j + line_bv_movements[n].x)
                    && ((game_state[i + line_bv_movements[n].y][j + line_bv_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + line_bv_movements[n].y][j + line_bv_movements[n].x]))))
                {
                    possible_movements[k].y = i + line_bv_movements[n].y;
                    possible_movements[k].x = j + line_bv_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = QUEEN | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + line_bv_movements[n].y][j + line_bv_movements[n].x] != EMPTY)
                        line3 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_LINE_POSSIBLE_MOVEMENTS && !line4; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + line_lh_movements[n].y, j + line_lh_movements[n].x)
                    && ((game_state[i + line_lh_movements[n].y][j + line_lh_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + line_lh_movements[n].y][j + line_lh_movements[n].x]))))
                {
                    possible_movements[k].y = i + line_lh_movements[n].y;
                    possible_movements[k].x = j + line_lh_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = QUEEN | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = QUEEN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + line_lh_movements[n].y][j + line_lh_movements[n].x] != EMPTY)
                        line4 = 1;
                }
                else
                    break;
            }

            break;
        }
        case BISHOP:
        {
            int diag1 = 0,
                diag2 = 0,
                diag3 = 0,
                diag4 = 0;

            for(int n = 0; n < MAX_DIAGONAL_POSSIBLE_MOVEMENTS && !diag1; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + diagonal_ul_movements[n].y, j + diagonal_ul_movements[n].x)
                    && ((game_state[i + diagonal_ul_movements[n].y][j + diagonal_ul_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + diagonal_ul_movements[n].y][j + diagonal_ul_movements[n].x]))))
                {
                    possible_movements[k].y = i + diagonal_ul_movements[n].y;
                    possible_movements[k].x = j + diagonal_ul_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = BISHOP | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = BISHOP | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = BISHOP | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + diagonal_ul_movements[n].y][j + diagonal_ul_movements[n].x] != EMPTY)
                        diag1 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_DIAGONAL_POSSIBLE_MOVEMENTS && !diag2; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + diagonal_ur_movements[n].y, j + diagonal_ur_movements[n].x)
                    && ((game_state[i + diagonal_ur_movements[n].y][j + diagonal_ur_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + diagonal_ur_movements[n].y][j + diagonal_ur_movements[n].x]))))
                {
                    possible_movements[k].y = i + diagonal_ur_movements[n].y;
                    possible_movements[k].x = j + diagonal_ur_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = BISHOP | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = BISHOP | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = BISHOP | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + diagonal_ur_movements[n].y][j + diagonal_ur_movements[n].x] != EMPTY)
                        diag2 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_DIAGONAL_POSSIBLE_MOVEMENTS && !diag3; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + diagonal_bl_movements[n].y, j + diagonal_bl_movements[n].x)
                    && ((game_state[i + diagonal_bl_movements[n].y][j + diagonal_bl_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + diagonal_bl_movements[n].y][j + diagonal_bl_movements[n].x]))))
                {
                    possible_movements[k].y = i + diagonal_bl_movements[n].y;
                    possible_movements[k].x = j + diagonal_bl_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = BISHOP | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = BISHOP | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = BISHOP | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + diagonal_bl_movements[n].y][j + diagonal_bl_movements[n].x] != EMPTY)
                        diag3 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_DIAGONAL_POSSIBLE_MOVEMENTS && !diag4; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + diagonal_br_movements[n].y, j + diagonal_br_movements[n].x)
                    && ((game_state[i + diagonal_br_movements[n].y][j + diagonal_br_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + diagonal_br_movements[n].y][j + diagonal_br_movements[n].x]))))
                {
                    possible_movements[k].y = i + diagonal_br_movements[n].y;
                    possible_movements[k].x = j + diagonal_br_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = BISHOP | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = BISHOP | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = BISHOP | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + diagonal_br_movements[n].y][j + diagonal_br_movements[n].x] != EMPTY)
                        diag4 = 1;
                }
                else
                    break;
            }

            break;
        }
        case KNIGHT:
        {
            for(int n = 0; n < MAX_KNIGHT_POSSIBLE_MOVEMENTS; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + knight_movements[n].y, j + knight_movements[n].x)
                    && ((game_state[i + knight_movements[n].y][j + knight_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + knight_movements[n].y][j + knight_movements[n].x]))))
                {
                    possible_movements[k].y = i + knight_movements[n].y;
                    possible_movements[k].x = j + knight_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = KNIGHT | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = KNIGHT | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = KNIGHT | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }
                }
            }

            break;
        }
        case ROOK:
        {
            int line1 = 0,
                line2 = 0,
                line3 = 0,
                line4 = 0;

            for(int n = 0; n < MAX_LINE_POSSIBLE_MOVEMENTS && !line1; n++)
            {                
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + line_uv_movements[n].y, j + line_uv_movements[n].x)
                    && ((game_state[i + line_uv_movements[n].y][j + line_uv_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + line_uv_movements[n].y][j + line_uv_movements[n].x]))))
                {
                    possible_movements[k].y = i + line_uv_movements[n].y;
                    possible_movements[k].x = j + line_uv_movements[n].x;

                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = ROOK | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = ROOK | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = ROOK | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + line_uv_movements[n].y][j + line_uv_movements[n].x] != EMPTY)
                        line1 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_LINE_POSSIBLE_MOVEMENTS && !line2; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + line_rh_movements[n].y, j + line_rh_movements[n].x)
                    && ((game_state[i + line_rh_movements[n].y][j + line_rh_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + line_rh_movements[n].y][j + line_rh_movements[n].x]))))
                {
                    possible_movements[k].y = i + line_rh_movements[n].y;
                    possible_movements[k].x = j + line_rh_movements[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = ROOK | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = ROOK | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = ROOK | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + line_rh_movements[n].y][j + line_rh_movements[n].x] != EMPTY)
                        line2 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_LINE_POSSIBLE_MOVEMENTS && !line3; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + line_bv_movements[n].y, j + line_bv_movements[n].x)
                    && ((game_state[i + line_bv_movements[n].y][j + line_bv_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + line_bv_movements[n].y][j + line_bv_movements[n].x]))))
                {
                    possible_movements[k].y = i + line_bv_movements[n].y;
                    possible_movements[k].x = j + line_bv_movements[n].x;

                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = ROOK | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = ROOK | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = ROOK | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + line_bv_movements[n].y][j + line_bv_movements[n].x] != EMPTY)
                        line3 = 1;
                }
                else
                    break;
            }

            for(int n = 0; n < MAX_LINE_POSSIBLE_MOVEMENTS && !line4; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + line_lh_movements[n].y, j + line_lh_movements[n].x)
                    && ((game_state[i + line_lh_movements[n].y][j + line_lh_movements[n].x] == EMPTY) 
                    || (isEnemyTeam(piece, game_state[i + line_lh_movements[n].y][j + line_lh_movements[n].x]))))
                {
                    possible_movements[k].y = i + line_lh_movements[n].y;
                    possible_movements[k].x = j + line_lh_movements[n].x;

                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = ROOK | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = ROOK | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = ROOK | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }

                    if(game_state[i + line_lh_movements[n].y][j + line_lh_movements[n].x] != EMPTY)
                        line4 = 1;
                }
                else
                    break;
            }

            break;
        }
        case PAWN:
        {
            Tuple *pawn_movement_array = pawn_p1_movements;
            Tuple *pawn_eat_movement_array = pawn_p1_eat_movements;
            int l = -1;
            if ((piece & PLAYER2) == PLAYER2)
            {
                pawn_movement_array = pawn_p2_movements;
                pawn_eat_movement_array = pawn_p2_eat_movements;
                l = 1;
            }

            // Forward movement (1 square)
            if (k < MAX_POSSIBLE_MOVEMENTS 
                && validBoardArrayCoords(i + pawn_movement_array[0].y, j + pawn_movement_array[0].x)
                && game_state[i + pawn_movement_array[0].y][j + pawn_movement_array[0].x] == EMPTY)
            {
                possible_movements[k].y = i + pawn_movement_array[0].y;
                possible_movements[k].x = j + pawn_movement_array[0].x;
                
                game_state[i][j] = EMPTY;
                uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                game_state[possible_movements[k].y][possible_movements[k].x] = PAWN | current_player;

                if(isCheck(getKingOfTeam()))
                {
                    game_state[i][j] = PAWN | current_player;
                    game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                    possible_movements[k].y = -1;
                    possible_movements[k].x = -1;
                }
                else
                {
                    game_state[i][j] = PAWN | current_player;
                    game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                    k += 1;
                }
            }

            // Forward movement (2 squares)
            if (k < MAX_POSSIBLE_MOVEMENTS
                && ((i == 6 && ((piece & PLAYER1) == PLAYER1)) || (i == 1 && ((piece & PLAYER2) == PLAYER2)))
                && validBoardArrayCoords(i + 2 * l, j)
                && game_state[i + 2 * l][j] == EMPTY)
            {
                possible_movements[k].y = i + 2 * l;
                possible_movements[k].x = j;
                
                game_state[i][j] = EMPTY;
                uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                game_state[possible_movements[k].y][possible_movements[k].x] = PAWN | current_player;

                if(isCheck(getKingOfTeam()))
                {
                    game_state[i][j] = PAWN | current_player;
                    game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                    possible_movements[k].y = -1;
                    possible_movements[k].x = -1;
                }
                else
                {
                    game_state[i][j] = PAWN | current_player;
                    game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                    k += 1;
                }
            }

            // Eating movements
            for(int n = 0; n < MAX_PAWN_EAT_MOVEMENTS; n++)
            {
                if (k < MAX_POSSIBLE_MOVEMENTS 
                    && validBoardArrayCoords(i + pawn_eat_movement_array[n].y, j + pawn_eat_movement_array[n].x)
                    && game_state[i + pawn_eat_movement_array[n].y][j + pawn_eat_movement_array[n].x] != EMPTY 
                    && isEnemyTeam(piece, game_state[i + pawn_eat_movement_array[n].y][j + pawn_eat_movement_array[n].x]))
                {
                    possible_movements[k].y = i + pawn_eat_movement_array[n].y;
                    possible_movements[k].x = j + pawn_eat_movement_array[n].x;
                    
                    game_state[i][j] = EMPTY;
                    uint8_t eaten_piece = game_state[possible_movements[k].y][possible_movements[k].x];
                    game_state[possible_movements[k].y][possible_movements[k].x] = PAWN | current_player;

                    if(isCheck(getKingOfTeam()))
                    {
                        game_state[i][j] = PAWN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        possible_movements[k].y = -1;
                        possible_movements[k].x = -1;
                    }
                    else
                    {
                        game_state[i][j] = PAWN | current_player;
                        game_state[possible_movements[k].y][possible_movements[k].x] = eaten_piece;

                        k += 1;
                    }
                }
            }

            break;
        }
        default:
            break;
    }

    possible_movements[k].y = -1;
    possible_movements[k].x = -1;

    if(k > 0)
        piece_action_info[0] = 1;
}

void renderPossibleMovements(AppItems *app_items, SDL_Rect *renderer_piece_rect)
{
    SDL_Renderer *renderer = *(app_items->renderer);
    setRenderDrawColor(renderer, POSSIBLE_MOVEMENTS_COLOR);

    int square_size = renderer_piece_rect->w;

    int i = 0;
    while(i < MAX_POSSIBLE_MOVEMENTS && possible_movements[i].x != -1)
    {
        renderer_piece_rect->x = app_items->darea->x + possible_movements[i].x * (square_size + 1);
        renderer_piece_rect->y = app_items->darea->y + possible_movements[i].y * (square_size + 1);
        
        SDL_RenderDrawRect(renderer, renderer_piece_rect);
        i += 1;
    }
}

int validBoardArrayCoords(int i, int j)
{
    return (i >= 0 && i < NB_EDGE_SQUARES) && (j >= 0 && j < NB_EDGE_SQUARES);
}

int isEnemyTeam(uint8_t piece_1, uint8_t piece_2)
{
    return (piece_1 & 0b11000000) != (piece_2 & 0b11000000);
}

int isPossibleMovement(int x, int y)
{
    int i = 0;
    while (i < MAX_POSSIBLE_MOVEMENTS && possible_movements[i].x != -1)
    {
        if (possible_movements[i].y == y && possible_movements[i].x == x)
            return 1;
        
        i += 1;
    }
    return 0;
}

void switchCurrentPlayer()
{
    if (current_player == PLAYER1)
    {
        current_player = PLAYER2;
    }
    else
    {
        current_player = PLAYER1;
    }
}

uint8_t getOpponent()
{
    if (current_player == PLAYER1)
        return PLAYER2;
    return PLAYER1;
}

Tuple getKingOfTeam()
{
    if (current_player == PLAYER1)
        return king_p1;
    return king_p2;
}

int isCheck(Tuple king_position)
{
    int diag1 = 0,
        diag2 = 4,
        diag3 = 6,
        diag4 = 2,
        vert1 = 7,
        vert2 = 3,
        horz1 = 1,
        horz2 = 5;
    
    int directions[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < MAX_KING_POSSIBLE_MOVEMENTS; i++)
    {
        int x = king_position.x + king_movements[i].x;
        int y = king_position.y + king_movements[i].y;
        
        if(!validBoardArrayCoords(y, x))
            continue;

        if (!isEnemyTeam(game_state[y][x], current_player))
            directions[i] = 1;
        else
        {
            uint8_t enemy_piece = game_state[y][x] & 0b00111111;
            if (enemy_piece == QUEEN || enemy_piece == KING)
            {
                is_check = 1;
                return 1;
            }
            if (enemy_piece == BISHOP 
                && (i == 0 || i == 2 || i == 4 || i == 6))
            {
                is_check = 1;
                return 1;
            }
            if (enemy_piece == ROOK 
            && (i == 1 || i == 3 || i == 5 || i == 7))
            {
                is_check = 1;
                return 1;
            }
            if (enemy_piece == PAWN 
                && (i == 0 || i == 6))
            {
                is_check = 1;
                return 1;
            }
            if (enemy_piece == KNIGHT)
                directions[i] = 1;
        }
    }
    
    // Diagonal 1
    for(int i = 2; i < MAX_KING_POSSIBLE_MOVEMENTS && !directions[diag1]; i++)
    {
        int x = king_position.x - i;
        int y = king_position.y - i;

        if(!validBoardArrayCoords(y, x))
        {
            directions[diag1] = 1;
            break;
        }
        
        if (game_state[y][x] == EMPTY)
            continue;
        else if (!isEnemyTeam(game_state[y][x], current_player))
            directions[diag1] = 1;
        else
        {
            uint8_t enemy_piece = game_state[y][x] & 0b00111111;
            if (enemy_piece == QUEEN 
            ||  enemy_piece == BISHOP)
            {
                is_check = 1;
                return 1;
            }
        }
    }
    
    // Diagonal 2
    for(int i = 2; i < MAX_KING_POSSIBLE_MOVEMENTS && !directions[diag2]; i++)
    {
        int x = king_position.x + i;
        int y = king_position.y - i;

        if(!validBoardArrayCoords(y, x))
        {
            directions[diag2] = 1;
            break;
        }

        if (game_state[y][x] == EMPTY)
            continue;
        else if (!isEnemyTeam(game_state[y][x], current_player))
            directions[diag2] = 1;
        else
        {
            uint8_t enemy_piece = game_state[y][x] & 0b00111111;
            if (enemy_piece == QUEEN 
            ||  enemy_piece == BISHOP)
            {
                is_check = 1;
                return 1;
            }
        }
    }
    
    // Diagonal 3
    for(int i = 2; i < MAX_KING_POSSIBLE_MOVEMENTS && !directions[diag3]; i++)
    {
        int x = king_position.x + i;
        int y = king_position.y + i;

        if(!validBoardArrayCoords(y, x))
        {
            directions[diag3] = 1;
            break;
        }

        if (game_state[y][x] == EMPTY)
            continue;
        else if (!isEnemyTeam(game_state[y][x], current_player))
            directions[diag3] = 1;
        else
        {
            uint8_t enemy_piece = game_state[y][x] & 0b00111111;
            if (enemy_piece == QUEEN 
            ||  enemy_piece == BISHOP)
            {
                is_check = 1;
                return 1;
            }
        }
    }

    // Diagonal 4
    for(int i = 2; i < MAX_KING_POSSIBLE_MOVEMENTS && !directions[diag4]; i++)
    {
        int x = king_position.x - i;
        int y = king_position.y + i;

        if(!validBoardArrayCoords(y, x))
        {
            directions[diag4] = 1;
            break;
        }

        if (game_state[y][x] == EMPTY)
            continue;
        else if (!isEnemyTeam(game_state[y][x], current_player))
            directions[diag4] = 1;
        else
        {
            uint8_t enemy_piece = game_state[y][x] & 0b00111111;
            if (enemy_piece == QUEEN 
            ||  enemy_piece == BISHOP)
            {
                is_check = 1;
                return 1;
            }
        }
    }

    // Vertical 1
    for(int i = 2; i < MAX_KING_POSSIBLE_MOVEMENTS && !directions[vert1]; i++)
    {
        int x = king_position.x;
        int y = king_position.y - i;

        if(!validBoardArrayCoords(y, x))
        {
            directions[vert1] = 1;
            break;
        }

        if (game_state[y][x] == EMPTY)
            continue;
        else if (!isEnemyTeam(game_state[y][x], current_player))
        {
            directions[vert1] = 1;
        }
        else
        {
            uint8_t enemy_piece = game_state[y][x] & 0b00111111;
            if (enemy_piece == QUEEN 
            ||  enemy_piece == ROOK)
            {
                is_check = 1;
                return 1;
            }
        }
    }

    // Vertical 2
    for(int i = 2; i < MAX_KING_POSSIBLE_MOVEMENTS && !directions[vert2]; i++)
    {
        int x = king_position.x;
        int y = king_position.y + i;

        if(!validBoardArrayCoords(y, x))
        {
            directions[vert2] = 1;
            break;
        }

        if (game_state[y][x] == EMPTY)
            continue;
        else if (!isEnemyTeam(game_state[y][x], current_player))
            directions[vert2] = 1;
        else
        {
            uint8_t enemy_piece = game_state[y][x] & 0b00111111;
            if (enemy_piece == QUEEN 
            ||  enemy_piece == ROOK)
            {
                is_check = 1;
                return 1;
            }
        }
    }

    // Horizontal 1
    for(int i = 2; i < MAX_KING_POSSIBLE_MOVEMENTS && !directions[horz1]; i++)
    {
        int x = king_position.x - i;
        int y = king_position.y;

        if(!validBoardArrayCoords(y, x))
        {
            directions[horz1] = 1;
            break;
        }

        if (game_state[y][x] == EMPTY)
            continue;
        else if (!isEnemyTeam(game_state[y][x], current_player))
            directions[horz1] = 1;
        else
        {
            uint8_t enemy_piece = game_state[y][x] & 0b00111111;
            if (enemy_piece == QUEEN 
            ||  enemy_piece == ROOK)
            {
                is_check = 1;
                return 1;
            }
        }
    }

    // Horizontal 2
    for(int i = 2; i < MAX_KING_POSSIBLE_MOVEMENTS && !directions[horz2]; i++)
    {
        int x = king_position.x + i;
        int y = king_position.y;

        if(!validBoardArrayCoords(y, x))
        {
            directions[horz2] = 1;
            break;
        }

        if (game_state[y][x] == EMPTY)
            continue;
        else if (!isEnemyTeam(game_state[y][x], current_player))
            directions[horz2] = 1;
        else
        {
            uint8_t enemy_piece = game_state[y][x] & 0b00111111;
            if (enemy_piece == QUEEN 
            ||  enemy_piece == ROOK)
            {
                is_check = 1;
                return 1;
            }
        }
    }

    // Knights
    for (int i = 0; i < MAX_KNIGHT_POSSIBLE_MOVEMENTS; i++)
    {
        int x = king_position.x + knight_movements[i].x;
        int y = king_position.y + knight_movements[i].y;

        if(!validBoardArrayCoords(y, x))
            continue;

        uint8_t piece = game_state[y][x] & 0b00111111;
        if (isEnemyTeam(game_state[y][x], current_player) && piece == KNIGHT)
        {
            is_check = 1;
            return 1;
        }
    }
    
    is_check = 0;
    return 0;
}

int isChessMate(int PLAYER_TEAM)
{
    for (int i = 0; i < NB_EDGE_SQUARES; i++)
    {
        for (int j = 0; j < NB_EDGE_SQUARES; j++)
        {
            if ((game_state[i][j] & 0b11000000) == PLAYER_TEAM)
            {
                getPossibleMovements(game_state[i][j], i, j);
                if(possible_movements[0].x != -1)
                    return 0;
            }
        }
        
    }
    
    return 1;
}