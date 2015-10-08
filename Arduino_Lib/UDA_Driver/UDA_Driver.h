/*
* UDA_Driver.h
*
* UDA_Driver - A Driver for UDA library for Arduino.
* Author: mromani@ottotecnica.com
* Copyright (c) 2010 OTTOTECNICA Italy
*
* This library is free software; you can redistribute it
* and/or modify it under the terms of the GNU Lesser
* General Public License as published by the Free Software
* Foundation; either version 2.1 of the License, or (at
* your option) any later version.
*
* This library is distributed in the hope that it will
* be useful, but WITHOUT ANY WARRANTY; without even the
* implied warranty of MERCHANTABILITY or FITNESS FOR A
* PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* You should have received a copy of the GNU Lesser
* General Public License along with this library; if not,
* write to the Free Software Foundation, Inc.,
* 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*
*/


#ifndef UDA_DRIVER_H
#define UDA_DRIVER_H
   #if defined(ARDUINO) && ARDUINO >= 100
      #include <Arduino.h>
   #else
      #include <WProgram.h>
   #endif
   
   class UDA_Driver 
   {
      public:

         typedef struct
         {
            bool              comma;
            unsigned short    value;
         } s_DIGIT;

         enum
         {
            UDA_SIN = 0,
            UDA_CLK,
            UDA_DSEL,
            UDA_LATCH,
            UDA_CSEL0,
            UDA_CSEL1,
            UDA_CSEL2,
            UDA_MAX_PIN,
         };
         
         enum
         {
            BARGRAPH_LINE = 0,
            BARGRAPH_DOT,
         };
         // constructor
         UDA_Driver();
         
         void Start( void );
         void SetPin(uint8_t selectPin , uint8_t val );
         void AutoRefresh(bool enable,unsigned long period);
         
         void RefreshScreen(void); 
         
         void UpdateSeg(unsigned char idxDigit, unsigned short value);
         
         void PutChar(unsigned char idxDigit, unsigned char data);
         
         //void PutFloat(float val);
         void PutFloat(float val, uint8_t nbDecimal);
         void PutBargraph(float val);
         void SetBargraphConf(float valMin,float valMax);
         void SetBargraphConf(float valMin,float valMax , uint8_t mode);
   };
#endif