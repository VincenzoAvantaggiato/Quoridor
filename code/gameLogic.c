#include "gameLogic.h"
#include "graphicInit.h"
#include "./timer/timer.h"
#include "stdio.h"

int turn = 1; // 1 -> turno P1, 2 -> turno P2
int xP1, yP1, xP2, yP2, wallsP1, wallsP2; // variabili di stato giocatori
unsigned char gameMatrix[13][13];
int gameMode = 0; // 0 -> wait mode, 1 -> game mode, 2 -> wall mode
int numBoards = 0;
int human_NPC = 0; // 1 -> human, 2 -> NPC
int xPosPlacingWall, yPosPlacingWall, wallDirection; // 0 -> orizzontale, 1 -> verticale
uint32_t lastMove;
int recursions1;
int recursions2;
int recursions3;
int recursions4;

void initGameMatrix(void);
void skipTurn(void);
void setXPosPlayer(int xPos);
void setYPosPlayer(int yPos);
int checkAvailablePathToWin(int i, int j, int player, int gameMatrixCopy[13][13]);
int movePlayer(int direction, int confirm);
void updateWallNumber(void);
int checkVictory(void);
int chooseMoveNPC(int i, int j, int gameMatrixCopy[13][13], int first);

void prepareGame(){

	printGrill();
	printGameInfo();
	printTexts();
	
	colorSquare(105, 6, 0x001F); //P1
	colorSquare(105, 204, 0xF800); //P2
	
}

void startGame(){

	// inizializzazione P1
	xP1 = 6;
	yP1 = 12;
	wallsP1 = 8;
	
	// inizializzazione P2
	xP2 = 6;
	yP2 = 0;
	wallsP2 = 8;
	
	initGameMatrix();
	
	gameMode = 1;
	
	// inizializzazione e avvio timer per turno giocatore
	init_timer(0, 0x017D7840 ); 						// 1s
	//init_timer(0, 0x00050840 ); 						// 1s
	enable_timer(0);
	
	colorPossibleMoves(0xF790, 3);

}

void initGameMatrix(){
	
	int i, j;
	
	for(i=0; i<13; i++){
		for(j=0; j<13; j++){
				gameMatrix[i][j] = 0;
		}
	}
	
	gameMatrix[xP2][yP2] = 2;
	gameMatrix[xP1][yP1] = 1;
	
}

void switchTurn(int moveDone){
	
	if (moveDone == 1){
		int verticalHorizontal = 0;
		
		// setta lastMove
		// playerID = turn--; 8 bit
		// PlayerMove/WallPlacement = 0;
		// Vertical/Horizontal = 1;
		// Y position = yP1 / yP2 / yPosPlacingWall;
		// X position = xP1 / xP2 / xPosPlacingWall;
		
		// inizializzazione lastMove
		lastMove = 0;
		
		if (gameMode == 2 && wallDirection == 0){
			verticalHorizontal = 1;
		}
		
		// PlayerID
		lastMove = (lastMove & 0x00000000) | ((uint32_t)(turn-1) << 24);
		
		// Move type
		lastMove = (lastMove & 0xFF000000) | ((uint32_t)(gameMode-1) << 20);
		
		// Vertical/Horizontal
		lastMove = (lastMove & 0xFFF00000) | ((uint32_t)verticalHorizontal << 16);
		if (gameMode == 1) {
			printWallEnded(White);
			if (turn == 1){
				lastMove = (lastMove & 0xFFFF0000) | ((uint32_t)yP1 << 8);
				lastMove = (lastMove & 0xFFFFFF00) | (uint32_t)xP1;
			} else {
				lastMove = (lastMove & 0xFFFF0000) | ((uint32_t)yP2 << 8);
				lastMove = (lastMove & 0xFFFFFF00) | (uint32_t)xP2;
			}
		} else {
			// Inserimento posizione di partenza del muro
			lastMove = (lastMove & 0xFFFF0000) | ((uint32_t)yPosPlacingWall << 8);
			lastMove = (lastMove & 0xFFFFFF00) | (uint32_t)xPosPlacingWall;
		}
	}

	if(turn == 1){
		turn++;
	} else {
		turn--;
		}

	// Controllo vittoria di un player
		if (checkVictory() == 0){
		gameMode = 1;
		colorPossibleMoves(0xF790, 3);
		playerTime = 20;
	} else {
		disable_timer(0);
	}
	

}

