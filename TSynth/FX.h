#pragma once
#include <stdint.h>

enum FXType : uint8_t {
  FX_CHORUS = 0,
  FX_DELAY  = 1,
  FX_OFF    = 2,
};

class FX {
public:
  virtual ~FX() {}
  // Process interleaved stereo buffer (inPlace) - samples are float [-1..1]
  // numFrames: number of frames (not samples), buffer layout: L,R,L,R...
  virtual void process(float *buf, int numFrames) = 0;

  // two-pot interface (0..1)
  virtual void setAmount(float v) { (void)v; } // pot A ("Amount")
  virtual void setMix(float v)    { (void)v; } // pot B ("Mix")
};
