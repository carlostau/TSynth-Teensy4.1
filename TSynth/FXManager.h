// FXManager.h - Manager for real-time safe FX switching and processing
// MIT License - see LICENSE.md
#pragma once

#include "FX.h"
#include "Delay.h"

class FXManager {
public:
  static FXManager& getInstance();
  
  // Initialize manager with sample rate
  void init(float sampleRate);
  
  // Set active FX type (safe for real-time, uses interrupt disable)
  void setType(FXType type);
  
  // Get current FX type
  FXType getType() const { return currentType_; }
  
  // Process audio through active FX
  void process(float* buf, int numFrames);
  
  // Forward parameter changes to active FX
  void setAmount(float amount);
  void setMix(float mix);
  
  // Cleanup
  void shutdown();
  
private:
  FXManager();
  ~FXManager();
  
  // Prevent copying
  FXManager(const FXManager&) = delete;
  FXManager& operator=(const FXManager&) = delete;
  
  // Preallocated FX instances
  Delay delayFX_;
  
  // Active FX pointer (swapped atomically)
  FX* activeFX_;
  FXType currentType_;
  float sampleRate_;
  bool initialized_;
};