void colorPossibleMoves(int color, int cellValue){

	int xPosPlayer;
	int yPosPlayer;
	//int wallsPlayer;
	int oppositePlayer;

	if (turn == 1){
		xPosPlayer = xP1;
		yPosPlayer = yP1;
		//wallsPlayer = wallsP1;
		oppositePlayer = 2; 
	} else {
		xPosPlayer = xP2;
		yPosPlayer = yP2;
		//wallsPlayer = wallsP2;
		oppositePlayer = 1; 
	}
	
	// calcolo possibilità di mossa in alto
	if (yPosPlayer-2 >= 0) {
		if (gameMatrix[xPosPlayer][yPosPlayer-1] == 0){
			if (gameMatrix[xPosPlayer][yPosPlayer-2] == oppositePlayer){
				if (yPosPlayer-4 >= 0){
					if (gameMatrix[xPosPlayer][yPosPlayer-3] == 0){
						gameMatrix[xPosPlayer][yPosPlayer-4] = cellValue;
						colorSquare(getAxisPosPixel(xPosPlayer), getAxisPosPixel(yPosPlayer-4), color);
					} else {
						// casella in alto a destra se non ci sono barriere
						if(gameMatrix[xPosPlayer+1][yPosPlayer-2] == 0 & xPosPlayer+2 <= 12){
							gameMatrix[xPosPlayer+2][yPosPlayer-2] = cellValue;
							colorSquare(getAxisPosPixel(xPosPlayer+2), getAxisPosPixel(yPosPlayer-2), color);
						}
						// casella in alto a sinistra se non ci sono barriere
						if(gameMatrix[xPosPlayer-1][yPosPlayer-2] == 0 & xPosPlayer-2 >= 0){
							gameMatrix[xPosPlayer-2][yPosPlayer-2] = cellValue;
							colorSquare(getAxisPosPixel(xPosPlayer-2), getAxisPosPixel(yPosPlayer-2), color);
						}
					}
				}
			} else {
				gameMatrix[xPosPlayer][yPosPlayer-2] = cellValue;
				colorSquare(getAxisPosPixel(xPosPlayer), getAxisPosPixel(yPosPlayer-2), color);
			}
		}
	}
	
	// calcolo possibilità di mossa in basso
	if (yPosPlayer+2 < 13) {
		if (gameMatrix[xPosPlayer][yPosPlayer+1] == 0){
			if (gameMatrix[xPosPlayer][yPosPlayer+2] == oppositePlayer){
				if (yPosPlayer+4 < 13){
					if (gameMatrix[xPosPlayer][yPosPlayer+3] ==  0){
						gameMatrix[xPosPlayer][yPosPlayer+4] = cellValue;
						colorSquare(getAxisPosPixel(xPosPlayer), getAxisPosPixel(yPosPlayer+4), color);
					} else {
						// casella in basso a destra se non ci sono barriere
						if(gameMatrix[xPosPlayer+1][yPosPlayer+2] == 0 & xPosPlayer+2 <= 12){
							gameMatrix[xPosPlayer+2][yPosPlayer+2] = cellValue;
							colorSquare(getAxisPosPixel(xPosPlayer+2), getAxisPosPixel(yPosPlayer+2), color);
						}
						// casella in basso a sinistra se non ci sono barriere
						if(gameMatrix[xPosPlayer-1][yPosPlayer+2] == 0 & xPosPlayer-2 >= 0){
							gameMatrix[xPosPlayer-2][yPosPlayer+2] = cellValue;
							colorSquare(getAxisPosPixel(xPosPlayer-2), getAxisPosPixel(yPosPlayer+2), color);
						}
					}
				}
			} else {
				gameMatrix[xPosPlayer][yPosPlayer+2] = cellValue;
				colorSquare(getAxisPosPixel(xPosPlayer), getAxisPosPixel(yPosPlayer+2), color);
			}
		}
	}

	
		// calcolo possibilità di mossa a sinistra
	if (xPosPlayer-2 >= 0) {
		if (gameMatrix[xPosPlayer-1][yPosPlayer] == 0){
			if (gameMatrix[xPosPlayer-2][yPosPlayer] == oppositePlayer){
				if (yPosPlayer-4 >= 0){
					if (gameMatrix[xPosPlayer-3][yPosPlayer] ==  0){
						gameMatrix[xPosPlayer-4][yPosPlayer] = cellValue;
						colorSquare(getAxisPosPixel(xPosPlayer-4), getAxisPosPixel(yPosPlayer), color);
					} else {
						// casella in basso a sinistra se non ci sono barriere
						if(gameMatrix[xPosPlayer-2][yPosPlayer+1] == 0 & yPosPlayer+2 <= 12){
							gameMatrix[xPosPlayer-2][yPosPlayer+2] = cellValue;
							colorSquare(getAxisPosPixel(xPosPlayer-2), getAxisPosPixel(yPosPlayer+2), color);
						}
						// casella in alto a sinistra se non ci sono barriere
						if(gameMatrix[xPosPlayer-2][yPosPlayer-1] == 0 & yPosPlayer-2 >= 0){
							gameMatrix[xPosPlayer-2][yPosPlayer-2] = cellValue;
							colorSquare(getAxisPosPixel(xPosPlayer-2), getAxisPosPixel(yPosPlayer-2), color);
						}
					}
				}
			} else {
				gameMatrix[xPosPlayer-2][yPosPlayer] = cellValue;
				colorSquare(getAxisPosPixel(xPosPlayer-2), getAxisPosPixel(yPosPlayer), color);
			}
		}
	}
	
		// calcolo possibilità di mossa a destra
	if (xPosPlayer+2 < 13) {
		if (gameMatrix[xPosPlayer+1][yPosPlayer] == 0){
			if (gameMatrix[xPosPlayer+2][yPosPlayer] == oppositePlayer){
				if (xPosPlayer+4 < 13){
					if (gameMatrix[xPosPlayer+3][yPosPlayer] ==  0){
						gameMatrix[xPosPlayer+4][yPosPlayer] = cellValue;
						colorSquare(getAxisPosPixel(xPosPlayer+4), getAxisPosPixel(yPosPlayer), color);
					} else {
						// casella in alto a destra se non ci sono barriere
						if(gameMatrix[xPosPlayer+2][yPosPlayer-1] == 0 & yPosPlayer-2 >= 0){
							gameMatrix[xPosPlayer+2][yPosPlayer-2] = cellValue;
							colorSquare(getAxisPosPixel(xPosPlayer+2), getAxisPosPixel(yPosPlayer-2), color);
						}
						// casella in basso a destra se non ci sono barriere
						if(gameMatrix[xPosPlayer+2][yPosPlayer+1] == 0 & yPosPlayer+2 <= 12){
							gameMatrix[xPosPlayer+2][yPosPlayer+2] = cellValue;
							colorSquare(getAxisPosPixel(xPosPlayer+2), getAxisPosPixel(yPosPlayer+2), color);
						}
					}
				}
			} else {
				gameMatrix[xPosPlayer+2][yPosPlayer] = cellValue;
				colorSquare(getAxisPosPixel(xPosPlayer+2), getAxisPosPixel(yPosPlayer), color);
			}
		}
	}
	
}

