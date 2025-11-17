// Delay.h - Stereo delay effect with feedback
// MIT License - see LICENSE.md
#pragma once

#include "FX.h"

class Delay : public FX {
public:
  Delay();
  ~Delay();
  
  void init(float sampleRate) override;
  void process(float* buf, int numFrames) override;
  void setAmount(float amount) override;  // Controls feedback (0.0 to 0.95)
  void setMix(float mix) override;        // Controls wet/dry mix (0.0 to 1.0)
  
  // Set delay time in milliseconds (default 300ms)
  void setDelayMs(float delayMs);
  
private:
  static const int MAX_DELAY_SAMPLES = 24000;  // 500ms at 48kHz (~192KB total)
  
  float delayBufferL[MAX_DELAY_SAMPLES];
  float delayBufferR[MAX_DELAY_SAMPLES];
  int writeIndex;
  int delayLengthSamples;
  float sampleRate_;
  float feedback_;  // 0.0 to 0.95
  float mix_;       // 0.0 to 1.0 (0=dry, 1=wet)
  float delayMs_;
};
