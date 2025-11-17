#include "FXManager.h"
#include "Delay.h"
#include "FXSettings.h"
#include <cstdint>
#include <cstring>

static FX * volatile activeFX = nullptr;
static FXType volatile activeFXType = FX_CHORUS;
static float masterSampleRate = 44100.0f;

static FX* createFXInstance(FXType t, float sampleRate) {
  if (t == FX_DELAY) {
    return new Delay(sampleRate);
  }
  // default: no FX
  return nullptr;
}

void FXManager::init(float sampleRate) {
  masterSampleRate = sampleRate;
  activeFXType = (FXType)getGlobalFXType();
  activeFX = createFXInstance(activeFXType, masterSampleRate);
}

void FXManager::shutdown() {
  __disable_irq();
  FX *tmp = activeFX;
  activeFX = nullptr;
  __enable_irq();
  if (tmp) { delete tmp; tmp = nullptr; }
}

void FXManager::setType(FXType t) {
  if (t == activeFXType) return;
  FX *newFX = createFXInstance(t, masterSampleRate);
  __disable_irq();
  FX *old = activeFX;
  activeFX = newFX;
  activeFXType = t;
  __enable_irq();
  if (old) delete old;
}

FXType FXManager::getType() {
  return activeFXType;
}

void FXManager::process(float *buf, int numFrames) {
  FX *f = activeFX;
  if (f) f->process(buf, numFrames);
}

void FXManager::setAmount(float v) {
  FX *f = activeFX;
  if (f) f->setAmount(v);
}
void FXManager::setMix(float v) {
  FX *f = activeFX;
  if (f) f->setMix(v);
}