int getAxisPosPixel(int axisPos){
		
	return 6 + (axisPos/2)*30 + (axisPos/2)*3;
	
}

void skipTurn(){

	colorPossibleMoves(0xFFFF, 0);
	// setta lastMove come mossa nulla
	// playerID = turn--; 8 bit
	// PlayerMove/WallPlacement = 0;
	// Vertical/Horizontal = 1;
	// Y position = yP1 / yP2;
	// X position = xP1 / xP2;
	
	lastMove = 0;
	lastMove = (lastMove & 0x00000000) | ((uint32_t)(turn-1) << 24);
	lastMove = (lastMove & 0xFF000000) | ((uint32_t)0 << 20);
	lastMove = (lastMove & 0xFFF00000) | ((uint32_t)1 << 16);
	if (turn == 1){
		lastMove = (lastMove & 0xFFFF0000) | ((uint32_t)yP1 << 8);
		lastMove = (lastMove & 0xFFFFFF00) | (uint32_t)xP1;
	} else {
		lastMove = (lastMove & 0xFFFF0000) | ((uint32_t)yP2 << 8);
		lastMove = (lastMove & 0xFFFFFF00) | (uint32_t)xP2;
	}

}

int checkWallAvailable(){

	int wallAvailable = 1;
	
	if (turn == 1){
		if (wallsP1 < 1){
			wallAvailable = 0;
		}
	} else {
		if (wallsP2 < 1){
			wallAvailable = 0;
		}
	}
	
	return wallAvailable;
	
}

