#pragma once
#include "FX.h"

class Delay : public FX {
public:
  Delay(float sampleRate = 44100.0f);
  ~Delay();

  void setDelayMs(float ms);
  void setAmount(float v) override; // pot A -> feedback
  void setMix(float v) override;    // pot B -> wet/dry mix
  void process(float *buf, int numFrames) override;

private:
  float sampleRate_;
  int maxSamples_;
  float *bufferL_;
  float *bufferR_;
  int writeIndex_;
  int delaySamples_;
  float feedback_;
  float mix_;
};
