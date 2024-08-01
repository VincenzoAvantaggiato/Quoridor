#include "graphicInit.h"
#include "stdio.h"
#include "gameLogic.h"

void printMenuRectangleWithText(int xPosPixel, int yPosPixel, char text[20], int xTextOffset, int yTextOffset);
void printMenuRectangle(int xPosPixel, int yPosPixel, int color);

void printGrill(){
		
		int xPosStart = 6;	// Lato quadrato 30px, spazio di 3px, bordi di 6px
		int yPosStart = 6;
		const int numeroQuadrati = 7;
		int i, j;
	
		for (j=0; j<numeroQuadrati; j++){
			for (i=0; i<numeroQuadrati; i++){
				printSquare(xPosStart, yPosStart);
				xPosStart += 33; // 33 è la distanza tra l'inizio di un lato di un quadrato e l'inizio del successivo
			}
			xPosStart = 6;
			yPosStart += 33;
		}
	
}

void printSquare(uint16_t xPosStart, uint16_t yPosStart){

	LCD_DrawLine(xPosStart, yPosStart, xPosStart+30, yPosStart, Black);
	LCD_DrawLine(xPosStart, yPosStart, xPosStart, yPosStart+30, Black);
	LCD_DrawLine(xPosStart+30, yPosStart, xPosStart+30, yPosStart+30, Black);
	LCD_DrawLine(xPosStart, yPosStart+30, xPosStart+30, yPosStart+30, Black);
	
}

void printGameInfo(){
	
	int xPosStart = 6;
	int yPosStart = 260;
	const int numeroRettangoli = 3;
	int i;
	
	for (i=0; i<numeroRettangoli; i++){
			LCD_DrawLine(xPosStart, yPosStart, xPosStart+72, yPosStart, Black);
			LCD_DrawLine(xPosStart, yPosStart, xPosStart, yPosStart+50, Black);
			LCD_DrawLine(xPosStart+72, yPosStart, xPosStart+72, yPosStart+50, Black);
			LCD_DrawLine(xPosStart, yPosStart+50, xPosStart+72, yPosStart+50, Black);
			xPosStart += 78; // 75 è la distanza tra l'inizio di un lato di un rettangolo e l'inizio del successivo
		}
		
}

void printTexts(){
	
	int xPosStart = 11;
	int yPosStart = 266;
	
	GUI_Text(xPosStart, yPosStart, (uint8_t *) "P1 Walls", Black, White);
	GUI_Text(xPosStart+92, yPosStart, (uint8_t *) "Time", Black, White);
	GUI_Text(xPosStart+156, yPosStart, (uint8_t *) "P2 Walls", Black, White);
	printWallsNumberP1("8");
	GUI_Text(112, 290, (uint8_t *) "20", Black, White);
	printWallsNumberP2("8");
	
}

void printWallsNumberP1(uint8_t * wallsNumber){
	
	int xPos = 39;
	int yPos = 290;

	GUI_Text(xPos, yPos, (uint8_t *) wallsNumber, Black, White);
	
}

void printWallsNumberP2(uint8_t * wallsNumber){
	
	int xPos = 194;
	int yPos = 290;

	GUI_Text(xPos, yPos, (uint8_t *) wallsNumber, Black, White);
	
}

void colorSquare(int xPos, int yPos, int color){

	int i;
	int linesNumber = 26;
	xPos = xPos + 2;
	yPos = yPos + 2;
	
	for(i = 0; i<=linesNumber; i++){
		LCD_DrawLine(xPos, yPos, xPos+linesNumber, yPos, color);
		yPos += 1;
		}

}

void printPlayerTime(int playerTime){
	
	int xPosTimeValue = 112;
	int yPosTimeValue = 290;
	char playerTimeChar[3];
	
	sprintf(playerTimeChar, "%2d", playerTime);
	GUI_Text(xPosTimeValue, yPosTimeValue, (uint8_t *) playerTimeChar, Black, White);

}

void printWall(int xPos, int yPos, int color, int direction){
	
	int xPosPixel = getAxisPosPixel(xPos);
	int yPosPixel = getAxisPosPixel(yPos)-3;
	
	if (color != White) {
		if (direction == 0) {
			LCD_DrawLine(xPosPixel, yPosPixel+1, xPosPixel+63, yPosPixel+1, color);
			LCD_DrawLine(xPosPixel, yPosPixel+2, xPosPixel+63, yPosPixel+2, color);
		} else if (direction == 1){
			LCD_DrawLine(xPosPixel-2, yPosPixel+3, xPosPixel-2, yPosPixel+66, color);
			LCD_DrawLine(xPosPixel-1, yPosPixel+3, xPosPixel-1, yPosPixel+66, color);
		}
	} else {
		if (direction == 0) {
			// controllo in orizzontale
			if (gameMatrix[xPos][yPos-1] == 0){
				// cancello la linea sopra quella posizione e lo spazio di distanza dalla casella vicina
				LCD_DrawLine(xPosPixel, yPosPixel+1, xPosPixel+33, yPosPixel+1, color);
				LCD_DrawLine(xPosPixel, yPosPixel+2, xPosPixel+33, yPosPixel+2, color);
			}
			if (gameMatrix[xPos+2][yPos-1] == 0){
				// cancello la linea sopra quella posizione e lo spazio di distanza dalla casella vicina
				LCD_DrawLine(xPosPixel+30, yPosPixel+1, xPosPixel+63, yPosPixel+1, color);
				LCD_DrawLine(xPosPixel+30, yPosPixel+2, xPosPixel+63, yPosPixel+2, color);
			}
			if (gameMatrix[xPos+1][yPos-2] == 1){
				// disegna il trattino intermedio di una barriera verticale, se intersecata
				LCD_DrawLine(xPosPixel+30, yPosPixel+1, xPosPixel+33, yPosPixel+1, 0xF18F);
				LCD_DrawLine(xPosPixel+30, yPosPixel+2, xPosPixel+33, yPosPixel+2, 0xF18F);
			}
		} else if (direction == 1){
			// controllo in verticale
			if (gameMatrix[xPos-1][yPos] == 0){
				// cancello la linea a sinistra di quella posizione e lo spazio di distanza dalla casella vicina
				LCD_DrawLine(xPosPixel-2, yPosPixel+3, xPosPixel-2, yPosPixel+36, color);
				LCD_DrawLine(xPosPixel-1, yPosPixel+3, xPosPixel-1, yPosPixel+36, color);
			}
			if (gameMatrix[xPos-1][yPos+2] == 0){
				// cancello la linea a sinistra di quella posizione e lo spazio di distanza dalla casella vicina
				LCD_DrawLine(xPosPixel-2, yPosPixel+33, xPosPixel-2, yPosPixel+66, color);
				LCD_DrawLine(xPosPixel-1, yPosPixel+33, xPosPixel-1, yPosPixel+66, color);
			}
			if (gameMatrix[xPos-2][yPos+1] == 1 & gameMatrix[xPos][yPos+1] == 1){
				// disegna il trattino intermedio di una barriera orizzontale, se intersecata
				LCD_DrawLine(xPosPixel-2, yPosPixel+34, xPosPixel-1, yPosPixel+34, 0xF18F);
				LCD_DrawLine(xPosPixel-2, yPosPixel+35, xPosPixel-1, yPosPixel+35, 0xF18F);
			}
		}
	}

}

