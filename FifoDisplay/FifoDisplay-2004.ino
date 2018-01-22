/**
  **************************************************************************************
  * File Name          : FifoDisplay-2004
  * Date of create     : 2018-01-21
  * Author             : Nicola Pomaro 
  *                    : nicola.pomaro@gmail.com
  *                    
  * Description        : Open source Arduino based McFIFO display project adapted for 
  *                      2004 LCD Display and use in Streaminator i2s Audio Source
  * 
  * This program is a free software: you can redistribute it and/or modify it
  * under the terms of the GNU Lesser General Public License as published by the
  * Free Software Foundation, either version 3 of the License, or any later
  * version.
  *
  * This program is distributed in the hope that it will be useful, but WITHOUT
  * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
  * for more details.
  *
  * You should have received a copy of the GNU Lesser General Public License
  * along with this program. If not, see <http://www.gnu.org/licenses/>.        
  * **************************************************************************************
  * COPYRIGHT(c) 2018 Nicola Pomaro All rights reserved
  */

/** ORIGINAL FILE COPYRIGHT NOTICE
  **************************************************************************************
  * File Name          : FifoDisplay
  * Date of create     : 2017-06-10
  * Author             : Ian Jin  
  *                    : iancanada.mail@gmail.com
  *                    : Twitter: @iancanadaTT
  *                    
  * Description        : Open source Arduino based McFIFO display project
  * 
  * This program is a free software: you can redistribute it and/or modify it
  * under the terms of the GNU Lesser General Public License as published by the
  * Free Software Foundation, either version 3 of the License, or any later
  * version.
  *
  * This program is distributed in the hope that it will be useful, but WITHOUT
  * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
  * for more details.
  *
  * You should have received a copy of the GNU Lesser General Public License
  * along with this program. If not, see <http://www.gnu.org/licenses/>.        
  * **************************************************************************************
  * COPYRIGHT(c) Ian Jin All rights reserved
  */

#include "FifoMessage.hpp"                 // mcFifo library header
#include <Wire.h>                          
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

hd44780_I2Cexp lcd;                        // declare lcd object: auto locate & config expander chip

CfifoMessage fifoMessage;
package_t package;

byte bar[8] = {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F};   // define special character for progress bar
const long progress = 1600;                 // time duration of progress bar segments in ms
const long scroll = 1000;                   // scrolling interval of title in ms
int pos = 3;                                // initial position of title
int dir = 1;                                // initial direction of title scroll
unsigned long previousMillis = 0; 
unsigned long currentMillis = 0;

void setup() {
  lcd.createChar(0, bar);                    // create special character for progress bar
  lcd.begin(20, 4);                          // initialize 20*4 LCD
  lcd.setBacklight(255);                     // switch on display light
  lcd.clear();
                                 
  // Boot message scrolling vertically
  lcd.setCursor(0,3);lcd.print("   Streamin-ATOR    ");
  delay(500);
  lcd.setCursor(0,2);lcd.print("   Streamin-ATOR    ");
  lcd.setCursor(0,3);lcd.print("                    ");
  delay(500);
  lcd.setCursor(0,1);lcd.print("   Streamin-ATOR    ");
  lcd.setCursor(0,2);lcd.print("                    ");
  lcd.setCursor(0,3);lcd.print("I2S/DSD Audio Source");
  delay(500);
  lcd.setCursor(0,0);lcd.print("   Streamin-ATOR    ");
  lcd.setCursor(0,1);lcd.print("                    ");
  lcd.setCursor(0,2);lcd.print("I2S/DSD Audio Source");
  lcd.setCursor(0,3);lcd.print("                    ");
  delay(3000); 
  lcd.setCursor(0,2); lcd.print("                    ");
  
  // Progress bar, set segment duration with progress constant
  lcd.setCursor(0,2);
  for (int x = 0; x < 20; x ++)
  {
  previousMillis = millis();
  currentMillis = millis();
  while (currentMillis - previousMillis <= progress) currentMillis = millis();
  previousMillis = currentMillis;
  lcd.write(0);
  }
  lcd.clear();
  
  // start serial port 
  Serial.begin(9600);                  // start serial por
}

void loop() {
  // title scrolling 
  currentMillis = millis();
  if (currentMillis - previousMillis > scroll)
  {
  if ((pos > 0) && (dir == 1))         // scroll to left
    { 
      lcd.setCursor(0,0); lcd.print("                    ");
      lcd.setCursor(pos,0);lcd.print("Streamin-ATOR");
      pos = pos-1;
      previousMillis = currentMillis;
    }
  else if ((pos < 7) && (dir == -1))   // scroll to right
    { 
      lcd.setCursor(0,0); lcd.print("                    ");
      lcd.setCursor(pos,0);lcd.print("Streamin-ATOR");
      pos = pos+1;
      previousMillis = currentMillis;
    }
  else dir = -dir;                      // end of line reached
  }
  
  if (fifoMessage.getNewPackageFlag()==0)   // any possible package was processed  
    while(Serial.available())
    {        
      fifoMessage.processReceiving((char)Serial.read());   // read new package  
    }  
  if (fifoMessage.getNewPackageFlag())                     // there is new package received
  {
    package=fifoMessage.getPackage();
    if(!fifoMessage.getFinishInitialFlag())              //it's first time receiving a package
    {
      lcd.clear();                                       // clear screen 
      fifoMessage.setFinishInitialFlag();                // set the flag
    }
  
    switch(package.command)                              // print package data
    {
      case UNLOCK:     lcd.setCursor(0,2);lcd.print("  UN-LOCK    EMPTY  ");break;
      case NOSIGNAL:   lcd.setCursor(0,2);lcd.print("  WAITING FOR DATA  ");break; 
      case WRONGSIGNAL:lcd.setCursor(0,2);lcd.print("  UN-LOCK    EMPTY  ");break;
      case I2S:        lcd.setCursor(0,1);lcd.print("I2S Freq: ");
                       lcd.print(normalizeStrLen(package.valueStr,10));break; 
      case DSD:        lcd.setCursor(0,1);lcd.print("DSD Freq: ");
                       lcd.print(normalizeStrLen(package.valueStr,10));break;
      case XONUMBER:   lcd.setCursor(0,2);lcd.print("XO Clock: ");
                       lcd.print(normalizeStrLen(package.valueStr,10));break;  
      case MASTER:     lcd.setCursor(14,3);lcd.print("Master");break;            
      case SLAVE:      lcd.setCursor(14,3);lcd.print("Slave");break; 
      case FMCK:       lcd.setCursor(10,2);
                       lcd.print(normalizeStrLen(package.valueStr,10));break;
      case DELAYTIME:  lcd.setCursor(0,3);lcd.print("FIFO: ");
                       lcd.print(normalizeStrLen(package.valueStr,5));break;
      case CLEAR:      lcd.clear(); break;
      default:         break;                         
    }
  }
}