void moveWall(int direction){ // direction: 0 -> destra, 1 -> in basso, 2 -> sinistra, 3 -> in alto
	
	switch (direction){
	
		case 0: 
			
			if (xPosPlacingWall+2 < 12 & wallDirection == 0 || xPosPlacingWall+2 <= 12 & wallDirection == 1){
				printWall(xPosPlacingWall, yPosPlacingWall, White, wallDirection);
				xPosPlacingWall += 2;
				printWall(xPosPlacingWall, yPosPlacingWall, 0xF18F, wallDirection);
			}
			break;
		
		case 1:
			
			if (yPosPlacingWall+2 < 13 & wallDirection == 0 || yPosPlacingWall+2 < 11 & wallDirection == 1){
				printWall(xPosPlacingWall, yPosPlacingWall, White, wallDirection);
				yPosPlacingWall += 2;
				printWall(xPosPlacingWall, yPosPlacingWall, 0xF18F, wallDirection);
			}
			break;
		
		case 2: 
			
			if (xPosPlacingWall-2 > -1 & wallDirection == 0 || xPosPlacingWall-2 > 1 & wallDirection == 1){
				printWall(xPosPlacingWall, yPosPlacingWall, White, wallDirection);
				xPosPlacingWall -= 2;
				printWall(xPosPlacingWall, yPosPlacingWall, 0xF18F, wallDirection);
			}
			break;
		
		case 3:
			
			if (yPosPlacingWall-2 >= 1 & wallDirection == 0 || yPosPlacingWall-2 > -1 & wallDirection == 1){
				printWall(xPosPlacingWall, yPosPlacingWall, White, wallDirection);
				yPosPlacingWall -= 2;
				printWall(xPosPlacingWall, yPosPlacingWall, 0xF18F, wallDirection);
			}
			break;
	
	}

}

	void placeWall(){ // richiamato in gamemode = 2 con pressione del tasto select del joystick
	
		int wallPlaced = 0;
		
		if (wallDirection == 0){
			if (gameMatrix[xPosPlacingWall][yPosPlacingWall-1] == 0 & gameMatrix[xPosPlacingWall+1][yPosPlacingWall-1] == 0 & gameMatrix[xPosPlacingWall+2][yPosPlacingWall-1] == 0) {
			
			int gameMatrixCopy[13][13], i, j;
			
			for(i=0; i<13; i++){
				for(j=0; j<13; j++){
						gameMatrixCopy[i][j] = gameMatrix[i][j];
				}
			}
					
			gameMatrixCopy[xPosPlacingWall][yPosPlacingWall-1] = 1;
			gameMatrixCopy[xPosPlacingWall+1][yPosPlacingWall-1] = 1;
			gameMatrixCopy[xPosPlacingWall+2][yPosPlacingWall-1] = 1;

			// controllare se il muro blocca un giocatore e aggiornare la matrice			
 			if (checkAvailablePathToWin(xP2, yP2, 2, gameMatrixCopy) == 1){
				
			for(i=0; i<13; i++){
				for(j=0; j<13; j++){
						gameMatrixCopy[i][j] = gameMatrix[i][j];
				}
			}
					
			gameMatrixCopy[xPosPlacingWall][yPosPlacingWall-1] = 1;
			gameMatrixCopy[xPosPlacingWall+1][yPosPlacingWall-1] = 1;
			gameMatrixCopy[xPosPlacingWall+2][yPosPlacingWall-1] = 1;
				
				if (checkAvailablePathToWin(xP1, yP1, 1, gameMatrixCopy) == 1){
					gameMatrix[xPosPlacingWall][yPosPlacingWall-1] = 1;
					gameMatrix[xPosPlacingWall+1][yPosPlacingWall-1] = 1;
					gameMatrix[xPosPlacingWall+2][yPosPlacingWall-1] = 1;	
					wallPlaced = 1;
				}
			}
			
			}
		} else {
			if (gameMatrix[xPosPlacingWall-1][yPosPlacingWall] == 0 & gameMatrix[xPosPlacingWall-1][yPosPlacingWall+1] == 0 & gameMatrix[xPosPlacingWall-1][yPosPlacingWall+2] == 0) {
			
			int gameMatrixCopy[13][13], i, j;
			
			for(i=0; i<13; i++){
				for(j=0; j<13; j++){
						gameMatrixCopy[i][j] = gameMatrix[i][j];
				}
			}
			
			gameMatrixCopy[xPosPlacingWall-1][yPosPlacingWall] = 1;
			gameMatrixCopy[xPosPlacingWall-1][yPosPlacingWall+1] = 1;
			gameMatrixCopy[xPosPlacingWall-1][yPosPlacingWall+2] = 1;
			
			// controllare se il muro blocca un giocatore e aggiornare la matrice
			if (checkAvailablePathToWin(xP2, yP2, 2, gameMatrixCopy) == 1){
				
			for(i=0; i<13; i++){
				for(j=0; j<13; j++){
						gameMatrixCopy[i][j] = gameMatrix[i][j];
				}
			}
					
			gameMatrixCopy[xPosPlacingWall][yPosPlacingWall-1] = 1;
			gameMatrixCopy[xPosPlacingWall+1][yPosPlacingWall-1] = 1;
			gameMatrixCopy[xPosPlacingWall+2][yPosPlacingWall-1] = 1;
			
				if (checkAvailablePathToWin(xP1, yP1, 1, gameMatrixCopy) == 1){
					gameMatrix[xPosPlacingWall-1][yPosPlacingWall] = 1;
					gameMatrix[xPosPlacingWall-1][yPosPlacingWall+1] = 1;
					gameMatrix[xPosPlacingWall-1][yPosPlacingWall+2] = 1;
					wallPlaced = 1;
				}
			}
			}
		}
		if (wallPlaced == 1){
			updateWallNumber();
			gameMode = 1;
			switchTurn(1);
		}
		
	}
	
