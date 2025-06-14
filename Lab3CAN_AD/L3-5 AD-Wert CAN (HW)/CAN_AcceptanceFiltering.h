
#include <stdint.h>   

/*********************************************************************************
		CAN-related Acceptance Filtering
**********************************************************************************/
/*  Settings related to CAN1 Controller  */
const uint32_t  CAN1_Number_FullCAN_ID = 2;
const uint16_t  CAN1_Array_FullCAN_IDs[CAN1_Number_FullCAN_ID] = {0x01, 05};

/**********  Änderbare Stellen im Rahmen des EMB Laborversuchs  CAN/AD ********************/
const uint32_t  CAN1_Number_SFF_EXP_ID = 2;
const uint16_t  CAN1_Array_SFF_EXP_IDs[CAN1_Number_SFF_EXP_ID] = {0x700, 0x0705};
/******************************************************************************/

const uint32_t  CAN1_Number_GRP_SFF_ID =2;
const uint16_t  CAN1_Array_GRP_SFF_IDs[CAN1_Number_GRP_SFF_ID][2] = {{0x200, 0x2FF},{0x300,0x3FF}};
const uint32_t  CAN1_Number_EXP_EXT_ID = 2;
const uint32_t  CAN1_Array_EXP_EXT_IDs[CAN1_Number_EXP_EXT_ID] = {0xAB00001, 0xAB00002};
const uint32_t  CAN1_Number_GRP_EXT_ID = 2;
const uint32_t  CAN1_Array_GRP_EXT_IDs[CAN1_Number_GRP_EXT_ID][2] = {{0xBC00001, 0xBC00002},{0xCD00001, 0xCD00002}};

/*  Settings related to CAN2 Controller  */
const uint32_t  CAN2_Number_FullCAN_ID = 2;
const uint16_t  CAN2_Array_FullCAN_IDs[CAN2_Number_FullCAN_ID] = {0x010,0x012};

/**********  Änderbare Stellen im Rahmen des EMB Laborversuchs  CAN/AD ********************/
const uint32_t  CAN2_Number_SFF_EXP_ID = 2;
const uint16_t  CAN2_Array_SFF_EXP_IDs[CAN2_Number_SFF_EXP_ID] = {0x711, 0x712};
/******************************************************************************/

const uint32_t  CAN2_Number_GRP_SFF_ID = 2;
const uint16_t  CAN2_Array_GRP_SFF_IDs[CAN2_Number_GRP_SFF_ID][2] = {{0x400,0x4FF},{0x500,0x5FF}};
const uint32_t  CAN2_Number_EXP_EXT_ID = 2;
const uint32_t  CAN2_Array_EXP_EXT_IDs[CAN2_Number_EXP_EXT_ID] = {0xCD00001, 0xCD00002};
const uint32_t  CAN2_Number_GRP_EXT_ID = 2;
const uint32_t  CAN2_Array_GRP_EXT_IDs[CAN2_Number_GRP_EXT_ID][2] = {{0xDE00001, 0xDE00002},{0xEF00001, 0xEF00002}};
