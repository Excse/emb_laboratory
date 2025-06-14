/*----------------------------------------------------------------------------
* Name:    CAN_AD_3_4_main.c
* Purpose: main File for LPC1768 Microcontrollers        
* Version: Version 1.0 / 02.05.2019 (KBL, HS-Mannheim)
*----------------------------------------------------------------------------*/

#include "LPC17xx.h"        /* LPC17xx definitions */
#include "adc.h"
#include "GLCD.h"                            
#include "emb1_can_labs.h"
#include <stdio.h>

#define RELOAD_VALUE         6249999    // Reload value for 250ms (25MHz * 250ms = 6250000) 

void Timer1_Config(void);

// CAN received data are processed by ISRs and temporally stored receved in the list
// of RX messages CAN1_RX_messages[CAN1_MAX_RX_MSG], CAN2_RX_messages[CAN2_MAX_RX_MSG];
extern uint32_t CAN1_Counter_RX, CAN2_Counter_RX;
extern CAN_MSG  CAN1_RX_messages[CAN1_MAX_RX_MSG], CAN2_RX_messages[CAN2_MAX_RX_MSG];

// Put your data in My_CAN_TX_Message prior to a transmission over the CAN bus 
CAN_MSG My_CAN_TX_Message;

uint8_t CAN1_New_RX_Data_Stored; // Set to 1 if the content of a new RX CAN1 message is ready but not yet 
                                 // shown on the LCD display, otherwise set to 0
uint8_t CAN2_New_RX_Data_Stored; // Set to 1 if the content of a new RX CAN1 message is ready but not yet 
                                 // shown on the LCD display, otherwise set to 0

extern CAN_MSG My_CAN1_RX_message; // Contains valid data if  CAN1_New_RX_Data_Stored set to 1
extern CAN_MSG My_CAN2_RX_message; // Contains valid data if  CAN2_New_RX_Data_Stored set to 1

// All transmitted CAN telegrams shall exhibit this ID value.
const uint16_t My_CAN_ID = 0x70A;

// Variables related to the processing of ADC outputs
extern uint16_t AD_value;           // ADC output
extern uint8_t AD_New_Value_Ready;  // Set to 1 if AD_Value has been read but not yet transmitted over CAN
                                    // Otherwise set to 0
float AD_Voltage;      // AD_Value converted in a voltage between 0 V and 3.3 V
float Old_AD_Voltage;  // Last value of AD_Voltage
float Delta_Voltage;   // Difference AD_Voltage - Old_AD_Voltage
char AD_Val_String[7]; // Float value AD_Voltage converted to string

char NewLine[21];

uint16_t GLCD_Last_Shown_ID;
uint8_t X_CAN_GLCD, i;
char ID_String[6];

