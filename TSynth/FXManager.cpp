// FXManager.cpp - Manager implementation for FX switching
// MIT License - see LICENSE.md

#include "FXManager.h"

FXManager::FXManager()
  : activeFX_(nullptr)
  , currentType_(FXType::Off)
  , sampleRate_(44100.0f)
  , initialized_(false)
{
}

FXManager::~FXManager() {
  shutdown();
}

FXManager& FXManager::getInstance() {
  static FXManager instance;
  return instance;
}

void FXManager::init(float sampleRate) {
  sampleRate_ = sampleRate;
  
  // Initialize all FX instances with sample rate
  delayFX_.init(sampleRate);
  
  initialized_ = true;
  
  // Default to Off
  setType(FXType::Off);
}

void FXManager::setType(FXType type) {
  if (!initialized_) {
    return;
  }
  
  FX* newFX = nullptr;
  
  // Select FX instance based on type
  switch (type) {
    case FXType::Delay:
      newFX = &delayFX_;
      break;
    case FXType::Chorus:
      // TODO: Implement Chorus FX
      newFX = nullptr;  // Fall back to Off
      type = FXType::Off;
      break;
    case FXType::Off:
    default:
      newFX = nullptr;
      type = FXType::Off;
      break;
  }
  
  // Atomically swap the active FX pointer (disable interrupts briefly)
  __disable_irq();
  activeFX_ = newFX;
  currentType_ = type;
  __enable_irq();
}

void FXManager::process(float* buf, int numFrames) {
  // Read active FX pointer atomically
  __disable_irq();
  FX* fx = activeFX_;
  __enable_irq();
  
  // Process if FX is active
  if (fx) {
    fx->process(buf, numFrames);
  }
}

void FXManager::setAmount(float amount) {
  // Read active FX pointer atomically
  __disable_irq();
  FX* fx = activeFX_;
  __enable_irq();
  
  // Forward to active FX
  if (fx) {
    fx->setAmount(amount);
  }
}

void FXManager::setMix(float mix) {
  // Read active FX pointer atomically
  __disable_irq();
  FX* fx = activeFX_;
  __enable_irq();
  
  // Forward to active FX
  if (fx) {
    fx->setMix(mix);
  }
}

void FXManager::shutdown() {
  __disable_irq();
  activeFX_ = nullptr;
  currentType_ = FXType::Off;
  __enable_irq();
  
  initialized_ = false;
}
