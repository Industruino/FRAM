//  Basic read/write functions for the MB85RS64A SPI FRAM chip
//  Copyright (C) 2017  Industruino <connect@industruino.com>
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//  Developed by Claudio Indellicati <bitron.it@gmail.com>

#ifndef __FRAM_H__
#define __FRAM_H__

#include <Arduino.h>
#include <SPI.h>


#define FRAM_DEFAULT_CS_PIN ((uint8_t) 6)

#define FRAM_DEFAULT_SPI_SETTINGS SPISettings(1000000, MSBFIRST, SPI_MODE0)

// MB85RS64A - 64 K (8 K x 8) bit SPI FRAM
#define FRAM_SIZE ((uint16_t) 0x2000)

////////////////////////////////////////////////////////////////////////////////

class FramClass
{
  private:

    uint8_t csPin;
    SPISettings spiSettings;

    inline void assertCS() { SPI.beginTransaction(spiSettings); digitalWrite(csPin, LOW); };
    inline void deassertCS() { digitalWrite(csPin, HIGH); SPI.endTransaction(); };

  public:

    FramClass();

    void begin (uint8_t csPin = FRAM_DEFAULT_CS_PIN, SPISettings ss = FRAM_DEFAULT_SPI_SETTINGS);

    uint8_t write (uint16_t addr, uint8_t *data, uint16_t count);
    uint8_t read (uint16_t addr, uint8_t *dataBuffer, uint16_t count);
};

////////////////////////////////////////////////////////////////////////////////

extern FramClass Fram;


#endif   // __FRAM_H__

