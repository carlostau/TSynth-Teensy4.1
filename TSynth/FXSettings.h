// FXSettings.h - Bridge between Settings menu and FXManager
// MIT License - see LICENSE.md
#pragma once

#include "FX.h"

namespace FXSettings {
  // Get current FX type
  FXType getGlobalFXType();
  
  // Set FX type and update FXManager
  void setGlobalFXType(FXType type);
  
  // Store FX type to persistence (placeholder for future EEPROM integration)
  void storeFXType(FXType type);
  
  // Load FX type from persistence (placeholder)
  FXType loadFXType();
}
