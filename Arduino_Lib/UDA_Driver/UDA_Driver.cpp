/* UDA_Driver.cpp
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
*/

#include "UDA_Driver.h"
#include "Timer1/TimerOne.cpp"
#include "Timer1/TimerOne.h"

static uint8_t    UDA_pin[UDA_Driver::UDA_MAX_PIN];
static unsigned char idxRefresh = 0;
static uint8_t    bargraphMode = 0;
static UDA_Driver::s_DIGIT digit[7];

static const unsigned char _char8Seg[] =
{
   0x00, //--- ' '
   0x00, //--- '!'
   0x00, //--- '"'
   0x00, //--- '#'
   0x00, //--- '$'
   0x00, //--- '%'
   0x00, //--- '&'
   0x00, //--- '''
   0x00, //--- '('
   0x00, //--- ')'
   0x00, //--- '*'
   0x00, //--- '+'
   0x80, //--- ','
   0x40, //--- '-'
   0x80, //--- '.'
   0x00, //--- '/'
   0x3F, //--- '0'
   0x06, //--- '1'
   0x5B, //--- '2'
   0x4F, //--- '3'
   0x66, //--- '4'
   0x6D, //--- '5'
   0x7D, //--- '6'
   0x07, //--- '7'
   0x7F, //--- '8'
   0x6F, //--- '9'
   0x00, //--- ':'
   0x00, //--- ';'
   0x00, //--- '<'
   0x00, //--- '='
   0x00, //--- '>'
   0x00, //--- '?'
   0x00, //--- '@'
   0x77, //--- 'A'
   0x7F, //--- 'B'
   0x39, //--- 'C'
   0x5E, //--- 'D'
   0x79, //--- 'E'
   0x71, //--- 'F'
   0x7D, //--- 'G'
   0x76, //--- 'H'
   0x06, //--- 'I'
   0x0E, //--- 'J'
   0x00, //--- 'K'
   0x38, //--- 'L'
   0x00, //--- 'M'
   0x00, //--- 'N'
   0x3F, //--- 'O'
   0x73, //--- 'P'
   0x00, //--- 'Q'
   0x00, //--- 'R'
   0x6D, //--- 'S'
   0x00, //--- 'T'
   0x3E, //--- 'U'
   0x00, //--- 'V'
   0x00, //--- 'W'
   0x00, //--- 'X'
   0x66, //--- 'Y'
   0x00, //--- 'Z'
   0x00, //--- '['
   0x00, //--- '\'
   0x00, //--- ']'
   0x00, //--- '^'
   0x00, //--- '_'
   0x00, //--- '`'
   0x5C, //--- 'a'
   0x7C, //--- 'b'
   0x58, //--- 'c'
   0x5E, //--- 'd'
   0x7B, //--- 'e'
   0x71, //--- 'f'
   0x6F, //--- 'g'
   0x74, //--- 'h'
   0x04, //--- 'i'
   0x0C, //--- 'j'
   0x00, //--- 'k'
   0x30, //--- 'l'
   0x00, //--- 'm'
   0x54, //--- 'n'
   0x5C, //--- 'o'
   0x73, //--- 'p'
   0x67, //--- 'q'
   0x50, //--- 'r'
   0x6D, //--- 's'
   0x78, //--- 't'
   0x1C, //--- 'u'
   0x00, //--- 'v'
   0x00, //--- 'w'
   0x00, //--- 'x'
   0x66, //--- 'y'
   0x00, //--- 'z'
   0x00, //--- '°'
   0x46, //--- '-1'
   0x00, //--- 'µ'
   0x00, //--- 'il'
   0x00  //--- 'll'
};

#define     IDX_CHAR(X)                      (X) - 0x20


float progressMin,progressMax;

UDA_Driver::UDA_Driver()
{
   UDA_pin[UDA_SIN] = 8;
   UDA_pin[UDA_CLK] = 7;
   UDA_pin[UDA_DSEL] = 5;
   UDA_pin[UDA_LATCH] = 12;
   UDA_pin[UDA_CSEL0] = 13;
   UDA_pin[UDA_CSEL1] = 11;
   UDA_pin[UDA_CSEL2] = 6;
   
   progressMin = 0.0;
   progressMax = 100.0;
}

void UDA_Driver::Start( void )
{
   for ( int i = 0 ; i < UDA_MAX_PIN ; i++ )
   {
      pinMode(UDA_pin[i], OUTPUT);
   }
}

void UDA_Driver::SetPin(uint8_t selectPin , uint8_t val )
{
   UDA_pin[selectPin] = val;
}

