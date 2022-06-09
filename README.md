# stm32-srampuf
 STM32 SRAM PUF project built with PlatformIO.
 
 I developed this tool in the course of my master thesis at FH Hagenberg. It is a very simple and experimental implementation of an SRAM PUF. I deployed it on Nucleo F401RE and F446RE development boards, resulting in the desired SRAM PUF behaviour. Again, this code is simple - I did not modify the bootloader, instead I read the SRAM PUF in main(), sending the SRAM's fingerprint over UART to my C# backend.

---

(C) Embedded Systems Lab / FH Hagenberg

All rights reserved.

This document contains proprietary information belonging to Research & Development FH OÖ Forschungs und Entwicklungs GmbH. Using, passing on and copying of this document or parts of it is generally not permitted without prior written authorization.

info(at)embedded-lab.at

https://www.embedded-lab.at/

The research on which this source code is based has been partly funded by BMK, BMDW, and the State of Upper Austria in the frame of the COMET Programme managed by FFG.
