/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "gameLogic.h"
#include "graphicInit.h"

volatile int mossa = -1;
volatile int sceltaMenu = -1; // 1 -> bottone in alto, 2 -> bottone in basso

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/	

void RIT_IRQHandler (void)
{			
	static int js_up=0;
	static int js_down=0;
	static int js_right=0;
	static int js_left=0;
	static int js_up_right=0;
	static int js_up_left=0;
	static int js_down_right=0;
	static int js_down_left=0;
	static int select=0;
	
	if (gameMode == 1 & turn == 2 & human_NPC == 2){
		int mossaNPC = moveNPC();
		
		if (move(mossaNPC, 0) == 1){
			move(mossaNPC, 1);
		}
	}

	if (downKey1 != 0){
		//controllo il PIN 10->INT0, 11->KEY1, 12->KEY2
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
			switch(downKey1){
				case 1:
						
					if (gameMode == 1){
					// controllare se il giocatore ha muri disponibili
						if (checkWallAvailable() == 1){
							// il giocatore entra in wall mode
							gameMode++;
							xPosPlacingWall = 6;
							yPosPlacingWall = 6;
							wallDirection = 0;
							colorPossibleMoves(White, 0);
							printWall(xPosPlacingWall, yPosPlacingWall, 0xF18F, wallDirection);	
						}	else {
							// mostra messaggio di muri terminati
							printWallEnded(Red);
						}
					} else if (gameMode == 2){
						// il giocatore torna in game mode
						gameMode--;
						// cancella il muro da inserire
						printWall(xPosPlacingWall, yPosPlacingWall, White, wallDirection);	
						colorPossibleMoves(0xF790, 3);
					}
				
					break;
				default:
					break;
			}
			downKey1++;
		} else {	/* button released */
			downKey1 = 0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
		if(downKey2!=0){  
			//controllo il piedino fisico 10->INT0, 11->KEY1, 12->KEY2
			if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){ 
				switch(downKey2){
				case 1:
					//codice da eseguire in caso di pressione del button
						if (gameMode==2) {
						printWall(xPosPlacingWall, yPosPlacingWall, 0xFFFF, wallDirection);	
							if (wallDirection == 0) {
								xPosPlacingWall += 2;
								yPosPlacingWall -= 2;
								wallDirection++;
							} else {
								xPosPlacingWall -= 2;
								yPosPlacingWall += 2;
								wallDirection--;
							}
						printWall(xPosPlacingWall, yPosPlacingWall, 0xF18F, wallDirection);	
						}
				
					break;				
				default:
					//se voglio gestire il button on X click
					//if(down_2 % X == 0){
					//}					
					break;
			}
			downKey2++;  
		} else {	/* button released */
			downKey2=0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
		}
		
	// JOYSTICK 
		
	if (gameMode != 0){
		if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<28)) == 0){ // up-right
			/* Joytick UP-RIGHT pressed */
			js_up_right++;
			
			switch(js_up_right){
				case 1:
				// Spostamento in alto a destra
				if (gameMode == 1){
					if (move(4, 0) == 1){
						mossa = 4;
					}
				}
				break;
								
				default:
				break;
			}
			
		} else {
			js_up_right = 0;
			if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) == 0){ // up-left
				/* Joytick UP-LEFT pressed */
				js_up_left++;
				
				switch(js_up_left){
					case 1:
					// Spostamento in alto a sinistra
					if (gameMode == 1){
						if (move(7, 0) == 1){
							mossa = 7;
						}
					}
					break;
								
					default:
					break;
				}
			
			} else {
				js_up_left = 0;
				if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<28)) == 0){ // down-right
					/* Joytick DOWN-RIGHT pressed */
					js_down_right++;
					
					switch(js_down_right){
						case 1:
						// Spostamento in basso a destra
						if (gameMode == 1){
							if (move(5, 0) == 1){
								mossa = 5;
							}
						}
						break;
									
						default:
						break;
					}
					
				} else {
					js_down_right = 0;
					if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) == 0){ // down-left
						/* Joytick DOWN-LEFT pressed */
						js_down_left++;
						
						switch(js_down_left){
							case 1:
							// Spostamento in basso a sinistra
							if (gameMode == 1){
								if (move(6, 0) == 1){
									mossa = 6;
								}
							}
							break;
										
							default:
							break;
						}
						
					} else { // JOYSTICK base (4 direzioni)
						js_down_left = 0;
						if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	 // 26 down 27 left 28 right 29 up
							/* Joytick UP pressed */
							js_up++;
							switch(js_up){
								case 1:
									// Spostamento in alto
									if (gameMode == 1){
										if (move(3, 0)  == 1){
											mossa = 3;
										}
									} else if (gameMode == 2){
										move(3, 0);
									}
									break;
								
								default:
									break;
							}
						}	else {
								js_up=0;
						}
						
							if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){
							/* Joytick right pressed */
							js_right++;
							switch(js_right){
								case 1:
									// Spostamento a destra
									if (gameMode == 1){
										if (move(0, 0) == 1){
											mossa = 0;
										}
									} else if (gameMode == 2){
										move(0, 0);
									}
									break;
								
								default:
									break;
							}
						} else {
								js_right=0;
						}
						
								if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){
							/* Joytick left pressed */
							js_left++;
							switch(js_left){
								case 1:
									// Spostamento a sinistra
									if (gameMode == 1){
										if (move(2, 0) == 1){
											mossa = 2;
										}
									} else if (gameMode == 2){
										move(2, 0);
									}
									break;

								default:
									break;
							}
						} else {
								js_left=0;
						}
						
							if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){
							/* Joytick down pressed */
							js_down++;
							switch(js_down){
								case 1:
									// Spostamento in basso
									if (gameMode == 1){
										if (move(1, 0) == 1){
											mossa = 1;
										}
									} else if (gameMode == 2){
										move(1, 0);
									}
									break;

								default:
									break;
							}
						} else {
								js_down=0;
						}
								
						if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){								//il joystick (il voltaggio è basso [==0] quando il tasto è premuto
							/* Joytick select pressed */
							select++;
							switch(select){										//lo switch su up per fare un polling di un tot di tempo
								case 1:
									// conferma mossa
									if (gameMode == 1){
										if (mossa != -1){
											move(mossa,1);
											mossa = -1;
										}
									} else if (gameMode == 2){
										placeWall();
									}
									select = 0;
									break;

								default:
									break;
							}
						} else {
								select = 0;
						}
					}
				}
			}
		}
	} else if (gameMode == 0){
		if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	 // 26 down 27 left 28 right 29 up
			/* Joytick UP pressed */
			js_up++;
			switch(js_up){
				case 1:
					// Selezione menù in alto
					sceltaMenu = 1;
					selectMenuButton(sceltaMenu);
					break;
								
					default:
						break;
				}
			}	else {
					js_up=0;
			}
			
			if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	 // 26 down 27 left 28 right 29 up
			/* Joytick DOWN pressed */
			js_down++;
			switch(js_down){
				case 1:
					// Selezione menù in basso
					sceltaMenu = 2;
					selectMenuButton(sceltaMenu);
					break;
								
					default:
						break;
				}
			}	else {
					js_down=0;
			}
			
			if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){								//il joystick (il voltaggio è basso [==0] quando il tasto è premuto
				/* Joytick select pressed */
				select++;
				switch(select){										//lo switch su up per fare un polling di un tot di tempo
					case 1:
						// conferma scelta menù
						if (sceltaMenu != -1){
							if (numBoards == 0){
								confirmMenu1Choice(sceltaMenu);
							} else{
								confirmMenu2Choice(sceltaMenu);
							}
							select = 0;
							sceltaMenu = -1;
						}
						break;

					default:
						break;
				}
			} else {
					select = 0;
			}
	}
		
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
