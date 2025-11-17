#pragma once
#include <stdint.h>
#include "FX.h"

uint8_t getGlobalFXType();
void setGlobalFXType(uint8_t t);
void storeGlobalFXType(uint8_t t); // currently in-memory; replace with persistence later
