/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../graphicInit.h"
#include "../gameLogic.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

int playerTime = 20;

void TIMER0_IRQHandler (void)
{
	playerTime -= 1;
	if (playerTime > 0) {
		printPlayerTime(playerTime);
	} else {
		//mossa nulla
		skipTurn();
		if (gameMode==2){
			//	Cancella muro
			printWall(xPosPlacingWall, yPosPlacingWall, White, wallDirection);
		}
		switchTurn(0);
	}

	if (gameMode != 0){
		reset_timer(0);
		enable_timer(0);
	} else {
		disable_timer(0);
	}

  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
