/******************** (C) COPYRIGHT 2012 Belling Inc. ********************
* File Name          : compile define.h
* Author             : Lv Haifeng
* Version            : V2.0
* Date               : 2016-04-22
* Description        : 
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODECTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, BELLING INC. SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAINMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#ifndef _COMPILE_DEFINE_H
#define _COMPILE_DEFINE_H
/* Pragma ------------------------------------------------------------------------*/
#pragma pack(1)						//1字节数据对齐

/* Basic Type Define ------------------------------------------------------------*/
#define DEVICE_CC	1				//Concentrator
#define DEVICE_MT	2				//Meter Module
#define DEVICE_DC	3				//Data Collector

#define SOC_MODE	1
#define SPI_MODE	2

#define CPU_STM32F103ZE			1
#define CPU_BL6810				2
#define CPU_STM32F103RC			3
#define CPU_STM32F030C8			4

#define CPU_STM32

#define CPU_TYPE CPU_STM32F030C8

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed long s32;



typedef unsigned char * ARRAY_HANDLE;

typedef enum
{
	FALSE = 0,
	TRUE = !FALSE
}BOOL;

typedef enum
{
	FAIL = 0,
	OKAY = !FAIL
}STATUS;



/* Compatibility Defines -----------------------------------------------------*/
#if CPU_TYPE==CPU_STM32F103ZE || CPU_TYPE==CPU_STM32F103RC || CPU_TYPE==CPU_STM32F030C8
	#define idata
	#define xdata					// keep compatibility with 8051 code
	#define code
	#define reentrant
#endif

#define ENTER_CRITICAL()
#define EXIT_CRITICAL()


#endif


