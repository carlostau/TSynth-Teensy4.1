#include "FXSettings.h"
#include "FXManager.h"
#include "EepromMgr.h"

static uint8_t g_fx_type = FX_CHORUS;
static bool g_fx_initialized = false;

uint8_t getGlobalFXType(){
  if (!g_fx_initialized) {
    g_fx_type = getFXType();
    g_fx_initialized = true;
  }
  return g_fx_type;
}

void setGlobalFXType(uint8_t t){
  if (t == g_fx_type) return;
  g_fx_type = t;
  // update runtime engine
  FXManager::setType((FXType)g_fx_type);
}

void storeGlobalFXType(uint8_t t){
  g_fx_type = t;
  storeFXType(t);
}