int movePlayer(int direction, int confirm){
		
		int xPos, yPos, playerColor;
		int moveAllowed = 0;
		
		if (turn == 1){
			xPos = xP1;
			yPos = yP1;
			playerColor = Red;
		} else{
			xPos = xP2;
			yPos = yP2;
			playerColor = Blue;
		}
	
		switch (direction){
			
			// a destra
			case 0:
				
				if (xPos <= 10){
					if (gameMatrix[xPos+2][yPos] == 3){
						// muovi a destra
						moveAllowed = 1;
						if (confirm == 1){
							colorPossibleMoves(0xFFFF, 0);
							colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);	
							colorSquare(getAxisPosPixel(xPos+2), getAxisPosPixel(yPos), playerColor);
							gameMatrix[xPos+2][yPos] = turn;
							gameMatrix[xPos][yPos] = 0;
							// aggiornamento posizione player
							setXPosPlayer(xPos+2);
							
							switchTurn(1);
						} else {
							colorPossibleMoves(0xF790, 3);
							colorSquare(getAxisPosPixel(xPos+2), getAxisPosPixel(yPos), Green);
						}

					} else if(xPos <= 8){
						if (gameMatrix[xPos+4][yPos] == 3){
							// muovi due celle a destra
							moveAllowed = 1;

							if (confirm == 1){
							colorPossibleMoves(0xFFFF, 0);
							colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);	
							colorSquare(getAxisPosPixel(xPos+4), getAxisPosPixel(yPos), playerColor);
							gameMatrix[xPos+4][yPos] = turn;
							gameMatrix[xPos][yPos] = 0;
							// aggiornamento posizione player
							setXPosPlayer(xPos+4);
							
							switchTurn(1);
							} else {
								colorPossibleMoves(0xF790, 3);
								colorSquare(getAxisPosPixel(xPos+4), getAxisPosPixel(yPos), Green);
							}
						
						}
					}
				}
				break;
			
			// in basso
			case 1:
			
				if(yPos <= 10) {
					if (gameMatrix[xPos][yPos+2] == 3){
					// muovi in basso
					moveAllowed = 1;

					if (confirm == 1){
					colorPossibleMoves(0xFFFF, 0);
					colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);
					colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos+2), playerColor);
					gameMatrix[xPos][yPos+2] = turn;
					gameMatrix[xPos][yPos] = 0;
					// aggiornamento posizione player
					setYPosPlayer(yPos+2);					

					switchTurn(1);
					} else {
						colorPossibleMoves(0xF790, 3);
						colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos+2), Green);
					}

				} else if (yPos <= 8){
					if (gameMatrix[xPos][yPos+4] == 3){
						// muovi due celle in basso
						moveAllowed = 1;
						
						if (confirm == 1){
							colorPossibleMoves(0xFFFF, 0);	
							colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);							
							colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos+4), playerColor);
							gameMatrix[xPos][yPos+4] = turn;
							gameMatrix[xPos][yPos] = 0;
							// aggiornamento posizione player
							setYPosPlayer(yPos+4);
							
							switchTurn(1);
							} else {
								colorPossibleMoves(0xF790, 3);
								colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos+4), Green);
							}
						}
					}
				}
				
				break;
			
			// a sinistra
			case 2:
				
				if (xPos >= 2){
					if (gameMatrix[xPos-2][yPos] == 3){
						// muovi a sinistra
						moveAllowed = 1;

						if (confirm == 1){
						colorPossibleMoves(0xFFFF, 0);
						colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);
						colorSquare(getAxisPosPixel(xPos-2), getAxisPosPixel(yPos), playerColor);
						gameMatrix[xPos-2][yPos] = turn;
						gameMatrix[xPos][yPos] = 0;
						// aggiornamento posizione player
						setXPosPlayer(xPos-2);
						
						switchTurn(1);
						} else {
							colorPossibleMoves(0xF790, 3);
							colorSquare(getAxisPosPixel(xPos-2), getAxisPosPixel(yPos), Green);
						}

					} else if (xPos >=4) {
						if (gameMatrix[xPos-4][yPos] == 3){
							// muovi due celle a sinistra
							moveAllowed = 1;
							
							if (confirm == 1){
							colorPossibleMoves(0xFFFF, 0);
							colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);
							colorSquare(getAxisPosPixel(xPos-4), getAxisPosPixel(yPos), playerColor);
							gameMatrix[xPos-4][yPos] = turn;
							gameMatrix[xPos][yPos] = 0;
							// aggiornamento posizione player
							setXPosPlayer(xPos-4);
							
							switchTurn(1);
							} else {
								colorPossibleMoves(0xF790, 3);
								colorSquare(getAxisPosPixel(xPos-4), getAxisPosPixel(yPos), Green);
							}
							
						}
					}
				}
				break;
				
			// in alto
			case 3:
				
				if (yPos >= 2) {
					if (gameMatrix[xPos][yPos-2] == 3){
						// muovi in alto
						moveAllowed = 1;
						
						if (confirm == 1){
						colorPossibleMoves(0xFFFF, 0);
						colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);
						colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos-2), playerColor);
						gameMatrix[xPos][yPos-2] = turn;
						gameMatrix[xPos][yPos] = 0;
						// aggiornamento posizione player
						setYPosPlayer(yPos-2);
							
						// controllo della vittoria
						
						switchTurn(1);
						} else {
							colorPossibleMoves(0xF790, 3);
							colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos-2), Green);
						}

					} else if (yPos >= 4) {
						if (gameMatrix[xPos][yPos-4] == 3){
							// muovi due celle in alto
							moveAllowed = 1;
							
							if (confirm == 1){
								colorPossibleMoves(0xFFFF, 0);
								colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);
								colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos-4), playerColor);
								gameMatrix[xPos][yPos-4] = turn;
								gameMatrix[xPos][yPos] = 0;
								// aggiornamento posizione player
								setYPosPlayer(yPos-4);
								
								switchTurn(1);
							} else{
								colorPossibleMoves(0xF790, 3);
								colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos-4), Green);
							}
						}
					}
				}
				break;
				
			// in alto a destra
			case 4:
				
				if (yPos >= 2 && xPos <= 10) {
					if (gameMatrix[xPos+2][yPos-2] == 3){
						// muovi in alto a destra
						moveAllowed = 1;
						
						if (confirm == 1){
						colorPossibleMoves(0xFFFF, 0);
						colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);
						colorSquare(getAxisPosPixel(xPos+2), getAxisPosPixel(yPos-2), playerColor);
						gameMatrix[xPos+2][yPos-2] = turn;
						gameMatrix[xPos][yPos] = 0;
						// aggiornamento posizione player
						setXPosPlayer(xPos+2);
						setYPosPlayer(yPos-2);
							
						switchTurn(1);
						} else {
							colorPossibleMoves(0xF790, 3);
							colorSquare(getAxisPosPixel(xPos+2), getAxisPosPixel(yPos-2), Green);
						}
					}
				}
				break;
				
			// in basso a destra
			case 5:
				
				if (yPos <= 10 & xPos <= 10) {
					if (gameMatrix[xPos+2][yPos+2] == 3){
						// muovi in basso a destra
						moveAllowed = 1;
						
						if (confirm == 1){
						colorPossibleMoves(0xFFFF, 0);
						colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);
						colorSquare(getAxisPosPixel(xPos+2), getAxisPosPixel(yPos+2), playerColor);
						gameMatrix[xPos+2][yPos+2] = turn;
						gameMatrix[xPos][yPos] = 0;
						// aggiornamento posizione player
						setXPosPlayer(xPos+2);
						setYPosPlayer(yPos+2);
								
						switchTurn(1);
						} else {
							colorPossibleMoves(0xF790, 3);
							colorSquare(getAxisPosPixel(xPos+2), getAxisPosPixel(yPos+2), Green);
						}
					}
				}
				break;
				
			// in basso a sinistra
			case 6:
				
				if (yPos <= 10 & xPos >= 2) {
					if (gameMatrix[xPos-2][yPos+2] == 3){
						// muovi in basso a sinistra
						moveAllowed = 1;
						
						if (confirm == 1){
						colorPossibleMoves(0xFFFF, 0);
						colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);
						colorSquare(getAxisPosPixel(xPos-2), getAxisPosPixel(yPos+2), playerColor);
						gameMatrix[xPos-2][yPos+2] = turn;
						gameMatrix[xPos][yPos] = 0;
						// aggiornamento posizione player
						setXPosPlayer(xPos-2);
						setYPosPlayer(yPos+2);
						
						switchTurn(1);
						} else {
							colorPossibleMoves(0xF790, 3);
							colorSquare(getAxisPosPixel(xPos-2), getAxisPosPixel(yPos+2), Green);
						}

					}
				}
				break;
				
			// in alto a sinistra
			case 7:
				
				if (yPos >= 2 & xPos >= 2) {
					if (gameMatrix[xPos-2][yPos-2] == 3){
						// muovi in alto a sinistra
						moveAllowed = 1;
						
						if (confirm == 1){
						colorPossibleMoves(0xFFFF, 0);
						colorSquare(getAxisPosPixel(xPos), getAxisPosPixel(yPos), White);
						colorSquare(getAxisPosPixel(xPos-2), getAxisPosPixel(yPos-2), playerColor);
						gameMatrix[xPos-2][yPos-2] = turn;
						gameMatrix[xPos][yPos] = 0;
						// aggiornamento posizione player
						setXPosPlayer(yPos-2);
						setYPosPlayer(yPos-2);
						
						switchTurn(1);
						} else {
							colorPossibleMoves(0xF790, 3);
							colorSquare(getAxisPosPixel(xPos-2), getAxisPosPixel(yPos-2), Green);
						}
					}
				}
				break;
			
		}
		
		return moveAllowed;
	
	}
	
	void setXPosPlayer(int xPos){
			
		if (turn == 1){
						xP1 = xPos;
					} else {
						xP2 = xPos;
					}
		
	}

	void setYPosPlayer(int yPos){
			
		if (turn == 1){
						yP1 = yPos;
					} else {
						yP2 = yPos;
					}
		
	}
	
  	int checkAvailablePathToWin(int i, int j, int player, int gameMatrixCopy[13][13]){
	
		// controllare che la matrice con i muri presenti consenta
		// ad entrambi i giocare di raggiungere la parte opposta della scacchiera
		int availablePath = 0;
		gameMatrixCopy[i][j] = 4;
		
		if (player == 1){
			if (j == 0){
				return 1;
			}
			if (availablePath == 0 && j>=2 && gameMatrixCopy[i][j-2]!= 4 && gameMatrixCopy[i][j-1] != 1){
				availablePath = checkAvailablePathToWin(i, j-2, player, gameMatrixCopy);
			}
			if (availablePath == 0 && i>=2 && gameMatrixCopy[i-2][j]!= 4 && gameMatrixCopy[i-1][j] != 1){
				availablePath = checkAvailablePathToWin(i-2, j, player, gameMatrixCopy);
			}
			if (availablePath == 0 && i<=10 && gameMatrixCopy[i+2][j]!= 4 && gameMatrixCopy[i+1][j] != 1){
				availablePath = checkAvailablePathToWin(i+2, j, player, gameMatrixCopy);
			}
			if (availablePath == 0 && j<=10 && gameMatrixCopy[i][j+2]!= 4 && gameMatrixCopy[i][j+1] != 1){
				availablePath = checkAvailablePathToWin(i, j+2, player, gameMatrixCopy);
			}
		} else {
			if (j == 12){
				return 1;
			}
			if (availablePath == 0 && j<=10 && gameMatrixCopy[i][j+2]!= 4 && gameMatrixCopy[i][j+1] != 1){
				availablePath = checkAvailablePathToWin(i, j+2, player, gameMatrixCopy);
			}
			if (availablePath == 0 && i>=2 && gameMatrixCopy[i-2][j]!= 4 && gameMatrixCopy[i-1][j] != 1){
				availablePath = checkAvailablePathToWin(i-2, j, player, gameMatrixCopy);
			}
			if (availablePath == 0 && i<=10 && gameMatrixCopy[i+2][j]!= 4 && gameMatrixCopy[i+1][j] != 1){
				availablePath = checkAvailablePathToWin(i+2, j, player, gameMatrixCopy);
			}
			if (availablePath == 0 && j>=2 && gameMatrixCopy[i][j-2]!= 4 && gameMatrixCopy[i][j-1] != 1){
				availablePath = checkAvailablePathToWin(i, j-2, player, gameMatrixCopy);
			}
		}
		
 	return availablePath;
		
	}
	
	int move(int direction, int confirm){ // direction: 0 -> destra, 1 -> in basso, 2 -> sinistra, 3 -> in alto 
																				// 4 -> alto-destra, 5 -> basso-destra, 6 -> basso sinistra, 7 -> alto-sinistra
		int moveAllowed = 0;
		
		if (gameMode == 1){
			// Spostamento giocatore
			moveAllowed = movePlayer(direction, confirm);
		} else if (gameMode == 2){
			// Spostamento muro
			moveWall(direction);
		}
		
	return moveAllowed;
		
	}
	
	void updateWallNumber(){
	
		char wallsNumberChar[2];		
		
		if (turn == 1){
			wallsP1--;
			sprintf(wallsNumberChar, "%d", wallsP1);
			printWallsNumberP1((uint8_t *) wallsNumberChar);
		} else {
			wallsP2--;
			sprintf(wallsNumberChar, "%d", wallsP2);
			printWallsNumberP2((uint8_t *) wallsNumberChar);
		}
		
	}
	
	int checkVictory(){
	
		int victory = 0;
		
		if (yP1 == 0){
			// P1 vince
			gameMode = 0;
			victory++;
			printPlayerWins(Red);
		} else if (yP2 == 12){
			// P2 vince
			gameMode = 0;
			victory++;
			printPlayerWins(Blue);
		}
		
		return victory;
		
	}
	
	int moveNPC(){
		
		int gameMatrixCopy[13][13], i, j;
			
		for(i=0; i<13; i++){
			for(j=0; j<13; j++){
					gameMatrixCopy[i][j] = gameMatrix[i][j];
			}
		}
		
		recursions1 = 0;
		recursions2 = 0;
		recursions3 = 0;
		recursions4 = 0;
		
		return chooseMoveNPC(xP2, yP2, gameMatrixCopy, 1);
	
	}

	int chooseMoveNPC(int i, int j, int gameMatrixCopy[13][13], int first){
	
		// controllare che la matrice con i muri presenti consenta
		// ad entrambi i giocare di raggiungere la parte opposta della scacchiera
		int availablePath = 0;
		gameMatrixCopy[i][j] = 4;
		
		recursions1++;
		recursions2++;
		recursions3++;
		recursions4++;
		
		if (j == 0){
			return 1;
		}
		
		if (availablePath == 0 && j<=10 && gameMatrixCopy[i][j+2]!= 4 && gameMatrixCopy[i][j+1] != 1){
			availablePath = chooseMoveNPC(i, j+2, gameMatrixCopy, 0);
			if (availablePath != 0 & first == 1){
				if (recursions1 <= recursions2 & recursions1 <= recursions3 & recursions1 <= recursions4){
					return 1;
				}
			}
		}
		
		if (availablePath == 0 && i>=2 && gameMatrixCopy[i-2][j]!= 4 && gameMatrixCopy[i-1][j] != 1){
			availablePath = chooseMoveNPC(i-2, j, gameMatrixCopy, 0);
			if (availablePath != 0 & first == 1){
				if (recursions2 <= recursions1 & recursions2 <= recursions3 & recursions2 <= recursions4){
					return 2;
				}
			}
		}
		
		if (availablePath == 0 && i<=10 && gameMatrixCopy[i+2][j]!= 4 && gameMatrixCopy[i+1][j] != 1){
			availablePath = chooseMoveNPC(i+2, j, gameMatrixCopy, 0);
			if (availablePath != 0 & first == 1){
				if (recursions3 <= recursions1 & recursions3 <= recursions2 & recursions3 <= recursions4){
					return 0;
				}
			}
		}

		if (availablePath == 0 && j>=2 && gameMatrixCopy[i][j-2]!= 4 && gameMatrixCopy[i][j-1] != 1){
			availablePath = chooseMoveNPC(i, j-2, gameMatrixCopy, 0);
			if (availablePath != 0 & first == 1){
				if (recursions4 <= recursions1 & recursions4 <= recursions2 & recursions4 <= recursions3){
					return 3;
				}
			}
		}
		
		return availablePath;
		
	}
	