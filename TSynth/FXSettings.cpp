// FXSettings.cpp - FX settings implementation
// MIT License - see LICENSE.md

#include "FXSettings.h"
#include "FXManager.h"

// In-memory storage for current FX type
static FXType globalFXType = FXType::Off;

namespace FXSettings {

FXType getGlobalFXType() {
  return globalFXType;
}

void setGlobalFXType(FXType type) {
  globalFXType = type;
  // Update FXManager with new type
  FXManager::getInstance().setType(type);
}

void storeFXType(FXType type) {
  // TODO: Integrate with EepromMgr for persistence
  // For now, just update in-memory value
  globalFXType = type;
}

FXType loadFXType() {
  // TODO: Load from EEPROM via EepromMgr
  // For now, return default
  return FXType::Off;
}

}  // namespace FXSettings
