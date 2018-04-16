/**
  **************************************************************************************
    File Name          : FifoDisplay-2004 ver. 1.1
    Date of create     : 2018-04-16
    Author             : Nicola Pomaro
                       : nicola.pomaro@gmail.com

    Description        : Open source Arduino based McFIFO display project adapted for
                         2004 LCD Display and use in Streaminator i2s Audio Source

    This program is a free software: you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
  * **************************************************************************************
    COPYRIGHT(c) 2018 Nicola Pomaro All rights reserved
*/

/** ORIGINAL FILE COPYRIGHT NOTICE
  **************************************************************************************
    File Name          : FifoDisplay
    Date of create     : 2017-06-10
    Author             : Ian Jin
                       : iancanada.mail@gmail.com
                       : Twitter: @iancanadaTT

    Description        : Open source Arduino based McFIFO display project

    This program is a free software: you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
  * **************************************************************************************
    COPYRIGHT(c) Ian Jin All rights reserved
*/

#include "FifoMessage.hpp"                 // mcFifo library header
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

hd44780_I2Cexp lcd;                        // declare lcd object: auto locate & config expander chip

CfifoMessage fifoMessage;
package_t package;

byte bar[8] = {0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011}; // define special character for bar
byte wave1[8] = {0b01111, 0b01001, 0b01001, 0b01001, 0b01001, 0b01001, 0b01001, 0b11001}; // define special character for animated wave
byte wave2[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111}; // define special character for animated wave
byte wave3[8] = {0b01110, 0b01010, 0b01010, 0b01010, 0b01010, 0b01010, 0b01010, 0b11011}; // define special character for animated wave
byte wave4[8] = {0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111}; // define special character for animated wave
byte wave5[8] = {0b11100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00111}; // define special character for animated 
byte wave6[8] = {0b00111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11100}; // define special character for animated wave
byte wave7[8] = {0b01111, 0b01000, 0b01000, 0b01000, 0b01000, 0b01000, 0b01000, 0b11000}; // define special character for animated wave
const long progress = 500;                 // time duration of progress bar segments in ms
const long scroll = 1000;                   // scrolling interval of wave in ms
int cont = 1;                                // initial position of wave
int len = 1;                               // counter of wave
int halt = 0;
String longwave = " ";
char wave[8];
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

void setup() {
  Serial.begin(9600);                  // start serial port
  lcd.begin(20, 4);                          // initialize 20*4 LCD
  lcd.setBacklight(255);                     // switch on display light
  lcd.clear();
  lcd.createChar(0, bar);                    // create special character for bar
  lcd.createChar(1, wave1);                    // create special character for animated wave
  lcd.createChar(2, wave2);                    // create special character for animated wave
  lcd.createChar(3, wave3);                    // create special character for animated wave
  lcd.createChar(4, wave4);                    // create special character for animated wave
  lcd.createChar(5, wave5);                    // create special character for animated wave
  lcd.createChar(6, wave6);                    // create special character for animated wave
  lcd.createChar(7, wave7);                    // create special character for animated wave
  wave[0] = '\0'; wave[1] = '\1'; wave[2] = '\2'; wave[3] = '\3'; 
  wave[4] = '\4'; wave[5] = '\5'; wave[6] = '\6'; wave[7] = '\7';
  for (int i=0; i < 20; i++){
      if (cont == 5) cont = random(6,8);
      else cont = random(1,6);
      longwave = String(wave[cont]) + longwave;
   }
  longwave.trim();

  // Boot message scrolling vertically
  lcd.setCursor(0, 3); lcd.print("    StreaminatoR    ");
  delay(500);
  lcd.setCursor(0, 2); lcd.print("    StreaminatoR    ");
  lcd.setCursor(0, 3); lcd.print("                    ");
  delay(500);
  lcd.setCursor(0, 1); lcd.print("    StreaminatoR    ");
  lcd.setCursor(0, 2); lcd.print("                    ");
  lcd.setCursor(0, 3); lcd.print("I2S/DSD Audio Player");
  delay(500);
  lcd.setCursor(0, 0); lcd.print("    StreaminatoR    ");
  lcd.setCursor(0, 1); lcd.print("                    ");
  lcd.setCursor(0, 2); lcd.print("I2S/DSD Audio Player");
  lcd.setCursor(0, 3); lcd.print("                    ");
  delay(500);
  lcd.setCursor(0, 0); lcd.print("                    ");
  lcd.setCursor(0, 1); lcd.print("I2S/DSD Audio Player");
  lcd.setCursor(0, 2); lcd.print("                    ");
  lcd.setCursor(0, 3); lcd.print("        by          ");
  delay(500);
  lcd.setCursor(0, 0); lcd.print("I2S/DSD Audio Player");
  lcd.setCursor(0, 1); lcd.print("                    ");
  lcd.setCursor(0, 2); lcd.print("        by          ");
  lcd.setCursor(0, 3); lcd.print("                    ");
  delay(500);
  lcd.setCursor(0, 0); lcd.print("                    ");
  lcd.setCursor(0, 1); lcd.print("        by          ");
  lcd.setCursor(0, 2); lcd.print("                    ");
  lcd.setCursor(0, 3); lcd.print(" NICLAL Engineering ");
  delay(500);
  lcd.setCursor(0, 0); lcd.print("        by          ");
  lcd.setCursor(0, 1); lcd.print("                    ");
  lcd.setCursor(0, 2); lcd.print(" NICLAL Engineering ");
  lcd.setCursor(0, 3); lcd.print("                    ");
  delay(500);
  lcd.setCursor(0, 0); lcd.print("                    ");
  lcd.setCursor(0, 1); lcd.print(" NICLAL Engineering ");
  lcd.setCursor(0, 2); lcd.print("                    ");
  lcd.setCursor(0, 3); lcd.print("ver. 1.1 2018-04-16 ");
  delay(3000);
  lcd.setCursor(0, 0); lcd.print("     Booting...     ");
  lcd.setCursor(0, 1); lcd.print("                    ");
  lcd.setCursor(0, 3); lcd.print("                    ");

  // Progress bar, set segment duration with progress constant
  lcd.setCursor(0, 2);
  for (int x = 0; x < 20; x ++)
  {
    previousMillis = millis();
    currentMillis = millis();
    while (currentMillis - previousMillis <= progress) currentMillis = millis();
    previousMillis = currentMillis;
    lcd.write(0);
  }
  lcd.clear();
}

