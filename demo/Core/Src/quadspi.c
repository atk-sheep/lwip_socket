/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.c
  * @brief   This file provides code for the configuration
  *          of the QUADSPI instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "quadspi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

QSPI_HandleTypeDef hqspi;

/* QUADSPI init function */
void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 1;
  hqspi.Init.FifoThreshold = 32;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hqspi.Init.FlashSize = 24;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_5_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

void HAL_QSPI_MspInit(QSPI_HandleTypeDef* qspiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */
    /* QUADSPI clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**QUADSPI GPIO Configuration
    PF6     ------> QUADSPI_BK1_IO3
    PF7     ------> QUADSPI_BK1_IO2
    PF8     ------> QUADSPI_BK1_IO0
    PF9     ------> QUADSPI_BK1_IO1
    PB2     ------> QUADSPI_CLK
    PB6     ------> QUADSPI_BK1_NCS
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN QUADSPI_MspInit 1 */

  /* USER CODE END QUADSPI_MspInit 1 */
  }
}

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* qspiHandle)
{

  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

  /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();

    /**QUADSPI GPIO Configuration
    PF6     ------> QUADSPI_BK1_IO3
    PF7     ------> QUADSPI_BK1_IO2
    PF8     ------> QUADSPI_BK1_IO0
    PF9     ------> QUADSPI_BK1_IO1
    PB2     ------> QUADSPI_CLK
    PB6     ------> QUADSPI_BK1_NCS
    */
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2|GPIO_PIN_6);

  /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

  /* USER CODE END QUADSPI_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//QSPIå?‘é€?å‘½ä»¤
//instruction:è¦?å?‘é€?çš„æŒ‡ä»¤
//address:å?‘é€?åˆ°çš„ç›®çš„åœ°å?€
//dummyCycles:ç©ºæŒ‡ä»¤å‘¨æœŸæ•°
//	instructionMode:æŒ‡ä»¤æ¨¡å¼?;QSPI_INSTRUCTION_NONE,QSPI_INSTRUCTION_1_LINE,QSPI_INSTRUCTION_2_LINE,QSPI_INSTRUCTION_4_LINE  
//	addressMode:åœ°å?€æ¨¡å¼?; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINE,QSPI_ADDRESS_4_LINE
//	addressSize:åœ°å?€é•¿åº¦;QSPI_ADDRESS_8_BITS,QSPI_ADDRESS_16_BITS,QSPI_ADDRESS_24_BITS,QSPI_ADDRESS_32_BITS
//	dataMode:æ•°æ?®æ¨¡å¼?; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINE,QSPI_DATA_4_LINE

void QSPI_Send_CMD(u32 instruction,u32 address,u32 dummyCycles,u32 instructionMode,u32 addressMode,u32 addressSize,u32 dataMode)
{
    QSPI_CommandTypeDef Cmdhandler;
    
    Cmdhandler.Instruction=instruction;                 	//æŒ‡ä»¤
    Cmdhandler.Address=address;                            	//åœ°å?€
    Cmdhandler.DummyCycles=dummyCycles;                     //è®¾ç½®ç©ºæŒ‡ä»¤å‘¨æœŸæ•°
    Cmdhandler.InstructionMode=instructionMode;				//æŒ‡ä»¤æ¨¡å¼?
    Cmdhandler.AddressMode=addressMode;   					//åœ°å?€æ¨¡å¼?
    Cmdhandler.AddressSize=addressSize;   					//åœ°å?€é•¿åº¦
    Cmdhandler.DataMode=dataMode;             				//æ•°æ?®æ¨¡å¼?
    Cmdhandler.SIOOMode=QSPI_SIOO_INST_EVERY_CMD;       	//æ¯?æ¬¡éƒ½å?‘é€?æŒ‡ä»¤
    Cmdhandler.AlternateByteMode=QSPI_ALTERNATE_BYTES_NONE; //æ— äº¤æ›¿å­—èŠ‚
    Cmdhandler.DdrMode=QSPI_DDR_MODE_DISABLE;           	//å…³é—­DDRæ¨¡å¼?
    Cmdhandler.DdrHoldHalfCycle=QSPI_DDR_HHC_ANALOG_DELAY;

    HAL_QSPI_Command(&hqspi,&Cmdhandler,5000);
}

//QSPIæŽ¥æ”¶æŒ‡å®šé•¿åº¦çš„æ•°æ?®
//buf:æŽ¥æ”¶æ•°æ?®ç¼“å†²åŒºé¦–åœ°å?€
//datalen:è¦?ä¼ è¾“çš„æ•°æ?®é•¿åº¦
//è¿”å›žå€¼:0,æ­£å¸¸
//    å…¶ä»–,é”™è¯¯ä»£ç ?
u8 QSPI_Receive(u8* buf,u32 datalen)
{
    hqspi.Instance->DLR=datalen-1;                           //é…?ç½®æ•°æ?®é•¿åº¦
    if(HAL_QSPI_Receive(&hqspi,buf,5000)==HAL_OK) return 0;  //æŽ¥æ”¶æ•°æ?®
    else return 1;
}

//QSPIå?‘é€?æŒ‡å®šé•¿åº¦çš„æ•°æ?®
//buf:å?‘é€?æ•°æ?®ç¼“å†²åŒºé¦–åœ°å?€
//datalen:è¦?ä¼ è¾“çš„æ•°æ?®é•¿åº¦
//è¿”å›žå€¼:0,æ­£å¸¸
//    å…¶ä»–,é”™è¯¯ä»£ç ?
u8 QSPI_Transmit(u8* buf,u32 datalen)
{
    hqspi.Instance->DLR=datalen-1;                            //é…?ç½®æ•°æ?®é•¿åº¦
    if(HAL_QSPI_Transmit(&hqspi,buf,5000)==HAL_OK) return 0;  //å?‘é€?æ•°æ?®
    else return 1;
}
/* USER CODE END 1 */
