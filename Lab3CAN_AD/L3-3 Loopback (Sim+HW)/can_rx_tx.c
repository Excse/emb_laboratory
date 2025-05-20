/*----------------------------------------------------------------------------
 * Name:    can_rx_tx.c
 * Purpose: TX Testbench for CAN Module of LPC23xx Microcontrollers
 *            (Keil MCB2300 Boards are assumed)
 * Version: V1.2 / 9.6.2016 (KBL/Poh, HS-Mannheim) Baud Rate 250kBit/s @12MHz
 *----------------------------------------------------------------------------
 * History:
 *          V1.1 / 13.6.2015 (KBL, HS-Mannheim) Initial Version
 *----------------------------------------------------------------------------*/

#include "LPC17xx.h"
#include "emb1_can_labs.h"


/* Example of CAN message / ID 0x101 and 8 bytes as payload */
CAN_MSG   message1 = {0x101, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}, 8, 0,0};

extern CAN_MSG   My_CAN1_RX_message;
extern CAN_MSG   My_CAN2_RX_message;

uint8_t  CAN1_New_RX_Data_Stored;  //Set to 1 if the content of a new RX CAN1 message is ready but not yet 
                              //shown on the LCD display, otherwise set to 0
uint8_t  CAN2_New_RX_Data_Stored;  //Set to 1 if the content of a new RX CAN1 message is ready but not yet 
                              //shown on the LCD display, otherwise set to 0

uint16_t Received_CAN1_ID, Received_CAN2_ID; 

extern uint8_t  CAN_Check_for_new_received_Data(uint8_t  CAN_x);

CAN_MSG   My_CAN_TX_message; //Message to be sent
const int My_CAN_ID = 0x701;

int main(void)
{
     int i;
  // Put your code here to perform the following tasks        
    
    CAN_Init(1, 0x007FC003);
    CAN_Init(2, 0x007FC003);
    //CAN_Set_Acceptance_Filter_Mode(ACCF_OFF);
    CAN_Set_Acceptance_Filter_Mode(ACCF_BYPASS);
    //CAN_Set_Acceptance_Filter_Mode(ACCF_ON);    
    
    My_CAN_TX_message.id = My_CAN_ID;
    My_CAN_TX_message.length = 5;
    My_CAN_TX_message.data[0] = 0xAA;
    My_CAN_TX_message.data[1] = 0xBB;
    My_CAN_TX_message.data[2] = 0xCC;
    My_CAN_TX_message.data[3] = 0xDD;
    My_CAN_TX_message.data[4] = 0x00;

    My_CAN_TX_message.frame_format  = 0;
    My_CAN_TX_message.frame_type  = 0;
    
    CAN1_New_RX_Data_Stored  = 0;
    CAN2_New_RX_Data_Stored  = 0;

    while(1){
        CAN_Send_Message(2, &My_CAN_TX_message);  //CAN frame has been sent  1=CAN1    2=CAN2
        for (i=0; i < 100000; i++);
                CAN1_New_RX_Data_Stored = CAN_Check_for_new_received_Data(1);    
        //CAN2_New_RX_Data_Stored = CAN_Check_for_new_received_Data(2);        

                if(CAN1_New_RX_Data_Stored){
                    Received_CAN1_ID = My_CAN1_RX_message.id;
                    My_CAN_TX_message.data[4] += 1; 
                }
        
                if(CAN2_New_RX_Data_Stored){
                    Received_CAN2_ID = My_CAN2_RX_message.id;
                    My_CAN_TX_message.data[4] += 5; 
                }

         
         }
         //The received data are in My_CANx_RX_message (x = 1 or 2)

    }
