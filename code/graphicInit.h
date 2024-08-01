#include "GLCD/GLCD.h"

void printGrill(void);
void printSquare(uint16_t xPosStart, uint16_t yPosStart);
void printGameInfo(void);
void printTexts(void);
void printWallsNumberP1(uint8_t * wallsNumber);
void printWallsNumberP2(uint8_t * wallsNumber);
void colorSquare(int xPos, int yPos, int color);
void printPlayerTime(int playerTime);
void printWall(int xPos, int yPos, int color, int direction);
void printWallEnded(int color);
void printPlayerWins(int playerColor);
void printMenu1(void);
void printMenu2(char text1[20], char text2[20]);
void selectMenuButton(int selection);
void confirmMenu1Choice(int selection);
void confirmMenu2Choice(int selection);