int main(void) {
    GLCD_Init();              /* Initialize the GLCD */
    GLCD_Clear(White);        /* Clear the GLCD */
    GLCD_SetBackColor(Blue);  /* Set the Back Color */
    GLCD_SetTextColor(White); /* Set the Text Color */

    GLCD_DisplayString(0, 0, "  EMB Lab3: CAN/ADC ");
    sprintf(NewLine, "   Local ID: %#3x  ", My_CAN_ID);
    GLCD_DisplayString(1, 0, (unsigned char *) NewLine);
    GLCD_DisplayString(2, 0, "Local ADValue:");

    GLCD_SetBackColor(White); /* Set the Back Color */
    GLCD_SetTextColor(Blue);  /* Set the Text Color */

    GLCD_DisplayString(4,0, "=Data Received@CAN1=");
    GLCD_DisplayString(5,0, "Remote ID:");
    GLCD_DisplayString(6,0, "Remote ADVal.:");
    GLCD_DisplayString(7,0, "@@@@@@@@@@@@@@@@@@@@");
    GLCD_DisplayString(8,0, "Remote ID:");
    GLCD_DisplayString(9,0, "Remote ADVal.:");

    Timer1_Config();
    ADC_Config();
    ADC_StartConversion();

    CAN_Init(1, 0x007FC003); // 1MBit/s = 0x007FC000 , 250KBit/s = 0x007FC003
    CAN_Init(2, 0x007FC003);
    // CAN_Set_Acceptance_Filter_Mode(ACCF_OFF);
    // CAN_Set_Acceptance_Filter_Mode(ACCF_BYPASS);
    CAN_Set_Acceptance_Filter_Mode(ACCF_ON);

    My_CAN_TX_Message.id = My_CAN_ID;
    My_CAN_TX_Message.length = 5;
    My_CAN_TX_Message.frame_format  = 0;
    My_CAN_TX_Message.frame_type = 0;

    X_CAN_GLCD = 0;
    My_CAN1_RX_message.id = 0;
    My_CAN2_RX_message.id = 0;
    AD_Val_String[5] = 0x56; // Letter V for Volt
    AD_Val_String[6] = 0x00; // Null terminator

    while(1) {
        if(AD_New_Value_Ready) {
            GLCD_SetBackColor(Blue);  /* Set the Back Color */
            GLCD_SetTextColor(White); /* Set the Text Color */
            
            /*************************************************/
            //Put some code lines here
            //  Goal: - Check if AD_Voltage has shown a change of more 0.5 V 
            //          If yes, convert AD_Voltage (as float value) to a string and
            //          send the string a CAN payload.
            //
            /************************************************/
            
            AD_Voltage = ((float) AD_value * 3.3f) / (float) 0xFFF;            
            Delta_Voltage = AD_Voltage - Old_AD_Voltage;
            Old_AD_Voltage = AD_Voltage;
            
            sprintf(NewLine, "Local ADValue:%#.3f", AD_Voltage);
            GLCD_DisplayString(2, 0, (unsigned char *) NewLine);    
            
            if(Delta_Voltage > 0.5) {
                sprintf(AD_Val_String, "%.3f", AD_Voltage);
                AD_Val_String[5] = 0x56; // Letter V for Volt
                AD_Val_String[6] = 0x00; // Null terminator
                My_CAN_TX_Message.data[0] = AD_Val_String[0];
                My_CAN_TX_Message.data[1] = AD_Val_String[1];
                My_CAN_TX_Message.data[2] = AD_Val_String[2];
                My_CAN_TX_Message.data[3] = AD_Val_String[3];
                My_CAN_TX_Message.data[4] = AD_Val_String[4];
                CAN_Send_Message(1, &My_CAN_TX_Message);
            }
        }    

        CAN1_New_RX_Data_Stored = CAN_Check_for_new_received_Data(1);    
        CAN2_New_RX_Data_Stored = CAN_Check_for_new_received_Data(2);                        

        GLCD_SetBackColor(White); /* Set the Back Color */
        GLCD_SetTextColor(Blue);  /* Set the Text Color */     

        if(CAN1_New_RX_Data_Stored) {
            // The variable allows us to use one line on the LCD display for one ID
            // Hence if the AD value is changing for one ID value, the same GLCD line
            // will be used. The other line is used only if a CAN message exhibiting another ID 
            // value has been received.                     
            if (My_CAN1_RX_message.id != GLCD_Last_Shown_ID) {
                X_CAN_GLCD = (X_CAN_GLCD + 1) % 2;
                GLCD_Last_Shown_ID = My_CAN1_RX_message.id;
            }

            /*************************************************/
            // Put some code lines here
            // Goal: - Convert the received ID value to a string called ID_String
            //       - Collect the received and convert them to the string AD_Val_String
            // Your task consists in preparing the values of the variables ID_String and 
            // AD_Val_string in order to facilitate the use of GLCD output functions. 
            /************************************************/

            sprintf(ID_String, "%#3x", My_CAN1_RX_message.id);
            AD_Val_String[0] = My_CAN1_RX_message.data[0];
            AD_Val_String[1] = My_CAN1_RX_message.data[1];
            AD_Val_String[2] = My_CAN1_RX_message.data[2];
            AD_Val_String[3] = My_CAN1_RX_message.data[3];
            AD_Val_String[4] = My_CAN1_RX_message.data[4];
            AD_Val_String[5] = 0x56; // Letter V for Volt
            AD_Val_String[6] = 0x00; // Null terminator
            
            if (X_CAN_GLCD == 0)
                i = 8; //Select GLCD lines 8 and 9 for output
            else 
                i = 5; //Select GLCD lines 5 and 6 for output

            sprintf(NewLine, "Remote ID:%s", ID_String);
            GLCD_DisplayString(i, 0, (unsigned char *) NewLine);
            
            sprintf(NewLine, "Remote ADVal.:%s", AD_Val_String);
            GLCD_DisplayString(i+1, 0, (unsigned char *) NewLine);
            
            CAN1_New_RX_Data_Stored = 0;
        }

        if(CAN2_New_RX_Data_Stored) {
            // The variable allows us to use one line on the LCD display for one ID
            // Hence if the AD value is changing for one ID value, the same GLCD line
            // will be used. The other line is used only if a CAN message exhibiting another ID 
            // value has been received.                     
            if (My_CAN2_RX_message.id != GLCD_Last_Shown_ID) {
                X_CAN_GLCD = (X_CAN_GLCD + 1) % 2;
                GLCD_Last_Shown_ID  = My_CAN2_RX_message.id;
            }
            
            /*************************************************/
            // Put some code lines here
            // Goal: - Convert the received ID value to a string called ID_String
            //       - Collect the received and convert them to the string AD_Val_String
            // Your task consists in preparing the values of the variables ID_String and 
            // AD_Val_string in order to facilitate the use of GLCD output functions. 
            /************************************************/

            sprintf(ID_String, "%#3x", My_CAN2_RX_message.id);
            AD_Val_String[0] = My_CAN2_RX_message.data[0];
            AD_Val_String[1] = My_CAN2_RX_message.data[1];
            AD_Val_String[2] = My_CAN2_RX_message.data[2];
            AD_Val_String[3] = My_CAN2_RX_message.data[3];
            AD_Val_String[4] = My_CAN2_RX_message.data[4];
            AD_Val_String[5] = 0x56; // Letter V for Volt
            AD_Val_String[6] = 0x00; // Null terminator
            
            if ( X_CAN_GLCD == 0)
                i = 8; //Select GLCD lines 8 and 9 for output
            else 
                i = 5; //Select GLCD lines 5 and 6 for output

            sprintf(NewLine, "Remote ID:%s", ID_String);
            GLCD_DisplayString(i, 0, (unsigned char *) NewLine);
            
            sprintf(NewLine, "Remote ADVal.:%s", AD_Val_String);
            GLCD_DisplayString(i+1, 0, (unsigned char *) NewLine);
            
            CAN2_New_RX_Data_Stored = 0;
        }
    }
}

void Timer1_Config(void) {
    LPC_SC->PCONP |= (1 << 2); // Enable Timer1

    // Put some code in order to enable Timer1 interrupt with priority level 1    
    NVIC_SetPriority(TIMER1_IRQn, 1);
    NVIC_EnableIRQ(TIMER1_IRQn);

    // and let Timer1 generate an Interrupt every 10ms
    LPC_TIM1->MR0 = RELOAD_VALUE;        // Zeit[s] * 25MHz = Cycles
    LPC_TIM1->MCR = (1 << 0) | (1 << 1); // MR0I + MR0R
    LPC_TIM1->TCR = 1;                   // Timer starten
}

void TIMER1_IRQHandler(void) {  
    // Start new AD conversion
    ADC_StartConversion();
    
    // Clear timer1 interrupt
    LPC_TIM1->IR = (1 << 0);
}

/************************************ EOF ***********************************/