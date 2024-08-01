#include "timer/timer.h"
#include "GLCD/GLCD.h"

void startGame(void);
void switchTurn(int moveDone);
void colorPossibleMoves(int color, int cellValue);
void skipTurn(void);
int getAxisPosPixel(int axisPos);
int checkWallAvailable(void);
void moveWall(int direction);
void placeWall(void);
int move(int direction, int confirm);
void prepareGame(void);
int moveNPC(void);

extern int numBoards, human_NPC, gameMode, turn, xPosPlacingWall, yPosPlacingWall, wallDirection;
extern uint32_t lastMove;
extern unsigned char gameMatrix[13][13];
