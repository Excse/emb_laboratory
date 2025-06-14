/*----------------------------------------------------------------------------
 * Name:    can_rx_tx.c
 * Purpose: TX Testbench for CAN Module of LPC17xx Microcontrollers
 *            (Landtiger Boards are assumed)
 *----------------------------------------------------------------------------
 * History:
 *          V1.1 / 20.5.2019 (KBL, HS-Mannheim) Initial Version
 *----------------------------------------------------------------------------*/

#include "LPC17xx.h"
#include "emb1_can_labs.h"

int main(void) {     
    CAN_Init(1, 0x007FC003);
    CAN_Init(2, 0x007FC003);
    //CAN_Set_Acceptance_Filter_Mode(ACCF_OFF);
    CAN_Set_Acceptance_Filter_Mode(ACCF_BYPASS);
    //CAN_Set_Acceptance_Filter_Mode(ACCF_ON);    
    
    while(1) {}
}