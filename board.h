#ifndef BOARD_H
#define BOARD_H

#include "types.h"
#include <stdbool.h>

int getCurrPlayer();
int getCurrPawn();
void setKingPawn(bool);

void setCurrPawn(int i);

u8 *getBoard();
void setBoard(u8 cell, u8 value);
int getOpponent();

bool isOpponentPawnOrKing(int value);
void nextPlayer();

bool isPlayerPawn(u8 col, u8 row);
bool isKingPawn(u8 col, u8 row);
bool isCurrentPawn(int col, int row);

u8 getRow(u8 idx);
u8 getCol(u8 idx);

u8 getCell(int col, int row);
bool isIdxBlack(u8 idx);
bool isIdxTopTerritory(u8 idx);
bool isIdxBotTerritory(u8 idx);

void initBoard(ui row, ui col);
void freeBoard();

uint32_t handleColor(int col, int row, u8 player);
void movePawn(int s, int t);

void handlePromotion();
#endif
