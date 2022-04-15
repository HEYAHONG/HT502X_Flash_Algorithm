/**************************************************************************//**
 * @file     FlashPrg.c
 * @brief    Flash Programming Functions adapted for HT502X Flash
 * @version  V1.0.0
 * @date     10. January 2018
 ******************************************************************************/
/*
 * Copyright (c) 2010-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "FlashOS.h"        // FlashOS Structures

/* 
   Mandatory Flash Programming Functions (Called by FlashOS):
                int Init        (unsigned long adr,   // Initialize Flash
                                 unsigned long clk,
                                 unsigned long fnc);
                int UnInit      (unsigned long fnc);  // De-initialize Flash
                int EraseSector (unsigned long adr);  // Erase Sector Function
                int ProgramPage (unsigned long adr,   // Program Page Function
                                 unsigned long sz,
                                 unsigned char *buf);

   Optional  Flash Programming Functions (Called by FlashOS):
                int BlankCheck  (unsigned long adr,   // Blank Check
                                 unsigned long sz,
                                 unsigned char pat);
                int EraseChip   (void);               // Erase complete Device
      unsigned long Verify      (unsigned long adr,   // Verify Function
                                 unsigned long sz,
                                 unsigned char *buf);

       - BlanckCheck  is necessary if Flash space is not mapped into CPU memory space
       - Verify       is necessary if Flash space is not mapped into CPU memory space
       - if EraseChip is not provided than EraseSector for all sectors is called
*/

/*
HT502X 相关寄存器定义
*/
#include "stdint.h"
#include "stdlib.h"

#define M8(adr) (*(( volatile uint8_t *) (adr))) //无需考虑对齐问题
#define M16(adr) (*(( volatile uint16_t *) (adr))) //需半字对齐，即 adr 的 bit0 必须为 0
#define M32(adr) (*(( volatile uint32_t *) (adr))) //需字对齐，即 adr 的 bit0 与 bit1 都必须为 0

//Flash寄存器基地址
#define FLASH_REGISTER_BASE (0x4000F000)
#define WPREG (*(uint32_t *)(FLASH_REGISTER_BASE+0x00))
#define FLASHCON (*(uint32_t *)(FLASH_REGISTER_BASE+0x34))
#define FLASHLOCK (*(uint32_t *)(FLASH_REGISTER_BASE+0x38))

#define WATCHDOG_BASE 0x40010000
#define WATCHDOG_WDTCLR (WATCHDOG_BASE+0x04)
#define WATCHDOG_CLR() M16(WATCHDOG_WDTCLR)=0xAAFF;


/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int Init (unsigned long adr, unsigned long clk, unsigned long fnc) {

  WPREG = 0xA55A;
	
	
  return (0);                                  // Finished without Errors
}


/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit (unsigned long fnc) {

 
  return (0);                                  // Finished without Errors
}




/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseChip (void) {

  WPREG = 0xA55A;
  FLASHLOCK = 0x7A68; //unlock flash memory
  FLASHCON = 0x03; // mass erase
  M32(0) = 0x00; //prog_data 可以为任意的数据（32bit），
  // prog_address 为 256K Flash 的任意地址
  while (FLASHCON&0x04) //等待 flash 全擦除操作完成，最长 10ms
	{
		WATCHDOG_CLR();	
	}
  //全擦除会导致用户执行的代码全部被擦除掉
	FLASHLOCK = 0x0000; //lock flash memory	
	WPREG = 0x0000;
  return (0);                                  // Finished without Errors
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseSector (unsigned long adr) {

  WPREG = 0xA55A;
  FLASHLOCK = 0x7A68; //unlock flash memory
  FLASHCON = 0x02; //page erase
  M32(adr) = 0; //prog_data 可以为任意的数据（32bit），
  //prog_address 为需要擦除的 Flash 页内的任意一个地址
  while (FLASHCON&0x04) //等待 flash 页擦除操作完成，最长 2m
	{
		WATCHDOG_CLR();		
	}
	FLASHLOCK = 0x0000; //lock flash memory	
	WPREG = 0x0000;
  return (0);                                  // Finished without Errors
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) {

  WPREG = 0xA55A;
  FLASHLOCK = 0x7A68; //unlock flash memory
	while(sz--)
	{
		FLASHCON = 0x01; //program
		M8(adr+sz) = buf[sz];
		while (FLASHCON&0x04)
		{
			WATCHDOG_CLR();			
		}
		WATCHDOG_CLR();	
	}
	
  FLASHLOCK = 0x0000; //lock flash memory	
	WPREG = 0x0000;
  return (0);                                  // Finished without Errors
}
