/*----------------------------------------------------------------------------
 * Name:    can_api.c
 * Purpose: CAN Module API File for LPC1768 Microcontrollers        
 * Version: Version 1.0 / 02.05.2019 (KBL, HS-Mannheim)
 *----------------------------------------------------------------------------*/

#include "LPC17xx.h"                          
#include "emb1_can_labs.h"
#include "CAN_AcceptanceFiltering.h"
#include <stdio.h>


/*********************************************************************************
        Declaration of CAN-related global variables
**********************************************************************************/
#define CAN1_MAX_RX_MSG 20
#define CAN2_MAX_RX_MSG 20

LPC_CAN_TypeDef     *CAN_Regs_ptr;  /* Pointer for CANx (1 or 2) Registers */
uint32_t  CAN_Init_Done;
uint32_t  CAN_Rx_Done;
uint32_t  CAN_Tx_Done;
uint32_t  CAN_LUT_RAM_layout_done;
uint32_t  CAN1_ErrCount, CAN2_ErrCount;
CAN_MSG  Received_Message;
CAN_MSG  CAN1_RX_messages[CAN1_MAX_RX_MSG], CAN2_RX_messages[CAN2_MAX_RX_MSG];
uint32_t CAN1_Counter_RX = 0;
uint32_t CAN2_Counter_RX = 0;

CAN_MSG   My_CAN1_RX_message; //Contains valid data if  CAN1_New_RX_Data_Stored set to 1
CAN_MSG   My_CAN2_RX_message; //Contains valid data if  CAN2_New_RX_Data_Stored set to 1



/*********************************************************************************
        Function prototypes
**********************************************************************************/
uint32_t CAN_Init( uint32_t CAN_Module_Number, uint32_t CAN_Baud_Rate );
uint32_t CAN_Receive_Message( uint32_t CAN_Module_Number, CAN_MSG  *Dest_Rx_Data );
uint32_t CAN_Send_Message( uint32_t CAN_Module_Number, CAN_MSG  *Tx_Data );
void CAN_Set_Acceptance_Filter_Mode(uint32_t  ACF_Mode);
uint32_t CAN_Set_Acceptance_LUT_RAM(uint32_t  ACF_Mode);
uint8_t  CAN_Check_for_new_received_Data(uint8_t  CAN_x);


/******************************************************************************
** Function name:        CAN_Init
**
** Descriptions:        Initialize selected CAN Module
**
** parameters:            CAN_Module_Number (1 or 2 for LPC2368) , CAN_Baud_Rate
** Returned value:        true or false, false if initialization failed.
** 
******************************************************************************/
uint32_t CAN_Init( uint32_t CAN_Module_Number, uint32_t CAN_Baud_Rate )
{
  CAN_Init_Done = False;
  
  if (( CAN_Module_Number == 1) || ( CAN_Module_Number == 2))
   {
     /* CAN-related Configuration of the Interrupt Controller VIC  */
        NVIC_EnableIRQ(CAN_IRQn);                    /* Enable CAN interrupt */
        NVIC_SetPriority(CAN_IRQn,2);

    if ( CAN_Module_Number == 1)
      {
        CAN_Regs_ptr = LPC_CAN1;                         
        LPC_SC->PCONP   |= 1 << 13;             /* Enable clock for LPC_CAN1 */
        LPC_PINCON->PINSEL0 &= 0xFFFFFFF0;          /* Reset LPC_CAN1-related Pin Selection bits */
        LPC_PINCON->PINSEL0 |= 0x00000005;          /* Set LPC_CAN1-related Pin Selection bits to b0101 */
      }
    else
      {
            
        CAN_Regs_ptr = LPC_CAN2;                         
        LPC_SC->PCONP   |= 1 << 14;             /* Enable clock for LPC_CAN2 */
        LPC_PINCON->PINSEL0 &= 0xFFFFF0FF;          /* Reset LPC_CAN2-related Pin Selection bits */
        LPC_PINCON->PINSEL0 |= 0x00000A00;          /* Set LPC_CAN2-related Pin Selection bits to b1010 */
      }    

    CAN_Regs_ptr->MOD |= 1;  // Reset CAN Module
    CAN_Regs_ptr->IER = 0;  // Disable Receive Interrupts
    CAN_Regs_ptr->GSR = 0;  // Reset error counter when CANxMOD is in reset
    CAN_Regs_ptr->BTR = CAN_Baud_Rate; 
    //CAN_Regs_ptr->MOD |= 4;  // Self-test mode
    CAN_Regs_ptr->MOD &= 0xFFFFFFFE;  // Recover normal mode of operation
    CAN_Regs_ptr->IER = 1;  // Enable Receive Interrupts
    CAN_Init_Done = True;
   }
   return CAN_Init_Done;
 }
      
        
