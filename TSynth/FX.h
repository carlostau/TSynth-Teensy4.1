// FX.h - Base class and types for TSynth effects framework
// MIT License - see LICENSE.md
#pragma once

// Enum for available FX types
enum class FXType {
  Off = 0,
  Chorus,
  Delay
};

// Virtual base class for all effects
class FX {
public:
  virtual ~FX() {}
  
  // Process stereo audio buffer
  // buf: interleaved stereo buffer (L, R, L, R, ...)
  // numFrames: number of stereo frames
  virtual void process(float* buf, int numFrames) = 0;
  
  // Set effect parameters (0.0 to 1.0 range)
  virtual void setAmount(float amount) = 0;  // Maps to pot 1
  virtual void setMix(float mix) = 0;        // Maps to pot 2
  
  // Initialize with sample rate
  virtual void init(float sampleRate) = 0;
};