//-----------------------------------------------------------------------------
// FONCTION    :  LCD_UpdateSeg
//
// DESCRIPTION :  Mise a jour des digits
//-----------------------------------------------------------------------------
void _UpdateSeg(unsigned char idxDigit, unsigned short value)
{
   unsigned short mask = 0x8000;
   unsigned char i;
   
   digitalWrite(UDA_pin[UDA_Driver::UDA_DSEL], 1);
   
   for ( i = 0 ; i < 16 ; i++ )
   {
      digitalWrite(UDA_pin[UDA_Driver::UDA_CLK], 0);
      digitalWrite(UDA_pin[UDA_Driver::UDA_SIN] , (value & mask) == mask);
      digitalWrite(UDA_pin[UDA_Driver::UDA_CLK], 1);
      mask = mask >> 1;
   }
  
   //--- Selection du digit a rafraichir
   digitalWrite(UDA_pin[UDA_Driver::UDA_CSEL0],(idxDigit & 0x01) == 0x01);
   digitalWrite(UDA_pin[UDA_Driver::UDA_CSEL1],(idxDigit & 0x02) == 0x02);
   digitalWrite(UDA_pin[UDA_Driver::UDA_CSEL2],(idxDigit & 0x04) == 0x04);

   //--- Latch des donnees
   digitalWrite(UDA_pin[UDA_Driver::UDA_LATCH], 1);
   digitalWrite(UDA_pin[UDA_Driver::UDA_LATCH], 0);

   digitalWrite(UDA_pin[UDA_Driver::UDA_DSEL], 0);

}

void _RefreshScreen(void)
{
  _UpdateSeg(idxRefresh,digit[idxRefresh].value);
  idxRefresh++;
  if ( idxRefresh >= 5 )
    idxRefresh = 0;
}

void UDA_Driver::RefreshScreen(void)
{
   _RefreshScreen();
}

void UDA_Driver::AutoRefresh(bool enable,unsigned long period)
{
   if ( enable )
   {
      Timer1.initialize(period);
      Timer1.attachInterrupt(_RefreshScreen);
   }
   else
   {
      Timer1.stop();
   } 
}

//-----------------------------------------------------------------------------
// FONCTION    :  LCD_PutChar
//
// DESCRIPTION :  Met a jour un digit d'affichage
//-----------------------------------------------------------------------------
void UDA_Driver::PutChar(unsigned char idxDigit, unsigned char data)
{
   s_DIGIT  *pDigit = &digit[idxDigit];

   pDigit->value = _char8Seg[IDX_CHAR(data&0x7F)];
   
   if ( data & 0x80 )
      pDigit->value |= 0x80;
}

void UDA_Driver::PutFloat(float val, uint8_t nbDecimal)
{
   char tab[4];
   unsigned char i;
   
   for ( i = 0 ; i < nbDecimal ; i++ )
   {
      val = val * 10.0;
   }
   
   unsigned short val10 = (unsigned short)(val);
   memset(tab,0,sizeof(tab));
   sprintf(tab,"%02d",val10);
   unsigned char offset = 0;
   for ( i = 0 ; i < 3 ; i++ )
   {
      if ( i >= ( 3 - strlen(tab)))
      {         
         if ( (i == (2-nbDecimal)) && (nbDecimal != 0) )
            PutChar(i,tab[offset] | 0x80 );
         else
            PutChar(i,tab[offset] );
         offset++;
      }
      else
      {
         PutChar(i,' ');
      }
   }
   
}

const unsigned long posLed[21] = {  0x02000000  ,  0x01000000  ,  0x00080000  ,  0x00100000  ,  0x00800000  ,
                                    0x00400000  ,  0x00040000  ,  0x00200000  ,  0x00020000  ,  0x00010000  ,
                                    0x00000001  ,  0x00000002  ,  0x00000004  ,  0x00000008  ,  0x00000010  ,
                                    0x00000020  ,  0x00000040  ,  0x00000080  ,  0x00000100  ,  0x00000200  ,
                                    0x00000400  };
 
void _progressBar(float percent)
{
   unsigned long val = 0;
   uint8_t nbBoucles; 
   uint8_t i;
   
   if ( percent <= 0.0 )
   {
      nbBoucles = 1;
   }
   else if ( percent >= 100.0 )
   {
      nbBoucles = 21;
   }
   else
   {
      nbBoucles = (percent / 5.0)+1;
   }
   if( bargraphMode == UDA_Driver::BARGRAPH_LINE )
   {
      for ( i = 0 ; i <  nbBoucles ; i++ )
      {
         val |= posLed[i];
      }
   }
   if ( bargraphMode == UDA_Driver::BARGRAPH_DOT )
   {
      val = posLed[nbBoucles];
   }
   digit[4].value = ((val & 0xFFFF0000) >> 16);
   digit[3].value = (val & 0x0000FFFF);
   
}

void UDA_Driver::SetBargraphConf(float valMin,float valMax)
{
   bargraphMode = UDA_Driver::BARGRAPH_LINE;
   progressMin = valMin;
   progressMax = valMax;
}

void UDA_Driver::SetBargraphConf(float valMin,float valMax , uint8_t mode)
{
   bargraphMode = mode;
   progressMin = valMin;
   progressMax = valMax;
}

void UDA_Driver::PutBargraph(float val)
{
   float valueToDisp;
   
   if ( (val - progressMin) <= 0 )
   {
      valueToDisp = 0.0;
   }
   else
   {
      valueToDisp = ((val - progressMin) / ( progressMax - progressMin)*100.0);
   }
   _progressBar(valueToDisp);
}