/******************************************************************************
** Function name:        CAN_Receive_Message
**
** Descriptions:        This function reads data from CAN RX Buffer and stores
**                      them at the given destination Dest_Rx_Data
**
** parameters:            CAN_Module_Number, Rx_Data_Dest (pointer to the destination
**                       address of the received CAN message) 
** Returned value:        True or False
** 
******************************************************************************/
uint32_t CAN_Receive_Message( uint32_t CAN_Module_Number, CAN_MSG  *Dest_Rx_Data )
{
  uint32_t  i, tmp_data;
  CAN_Rx_Done = False;
  
  if (( CAN_Module_Number == 1) || ( CAN_Module_Number == 2))
   {    
    if ( CAN_Module_Number == 1)
      {
        CAN_Regs_ptr = LPC_CAN1;                         
      }
    else
      {
        CAN_Regs_ptr = LPC_CAN2;                         
      }    
    /* Read frame information: Frame Format, Frame Type, Frame Length */
    Dest_Rx_Data->frame_format = (uint8_t)(((CAN_Regs_ptr->RFS) & 0x80000000) >> 31);
    Dest_Rx_Data->frame_type = (uint8_t)(((CAN_Regs_ptr->RFS) & 0x40000000) >> 30);    
    Dest_Rx_Data->length = (uint8_t)(((CAN_Regs_ptr->RFS) & 0x000F0000) >> 16);
    /* Read received Frame Message ID  */
    if (Dest_Rx_Data->frame_format)
        Dest_Rx_Data->id = ((CAN_Regs_ptr->RID) & 0x1FFFFFFF);
    else
        Dest_Rx_Data->id = ((CAN_Regs_ptr->RID) & 0x000007FF);
    /* Read data only if received message was a Data Frame */
    for (i = 0; i < 8; i++)
             Dest_Rx_Data->data[i] = 0;
    if (!(Dest_Rx_Data->frame_type))  // RTR=1 -->  Request Frame, RTR=0 -> Data Frame 
    {
       tmp_data =  CAN_Regs_ptr->RDA;
       /* Pop only the number of messages corresponding to the DATA LENGTH of message */
       if ((Dest_Rx_Data->length) > 0)
             *((uint8_t *) &Dest_Rx_Data->data[0]) = tmp_data & 0x000000FF;       
      if ((Dest_Rx_Data->length) > 1)
             *((uint8_t *) &Dest_Rx_Data->data[1]) = (tmp_data & 0x0000FF00)>>8;       
      if ((Dest_Rx_Data->length) > 2)
             *((uint8_t *) &Dest_Rx_Data->data[2]) = (tmp_data & 0x00FF0000)>>16;       
      if ((Dest_Rx_Data->length) > 3)
             *((uint8_t *) &Dest_Rx_Data->data[3]) = (tmp_data & 0xFF000000)>>24;       
       tmp_data =  CAN_Regs_ptr->RDB;
       if ((Dest_Rx_Data->length) > 4)
             *((uint8_t *) &Dest_Rx_Data->data[4]) = tmp_data & 0x000000FF;       
      if ((Dest_Rx_Data->length) > 5)
             *((uint8_t *) &Dest_Rx_Data->data[5]) = (tmp_data & 0x0000FF00)>>8;       
      if ((Dest_Rx_Data->length) > 6)
             *((uint8_t *) &Dest_Rx_Data->data[6]) = (tmp_data & 0x00FF0000)>>16;       
      if ((Dest_Rx_Data->length) > 7)
             *((uint8_t *) &Dest_Rx_Data->data[7]) = (tmp_data & 0xFF000000)>>24;
    }             
    /* Release receive buffer */
    CAN_Regs_ptr->CMR = (1 << 2);
    CAN_Rx_Done = True;
   }
  return  CAN_Rx_Done;
}  

   
 /******************************************************************************
** Function name:        CAN_Send_Message
**
** Descriptions:        Send a CAN message if a message buffer is released    
**
** parameters:            pointer to the CAN message
** Returned value:        true or false, if message buffer is available,
**                        message can be sent successfully, return TRUE,
**                        otherwise, return FALSE.
** 
******************************************************************************/
uint32_t CAN_Send_Message( uint32_t CAN_Module_Number, CAN_MSG  *Tx_Data )
{
  uint32_t CAN_Tx_BufferStatus;
  uint32_t  tmp_data;
    
  CAN_Tx_Done = False;
  
  if (( CAN_Module_Number == 1) || ( CAN_Module_Number == 2))
   {    
    if ( CAN_Module_Number == 1)
      {
        CAN_Regs_ptr = LPC_CAN1;                         
      }
    else
      {
        CAN_Regs_ptr = LPC_CAN2;                         
      }    
    
    CAN_Tx_BufferStatus = CAN_Regs_ptr->SR;
    if ( CAN_Tx_BufferStatus & (1 << 2) )
    {
      /* If TX Buffer 1 is available, write the message in its CANxTFI,
         CANxTID, CANxTDA, and CANxTDB registers. */
      CAN_Regs_ptr->TFI1 &=  0xFFF0FFFF;
      CAN_Regs_ptr->TFI1  |=  ((Tx_Data->length) << 16 );
      if (Tx_Data->frame_type)
            CAN_Regs_ptr->TFI1  |=  0x40000000;
      else
            CAN_Regs_ptr->TFI1  &= 0xBFFFFFFF;
      if (Tx_Data->frame_format)
            CAN_Regs_ptr->TFI1  |=  0x80000000;
      else
            CAN_Regs_ptr->TFI1  &= 0x7FFFFFFF;
          
      CAN_Regs_ptr->TID1 = Tx_Data->id; 
      // Write the first 4 bytes
      tmp_data  = ( Tx_Data->data[0]) | ((Tx_Data->data[1]) << 8) |
                             ((Tx_Data->data[2]) << 16) | ((Tx_Data->data[3]) << 24);
      CAN_Regs_ptr->TDA1 = tmp_data; 
      // Write the last 4 bytes
      tmp_data  = ( Tx_Data->data[4]) | ((Tx_Data->data[5]) << 8) |
                             ((Tx_Data->data[6]) << 16) | ((Tx_Data->data[7]) << 24);
      CAN_Regs_ptr->TDB1 = tmp_data;
      //Transmission Request
     CAN_Regs_ptr->CMR = 0x21;   // Transmit over Tx1 in normal mode of operation
     //CAN_Regs_ptr->CMR = 0x23;   // Transmit over Tx1 only once in normal mode of operation
     //CAN_Regs_ptr->CMR = 0x30;  //Self-Reception enabled    in normal mode /Global Self-Test
                                  //Local Self-Test if STM Bit in CANxMOD is set  
     //CAN_Regs_ptr->CMR = 0x32;  //Global Self-Test with only one transmission
                                  //Local Self-Test if STM Bit in CANxMOD is set       
      CAN_Tx_Done = True;
    }
    else if (CAN_Tx_BufferStatus & (1 << 10) )
    {
      /* If TX Buffer 2 is available, write the message in its CANxTFI,
         CANxTID, CANxTDA, and CANxTDB registers. */
      CAN_Regs_ptr->TFI2 &=  0xFFF0FFFF;
      CAN_Regs_ptr->TFI2  |=  ((Tx_Data->length) << 16 );
      if (Tx_Data->frame_type)
            CAN_Regs_ptr->TFI2  |=  0x40000000;
      else
            CAN_Regs_ptr->TFI2  &= 0xBFFFFFFF;
      if (Tx_Data->frame_format)
            CAN_Regs_ptr->TFI2  |=  0x80000000;
      else
            CAN_Regs_ptr->TFI2  &=  0x7FFFFFFF;
          
      CAN_Regs_ptr->TID2 = Tx_Data->id; 
      // Write the first 4 bytes
      tmp_data  = ( Tx_Data->data[0]) | ((Tx_Data->data[1]) << 8) |
                             ((Tx_Data->data[2]) << 16) | ((Tx_Data->data[3]) << 24);
      CAN_Regs_ptr->TDA2 = tmp_data; 
      // Write the last 4 bytes
      tmp_data  = ( Tx_Data->data[4]) | ((Tx_Data->data[5]) << 8) |
                             ((Tx_Data->data[6]) << 16) | ((Tx_Data->data[7]) << 24);
      CAN_Regs_ptr->TDB2 = tmp_data;
      //Transmission Request
      CAN_Regs_ptr->CMR = 0x41;   // Transmit over Tx2 in normal mode of operation
     //CAN_Regs_ptr->CMR = 0x43;   // Transmit over Tx2 only once in normal mode of operation
     //CAN_Regs_ptr->CMR = 0x50;  //Self-Reception enabled    in normal mode /Global Self-Test
                                  //Local Self-Test if STM Bit in CANxMOD is set  
     //CAN_Regs_ptr->CMR = 0x52;  //Global Self-Test with only transmission
                                  //Local Self-Test if STM Bit in CANxMOD is set         
      CAN_Tx_Done = True;
    }    
     else if (CAN_Tx_BufferStatus & (1 << 18) )
    {
      /* If TX Buffer 2 is available, write the message in its CANxTFI,
         CANxTID, CANxTDA, and CANxTDB registers. */
      CAN_Regs_ptr->TFI3 &=  0xFFF0FFFF;
      CAN_Regs_ptr->TFI3  |=  ((Tx_Data->length) << 16 );
      if (Tx_Data->frame_type)
            CAN_Regs_ptr->TFI3  |=  0x40000000;
      else
            CAN_Regs_ptr->TFI3  &= 0xBFFFFFFF;
      if (Tx_Data->frame_format)
            CAN_Regs_ptr->TFI3  |=  0x80000000;
      else
            CAN_Regs_ptr->TFI3  &=  0x7FFFFFFF;
          
      CAN_Regs_ptr->TID3 = Tx_Data->id; 
      // Write the first 4 bytes
      tmp_data  = ( Tx_Data->data[0]) | ((Tx_Data->data[1]) << 8) |
                             ((Tx_Data->data[2]) << 16) | ((Tx_Data->data[3]) << 24);
      CAN_Regs_ptr->TDA3 = tmp_data; 
      // Write the last 4 bytes
      tmp_data  = ( Tx_Data->data[4]) | ((Tx_Data->data[5]) << 8) |
                             ((Tx_Data->data[6]) << 16) | ((Tx_Data->data[7]) << 24);
      CAN_Regs_ptr->TDB3 = tmp_data;
      //Transmission Request
      CAN_Regs_ptr->CMR = 0x81;   // Transmit over Tx3 in normal mode of operation
     //CAN_Regs_ptr->CMR = 0x83;   // Transmit over Tx3 only once in normal mode of operation
     //CAN_Regs_ptr->CMR = 0x90;  //Self-Reception enabled    in normal mode /Global Self-Test
                                  //Local Self-Test if STM Bit in CANxMOD is set  
      //CAN_Regs_ptr->CMR = 0x92;  //Global Self-Test with only transmission
                                  //Local Self-Test if STM Bit in CANxMOD is set       
      
      CAN_Tx_Done = True;
    }
  }
  return  CAN_Tx_Done;
}   
                          
