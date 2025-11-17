#pragma once
#include "FX.h"

class FXManager {
public:
  static void init(float sampleRate);
  static void setType(FXType t);
  static FXType getType();
  static void process(float *buf, int numFrames);
  static void setAmount(float v);
  static void setMix(float v);
  static void shutdown();
private:
  FXManager() = delete;
};
