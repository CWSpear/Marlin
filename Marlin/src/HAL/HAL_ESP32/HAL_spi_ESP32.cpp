/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2017 Victor Perez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef ARDUINO_ARCH_ESP32

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------

#include "HAL.h"
#include "../shared/HAL_SPI.h"
#include <pins_arduino.h>
#include "spi_pins.h"
#include "../../core/macros.h"
#include <SPI.h>

// --------------------------------------------------------------------------
// Public Variables
// --------------------------------------------------------------------------

static SPISettings spiConfig;

// --------------------------------------------------------------------------
// Public functions
// --------------------------------------------------------------------------

#if ENABLED(SOFTWARE_SPI)

  // --------------------------------------------------------------------------
  // Software SPI
  // --------------------------------------------------------------------------
  #error "Software SPI not supported for ESP32. Use Hardware SPI."

#else

// --------------------------------------------------------------------------
// Hardware SPI
// --------------------------------------------------------------------------

void spiBegin() {
  #if !PIN_EXISTS(SS)
    #error "SS_PIN not defined!"
  #endif

  OUT_WRITE(SS_PIN, HIGH);
}

void spiInit(uint8_t spiRate) {
  uint32_t clock;

  switch (spiRate) {
    case SPI_FULL_SPEED:      clock = 16000000; break;
    case SPI_HALF_SPEED:      clock = 8000000;  break;
    case SPI_QUARTER_SPEED:   clock = 4000000;  break;
    case SPI_EIGHTH_SPEED:    clock = 2000000;  break;
    case SPI_SIXTEENTH_SPEED: clock = 1000000;  break;
    case SPI_SPEED_5:         clock = 500000;   break;
    case SPI_SPEED_6:         clock = 250000;   break;
    default:                  clock = 1000000; // Default from the SPI library
  }

  spiConfig = SPISettings(clock, MSBFIRST, SPI_MODE0);
  SPI.begin();
}

uint8_t spiRec(void) {
  SPI.beginTransaction(spiConfig);
  uint8_t returnByte = SPI.transfer(0xFF);
  SPI.endTransaction();
  return returnByte;
}

void spiRead(uint8_t* buf, uint16_t nbyte) {
  SPI.beginTransaction(spiConfig);
  SPI.transferBytes(0, buf, nbyte);
  SPI.endTransaction();
}

void spiSend(uint8_t b) {
  SPI.beginTransaction(spiConfig);
  SPI.transfer(b);
  SPI.endTransaction();
}

void spiSendBlock(uint8_t token, const uint8_t* buf) {
  SPI.beginTransaction(spiConfig);
  SPI.transfer(token);
  SPI.writeBytes(const_cast<uint8_t*>(buf), 512);
  SPI.endTransaction();
}

void spiBeginTransaction(uint32_t spiClock, uint8_t bitOrder, uint8_t dataMode) {
  spiConfig = SPISettings(spiClock, bitOrder, dataMode);

  SPI.beginTransaction(spiConfig);
}

#endif // !SOFTWARE_SPI

#endif // ARDUINO_ARCH_ESP32
