#include "button.h"
#include "lpc17xx.h"
#include "stdio.h"
#include "gameLogic.h"
#include "graphicInit.h"
#include "RIT.h"

int downKey0, downKey1, downKey2; // usare per il debouncing

void EINT0_IRQHandler (void)	  
{
	startGame();
  LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}

void EINT1_IRQHandler (void)	  
{
	
	downKey1 = 1;
	NVIC_DisableIRQ(EINT1_IRQn);							/* disable Button interrupts */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  
{
	
	downKey2 = 1;
	NVIC_DisableIRQ(EINT2_IRQn);							/* disable Button interrupts */
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
  
	LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */  

}
