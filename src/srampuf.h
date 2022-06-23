//########################################################################
// (C) Embedded Systems Lab
// All rights reserved.
// ------------------------------------------------------------
// This document contains proprietary information belonging to
// Research & Development FH OÖ Forschungs und Entwicklungs GmbH.
// Using, passing on and copying of this document or parts of it
// is generally not permitted without prior written authorization.
// ------------------------------------------------------------
// info(at)embedded-lab.at
// https://www.embedded-lab.at/
//########################################################################
// *** File name: srampuf.h
// *** Date of file creation: 2022-04-04
// *** List of autors: Lucas Drack
//########################################################################

#ifndef SRAMPUF_H
#define SRAMPUF_H

#include "stm32f4xx_hal.h"
#include <stm32f401re_system.h>
#include <peripherals.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>



// Board Specifier defines. This allows storing different development boards in the database.
#define BS_NUCLEOF401RE             1
#define BS_NUCLEOL001HE             2   // example - define new BS here

/**
  * @brief  Aggregates all the necessary values needed for this module.
  * @param  husart:     Initialised USART handle.
  * @param  membegin:   Start address of PUF.
  * @param  memend:     EXCLUSIVE end address of PUF. If start is 0x20005000 and end is
  *                     0x20005800, then address 0x200057FF is the last byte that is read!
  * @retval 0 on success, negative on error.
*/
int initSramPuf(USART_HandleTypeDef *husart, unsigned int membegin, unsigned int memend);


/**
  * @brief  Like printf, but sends text to UART. Use like normal printf()
  *         with variadic argument list.
  * @param  format: printf format list.
  * @retval Number of printed chars on success, negative on error.
*/
int uartPrintf(const char *format, ...);


/**
  * @brief  Prints the memory region from membegin to memend, using uartPrintf().
  * @param  membegin:   Start address of memory dump.
  * @param  memend:     End address of memory dump.
  * @param  humanreadable: 0 = binary format. Otherwise: nicely formatted, hexadecimal, row-based printing.
  * @retval Always 0.
*/
int memoryDump(unsigned int membegin, unsigned int memend, int const humanreadable);


/**
  * @brief  Prints the whole on-chip memory of STM32F401RE (0x20000000 - 0x20018000).
  * @param  humanreadable: 0 = binary format. Otherwise: nicely formatted, hexadecimal, row-based printing.
  * @retval Always 0.
*/
int f401reMemDump(int const humanreadable);


/**
  * @brief  Prints the memory area chosen in initSramPuf().
  * @param  humanreadable: 0 = binary format. Otherwise: nicely formatted, hexadecimal, row-based printing.
  * @retval Always 0.
*/
int srampufDump(int const humanreadable);


/**
  * @brief  Gets the buffer length for srampufProtocol(). This is determined by the information bytes (40 bytes)
  *         and the SRAM fingerprint (defined by initSramPuf()).
  * @retval Bytes needed for a protocol packet.
*/
int getSrampufProtocolBytes();


/**
  * @brief  Writes a packet to the given buffer according to the protocol specified in datenbank.md. A Packet
  *         contains a PUF reading (the fingerprint) with all associated data that gets stored in the database
  *         (used SRAM memory region, temperature, Unique Board ID...).
  *         The packets are designed to be sent to the C# backend.
  * @param  buf:    Buffer to store the packet
  * @param  len:    Length of buffer (must be equal to getSrampufProtocolBytes())
  * @param  bs:     Board Specifier, see top of srampuf.h for possible defines
  * @param  temp:   Temperature to be associated with this reading (read this from ADC)
  * @retval 0 on success, negative error code otherwise.
*/
int srampufProtocol(unsigned char *buf, size_t len, size_t bs, float temp);




#endif