/*****************************************************************************
** Function name:        CAN_Interrupt Handler
**
** Descriptions:        CAN interrupt handler currently used for CAN Data
**                       reception
**
** parameters:            None
** Returned value:        None
** 
*****************************************************************************/
void CAN_IRQHandler(void) 
{    
   volatile uint32_t temp;    
    
  //IENABLE;            /* Nested interrupt allowed */
    
  
  if  ((LPC_CANCR->CANRxSR) & (1 << 8))
         CAN_Receive_Message(1, &CAN1_RX_messages[CAN1_Counter_RX++]);
    
  if  ((LPC_CANCR->CANRxSR) & (1 << 9))
         CAN_Receive_Message(2, &CAN2_RX_messages[CAN2_Counter_RX++]);
           
  if ( (LPC_CAN1->GSR) & (1 << 6 ) )
  {
    CAN1_ErrCount = ((LPC_CAN1->GSR) >> 16 );
  }         
  if ( (LPC_CAN2->GSR) & (1 << 6 ) )
  {
    CAN2_ErrCount = ((LPC_CAN2->GSR) >> 16 );
  }
 
    temp = LPC_CAN1->ICR;
    temp = LPC_CAN2->ICR;      
}  
  
/******************************************************************************
** Function name:        CAN_Set_Acceptance_Filter_Mode
**
** Descriptions:        Set acceptance filter mode and associated LUT RAM if 
**                      necessary    
**
** parameters:            ACF_Mode (Acceptance Filter Mode)
** Returned value:        None
**
** 
******************************************************************************/
void CAN_Set_Acceptance_Filter_Mode(uint32_t  ACF_Mode)
{
  if (( ACF_Mode == ACCF_OFF) || ( ACF_Mode == ACCF_BYPASS))
  {
     LPC_CANAF->AFMR = ACF_Mode;
     if ( ACF_Mode == ACCF_OFF)
       {
               LPC_CAN1->MOD = LPC_CAN2->MOD = 1;    // Reset CAN
              LPC_CAN1->GSR = LPC_CAN2->GSR = 0;    // Reset error counter while CANxMOD is in reset
              LPC_CAN1->IER = LPC_CAN2->IER = 0;    // Disable Receive Interrupt for both CAN Modules
       }
   }
  else if  (( ACF_Mode == ACCF_ON) || ( ACF_Mode == ACCF_FULLCAN))
   {
     /* Acceptance LUT RAM must be configured */  
     LPC_CANAF->AFMR = ACCF_OFF; //LPC_CANAF->AFMR = ACCF_BYPASS;
     CAN_Set_Acceptance_LUT_RAM(ACF_Mode);
     LPC_CANAF->AFMR = ACF_Mode;
   }
}

   
/******************************************************************************
** Function name:        CAN_Set_Acceptance_LUT_RAM
**
** Descriptions:        Set acceptance LUT RAM     
**
** parameters:            None
** Returned value:        None
**
** 
******************************************************************************/
uint32_t CAN_Set_Acceptance_LUT_RAM(uint32_t  ACF_Mode)
{
  uint32_t i, jj, Entry_low, Entry_high, Entry;
  
  CAN_LUT_RAM_layout_done = False;
 
  for ( i=0; i < 512; i++)
   {
     LPC_CANAF_RAM->mask[i] = 0;
   }
   
   Entry = ((CAN1_Number_FullCAN_ID+1) >>1) + ((CAN2_Number_FullCAN_ID+1) >>1) +
            ((CAN1_Number_SFF_EXP_ID+1) >>1) + ((CAN2_Number_SFF_EXP_ID+1) >>1) +
              CAN1_Number_GRP_SFF_ID + CAN2_Number_GRP_SFF_ID +
                + CAN1_Number_EXP_EXT_ID + CAN2_Number_EXP_EXT_ID  +
                  + 2*CAN1_Number_GRP_EXT_ID + 2*CAN2_Number_GRP_EXT_ID +
                    CAN1_Number_FullCAN_ID *3  + CAN2_Number_FullCAN_ID *3;
                  
  if ( Entry  <= 512 ) // Maxiumum size of LUT RAM = 2 KB =  512 32-Bit words
  {
    if (ACF_Mode == ACCF_FULLCAN)
              LPC_CANAF->SFF_sa = 4*(((CAN1_Number_FullCAN_ID+1)>>1) + ((CAN2_Number_FullCAN_ID+1)>>1));
        else
              LPC_CANAF->SFF_sa = 0;
    LPC_CANAF->SFF_GRP_sa = LPC_CANAF->SFF_sa + 4*(((CAN1_Number_SFF_EXP_ID+1)>>1) + ((CAN2_Number_SFF_EXP_ID+1)>>1));
    LPC_CANAF->EFF_sa = LPC_CANAF->SFF_GRP_sa + 4*(CAN1_Number_GRP_SFF_ID + CAN2_Number_GRP_SFF_ID); 
    LPC_CANAF->EFF_GRP_sa = LPC_CANAF->EFF_sa + 4*(CAN1_Number_EXP_EXT_ID + CAN2_Number_EXP_EXT_ID);    
    LPC_CANAF->ENDofTable = LPC_CANAF->EFF_GRP_sa + 8*(CAN1_Number_GRP_EXT_ID + CAN2_Number_GRP_EXT_ID);
    
    /*************************************************************************
         Enter FullCAN IDs for LPC_CAN1 and LPC_CAN2 Controllers
    **************************************************************************/
    i = 0;
    if ((CAN1_Number_FullCAN_ID) && (ACF_Mode == ACCF_FULLCAN)) //Entering FullCAN IDs for LPC_CAN1
    {
       while ( i < CAN1_Number_FullCAN_ID )
       {
        Entry_high = (0x07FF & (uint32_t) CAN1_Array_FullCAN_IDs[i]) | 0x0800;
        Entry_high = Entry_high  << 16; 
        if (i == (CAN1_Number_FullCAN_ID - 1))
            Entry = Entry_high | (1 << 12);
        else    
        { 
          Entry_low = (0x07FF & (uint32_t) CAN1_Array_FullCAN_IDs[i+1]) | 0x0800;
          Entry = Entry_high |  Entry_low ;
        }
        jj = i>>1;
        LPC_CANAF_RAM->mask[jj] = Entry;
        i += 2;
       }    
    }
    
    if ((CAN2_Number_FullCAN_ID) && (ACF_Mode == ACCF_FULLCAN))   //Entering FullCAN IDs for LPC_CAN2
    {
      i = 0;
      while ( i  <  CAN2_Number_FullCAN_ID )
      {      
        Entry_high = (0x07FF & (uint32_t) CAN2_Array_FullCAN_IDs[i]) | 0x2800;
        Entry_high = Entry_high  << 16; 
        if (i == (CAN2_Number_FullCAN_ID - 1))
            Entry = Entry_high;
        else    
        { 
          Entry_low = (0x07FF & (uint32_t) CAN2_Array_FullCAN_IDs[i+1]) | 0x2800;
          Entry = Entry_high |  Entry_low ;
        }
        jj = (i>>1) + ((CAN1_Number_FullCAN_ID + 1)>>1) ;
        LPC_CANAF_RAM->mask[jj] = Entry;
        i += 2;
      }    
    }

    /*************************************************************************
         Enter SFF (11-Bit) Explicit IDs for LPC_CAN1 and LPC_CAN2 Controllers
    **************************************************************************/    
    i = 0;
    if (CAN1_Number_SFF_EXP_ID)  //Entering Explicit SFF IDs for LPC_CAN1
    {
       while ( i < CAN1_Number_SFF_EXP_ID )
       {
        Entry_high = (0x07FF & (uint32_t) CAN1_Array_SFF_EXP_IDs[i]) ;
        Entry_high = Entry_high  << 16; 
        if (i == (CAN1_Number_SFF_EXP_ID - 1))
            Entry = Entry_high;
        else    
        { 
          Entry_low = (0x07FF & (uint32_t) CAN1_Array_SFF_EXP_IDs[i+1]) ;
          Entry = Entry_high |  Entry_low ;
        }
        jj = ((LPC_CANAF->SFF_sa)>>2) + (i>>1);
        LPC_CANAF_RAM->mask[jj] = Entry;
        i += 2;
       }    
    }
    
    if (CAN2_Number_SFF_EXP_ID)  //Entering Explicit SFF IDs for LPC_CAN2
    {
      i = 0;
      while ( i  < CAN2_Number_SFF_EXP_ID)
      {      
        Entry_high = (0x07FF & (uint32_t) CAN2_Array_SFF_EXP_IDs[i]) | 0x2000;
        Entry_high = Entry_high  << 16; 
        if (i == (CAN2_Number_SFF_EXP_ID - 1))
            Entry = Entry_high;
        else    
        { 
          Entry_low = (0x07FF & (uint32_t) CAN2_Array_SFF_EXP_IDs[i+1]) | 0x2000;
          Entry = Entry_high |  Entry_low ;
        }
        jj = ((LPC_CANAF->SFF_sa)>>2) + (i>>1) + ((CAN1_Number_SFF_EXP_ID + 1)>>1) ;
        LPC_CANAF_RAM->mask[jj] = Entry;
        i += 2;
      }    
    }
    
    /*****************************************************************************
         Enter Groups of SFF (11-Bit) Explicit IDs for LPC_CAN1 and LPC_CAN2 Controllers
    *****************************************************************************/    
    i = 0;  
    if (CAN1_Number_GRP_SFF_ID)
    {
       while ( i < CAN1_Number_GRP_SFF_ID )
       {
        Entry_high = (0x07FF & (uint32_t) CAN1_Array_GRP_SFF_IDs[i][0]) ;
        Entry_high = Entry_high  << 16;  //Lower bound
        Entry_low = (0x07FF & (uint32_t) CAN1_Array_GRP_SFF_IDs[i][1]) ; //Higher bound
        Entry = Entry_high |  Entry_low ;
        jj = ((LPC_CANAF->SFF_GRP_sa)>>2)  + i;
        LPC_CANAF_RAM->mask[jj] = Entry;
        i++;
       }    
    }

    i = 0;
    if (CAN2_Number_GRP_SFF_ID)
    {
       while ( i <  CAN2_Number_GRP_SFF_ID)
       {
        Entry_high = (0x07FF & (uint32_t) CAN2_Array_GRP_SFF_IDs[i][0]) | 0x2000;
        Entry_high = Entry_high  << 16;  //Lower bound
        Entry_low = (0x07FF & (uint32_t) CAN2_Array_GRP_SFF_IDs[i][1]) | 0x2000; //Higher bound
        Entry = Entry_high |  Entry_low ;
        jj = ((LPC_CANAF->SFF_GRP_sa)>>2) + i + CAN1_Number_GRP_SFF_ID;
        LPC_CANAF_RAM->mask[jj] = Entry;
        i++;
       }    
    }
    
    /*****************************************************************************
         Enter EFF (29-Bit) Explicit IDs for LPC_CAN1 and LPC_CAN2 Controllers
    *****************************************************************************/    
    i = 0;  
    if (CAN1_Number_EXP_EXT_ID)
    {
       while ( i < CAN1_Number_EXP_EXT_ID )
       {
        Entry = (0x1FFFFFFF & CAN1_Array_EXP_EXT_IDs[i]) ;
        jj = i + ((LPC_CANAF->EFF_sa)>>2); 
        LPC_CANAF_RAM->mask[jj] = Entry;
        i++;
       }    
    }
    
    i = 0;
    if (CAN2_Number_EXP_EXT_ID)
    {
       while ( i < CAN2_Number_EXP_EXT_ID )
       {
        Entry = (0x1FFFFFFF & CAN2_Array_EXP_EXT_IDs[i]) | 0x20000000;
        jj = i + CAN1_Number_EXP_EXT_ID + ((LPC_CANAF->EFF_sa)>>2); 
        LPC_CANAF_RAM->mask[jj] = Entry;
        i++;
       }    
    }
    
    /*****************************************************************************
         Enter Groups of EFF (29-Bit) Explicit IDs for LPC_CAN1 and LPC_CAN2 Controllers
    *****************************************************************************/    
    i = 0; 
    if (CAN1_Number_GRP_EXT_ID)
    {
       while ( i < CAN1_Number_GRP_EXT_ID )
       {
        Entry = (0x1FFFFFFF & CAN1_Array_GRP_EXT_IDs[i][0]) ;
        jj = 2*i + ((LPC_CANAF->EFF_GRP_sa)>>2);
        LPC_CANAF_RAM->mask[jj] = Entry;
        Entry = (0x1FFFFFFF & CAN1_Array_GRP_EXT_IDs[i][1]) ;
        jj += 1;
        LPC_CANAF_RAM->mask[jj] = Entry;        
        i++ ;
       }    
    }

    i = 0;
    if (CAN2_Number_GRP_EXT_ID)
    {
       while ( i <  CAN2_Number_GRP_EXT_ID)
       {
        Entry = (0x1FFFFFFF & CAN2_Array_GRP_EXT_IDs[i][0]) | 0x20000000;
        jj = 2*i + 2*CAN1_Number_GRP_EXT_ID + ((LPC_CANAF->EFF_GRP_sa)>>2);
        LPC_CANAF_RAM->mask[jj] = Entry;    
        Entry = (0x1FFFFFFF & CAN2_Array_GRP_EXT_IDs[i][1]) | 0x20000000;
        jj += 1;
        LPC_CANAF_RAM->mask[jj] = Entry;                
        i++ ;
       }    
    }    

  CAN_LUT_RAM_layout_done = True;
  }
  return CAN_LUT_RAM_layout_done;
 }
    