void loop() {
  // Wave Animation
  currentMillis = millis();
  if ((currentMillis - previousMillis > scroll) & (halt == 0))
  {
    lcd.setCursor(0, 3);
    if (len <= 20)         // start tracing wave
    {
      lcd.print(longwave.substring(20 - len));
      len = len + 1;
    }
    else
    {
      if (cont == 5){ cont = random(6,8); }  //wave symbol made with 2 characters
      else { cont = random(1,6);}            //wave symbol made with 1 character
      longwave = String(wave[cont]) + longwave.substring(0,19) ;
      lcd.print(longwave);
    }
    previousMillis = currentMillis; // end of line reached
  }

  if (fifoMessage.getNewPackageFlag() == 0) // any possible package was processed
    while (Serial.available())
    {
      fifoMessage.processReceiving((char)Serial.read());   // read new package
    }
  if (fifoMessage.getNewPackageFlag())                     // there is new package received
  {
    package = fifoMessage.getPackage();

    switch (package.command)                             // print package data
    {
      case UNLOCK:     lcd.clear(); lcd.setCursor(0, 2); lcd.print("  UN-LOCK    EMPTY  "); 
                       len = 1; halt = 1; break;
      case NOSIGNAL:   lcd.clear(); lcd.setCursor(0, 2); lcd.print("  WAITING FOR DATA  "); 
                       len = 1; halt = 1; break;
      case WRONGSIGNAL: lcd.clear(); lcd.setCursor(0, 2); lcd.print("  UN-LOCK    WRONG  "); 
                       len = 1; halt = 1; break;
      case I2S:        lcd.setCursor(0, 1); lcd.print("I2S Freq: ");
        lcd.print(normalizeStrLen(package.valueStr, 10)); break;
      case DSD:        lcd.setCursor(0, 1); lcd.print("DSD Freq: ");
        lcd.print(normalizeStrLen(package.valueStr, 10)); break;
      case XONUMBER:   lcd.setCursor(0, 2); lcd.print("XO Clock: ");
        lcd.print(normalizeStrLen(package.valueStr, 10)); halt = 0; break;
      case MASTER:     lcd.setCursor(14, 3); lcd.print("Master"); break;
      case SLAVE:      lcd.setCursor(14, 3); lcd.print("Slave"); break;
      case FMCK:       lcd.setCursor(10, 2); halt = 0; 
        lcd.print(normalizeStrLen(package.valueStr, 10)); break;
      case DELAYTIME:  lcd.setCursor(0, 0); lcd.print("FIFO: ");
        lcd.print(normalizeStrLen(package.valueStr, 5)); break;
      case CLEAR:      lcd.clear(); break;
      default:         break;
    }
  }
}