void printWallEnded(int color){

	int xPosPixel = 45;
	int yPosPixel = 240;
	
	GUI_Text(xPosPixel, yPosPixel, (uint8_t *) "No walls available!", color, White);

}

void printPlayerWins(int playerColor){

	int xPosPixel = 90;
	int yPosPixel = 240;
	
	GUI_Text(xPosPixel, yPosPixel, (uint8_t *) "Victory!", playerColor, White);

}

// PARTE 2 - MENU'

void printMenu1(){ // Stampa il primo menù per selezione numero board da usare

	int xPosPixel = 78;
	int yPosPixel = 50;
	
	GUI_Text(xPosPixel, yPosPixel, (uint8_t *) "Select the", Black, White);
	
	xPosPixel += 5;
	yPosPixel += 20;
	
	GUI_Text(xPosPixel, yPosPixel, (uint8_t *) "GAME MODE", Black, White);
	
	xPosPixel = 50;
	yPosPixel = 143;
	
	printMenuRectangleWithText(xPosPixel, yPosPixel, "Single board", 22, 20);
	printMenuRectangleWithText(xPosPixel, yPosPixel+80, "Two boards", 32, 20);
	
}

// int xOffsetText1 aggiungere ai parametri
void printMenu2(char text1[20], char text2[20]){ // Stampa il menù per selezionare se giocare cno umano o NPC in modalità single board
	
	int xPosPixel = 40;
	int yPosPixel = 50;
	
	GUI_Text(xPosPixel, yPosPixel, (uint8_t *) text1, Black, White);
	
	xPosPixel += 35;
	yPosPixel += 20;
	
	GUI_Text(xPosPixel, yPosPixel, (uint8_t *) text2, Black, White);

	xPosPixel = 50;
	yPosPixel = 143;
	
	printMenuRectangleWithText(xPosPixel, yPosPixel, "Human", 52, 20);
	printMenuRectangleWithText(xPosPixel, yPosPixel+80, "NPC", 58, 20);

}

void selectMenuButton(int selection){

	int selectionColor = Blue;
	
	if (selection == 1){
		printMenuRectangle(50, 143, selectionColor);
		printMenuRectangle(50, 223, Black);
	} else if (selection == 2){
		printMenuRectangle(50, 223, selectionColor);
		printMenuRectangle(50, 143, Black);
	}
}

void confirmMenu1Choice(int selection){

	numBoards = selection;
	LCD_Clear(White);
	if (selection == 1){
		printMenu2("Single board: select", "the opponent");
	} else {
		// MP mode
		printMenu2("Two boards: select", "your player");
	}
	
}

void confirmMenu2Choice(int selection){

	human_NPC = selection;
	LCD_Clear(White);
	prepareGame();
	
}

void printMenuRectangleWithText(int xPosPixel, int yPosPixel, char text[20], int xTextOffset, int yTextOffset){
	
	LCD_DrawLine(xPosPixel, yPosPixel, xPosPixel+140, yPosPixel, Black);	
	LCD_DrawLine(xPosPixel, yPosPixel, xPosPixel, yPosPixel+50, Black);
	LCD_DrawLine(xPosPixel, yPosPixel+50, xPosPixel+140, yPosPixel+50, Black);
	LCD_DrawLine(xPosPixel+140, yPosPixel, xPosPixel+140, yPosPixel+50, Black);
	
	GUI_Text(xPosPixel+xTextOffset, yPosPixel+yTextOffset, (uint8_t *) text, Black, White);

}

void printMenuRectangle(int xPosPixel, int yPosPixel, int color){
	
	LCD_DrawLine(xPosPixel, yPosPixel, xPosPixel+140, yPosPixel, color);	
	LCD_DrawLine(xPosPixel, yPosPixel, xPosPixel, yPosPixel+50, color);
	LCD_DrawLine(xPosPixel, yPosPixel+50, xPosPixel+140, yPosPixel+50, color);
	LCD_DrawLine(xPosPixel+140, yPosPixel, xPosPixel+140, yPosPixel+50, color);
	
}