/****************************************************************************
 ** Function name:        CAN_Check_for_new_received_Data
**
** Descriptions:        Set acceptance filter mode and associated LUT RAM if 
**                      necessary    
**
** parameters:            ACF_Mode (Acceptance Filter Mode)
** Returned value:        True or False
**
** 
******************************************************************************/
 uint8_t  CAN_Check_for_new_received_Data(uint8_t  CAN_x){
    int  New_CAN_RX_Data_Stored =0;
    if (CAN_x == 1){
                 if ( CAN1_Counter_RX > 0){
                 //Read the last received message
               My_CAN1_RX_message.id = CAN1_RX_messages[CAN1_Counter_RX-1].id;
                 My_CAN1_RX_message.length = CAN1_RX_messages[CAN1_Counter_RX-1].length;
                 My_CAN1_RX_message.frame_type = CAN1_RX_messages[CAN1_Counter_RX-1].frame_type;
                 My_CAN1_RX_message.data[0] = CAN1_RX_messages[CAN1_Counter_RX-1].data[0];
                 My_CAN1_RX_message.data[1] = CAN1_RX_messages[CAN1_Counter_RX-1].data[1];
                 My_CAN1_RX_message.data[2] = CAN1_RX_messages[CAN1_Counter_RX-1].data[2];
                 My_CAN1_RX_message.data[3] = CAN1_RX_messages[CAN1_Counter_RX-1].data[3];
                 My_CAN1_RX_message.data[4] = CAN1_RX_messages[CAN1_Counter_RX-1].data[4];
                 My_CAN1_RX_message.data[5] = CAN1_RX_messages[CAN1_Counter_RX-1].data[5];
                 My_CAN1_RX_message.data[6] = CAN1_RX_messages[CAN1_Counter_RX-1].data[6];
                 My_CAN1_RX_message.data[7] = CAN1_RX_messages[CAN1_Counter_RX-1].data[7];
               New_CAN_RX_Data_Stored= 1;
                 CAN1_Counter_RX = 0;
                }
            }        
    else
        if (CAN_x == 2){                     
                if ( CAN2_Counter_RX > 0){
                 //Read the last received message
               My_CAN2_RX_message.id = CAN2_RX_messages[CAN2_Counter_RX-1].id;
                 My_CAN2_RX_message.length = CAN2_RX_messages[CAN2_Counter_RX-1].length;
                 My_CAN2_RX_message.frame_type = CAN2_RX_messages[CAN2_Counter_RX-1].frame_type;
                 My_CAN2_RX_message.data[0] = CAN2_RX_messages[CAN2_Counter_RX-1].data[0];
                 My_CAN2_RX_message.data[1] = CAN2_RX_messages[CAN2_Counter_RX-1].data[1];
                 My_CAN2_RX_message.data[2] = CAN2_RX_messages[CAN2_Counter_RX-1].data[2];
                 My_CAN2_RX_message.data[3] = CAN2_RX_messages[CAN2_Counter_RX-1].data[3];
                 My_CAN2_RX_message.data[4] = CAN2_RX_messages[CAN2_Counter_RX-1].data[4];
                 My_CAN2_RX_message.data[5] = CAN2_RX_messages[CAN2_Counter_RX-1].data[5];
                 My_CAN2_RX_message.data[6] = CAN2_RX_messages[CAN2_Counter_RX-1].data[6];
                 My_CAN2_RX_message.data[7] = CAN2_RX_messages[CAN2_Counter_RX-1].data[7];
               New_CAN_RX_Data_Stored = 1;
                 CAN2_Counter_RX = 0;
                    }    
                }
    return New_CAN_RX_Data_Stored;    
                
}
