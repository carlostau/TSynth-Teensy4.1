// Delay.cpp - Stereo delay effect implementation
// MIT License - see LICENSE.md

#include "Delay.h"
#include <cstring>
#include <algorithm>

Delay::Delay() 
  : writeIndex(0)
  , delayLengthSamples(0)
  , sampleRate_(44100.0f)
  , feedback_(0.5f)
  , mix_(0.5f)
  , delayMs_(300.0f)
{
  // Initialize buffers to silence (static arrays, no heap allocation)
  std::memset(delayBufferL, 0, sizeof(delayBufferL));
  std::memset(delayBufferR, 0, sizeof(delayBufferR));
}

Delay::~Delay() {
  // No dynamic memory to free
}

void Delay::init(float sampleRate) {
  sampleRate_ = sampleRate;
  setDelayMs(delayMs_);  // Recalculate delay length
}

void Delay::setDelayMs(float delayMs) {
  delayMs_ = delayMs;
  delayLengthSamples = static_cast<int>((delayMs_ / 1000.0f) * sampleRate_);
  // Clamp to buffer size
  if (delayLengthSamples > MAX_DELAY_SAMPLES) {
    delayLengthSamples = MAX_DELAY_SAMPLES;
  }
  if (delayLengthSamples < 1) {
    delayLengthSamples = 1;
  }
}

void Delay::setAmount(float amount) {
  // Map amount (0.0 to 1.0) to feedback (0.0 to 0.95)
  feedback_ = amount * 0.95f;
}

void Delay::setMix(float mix) {
  // Mix: 0.0 = fully dry, 1.0 = fully wet
  mix_ = std::max(0.0f, std::min(1.0f, mix));
}

void Delay::process(float* buf, int numFrames) {
  if (delayLengthSamples < 1) {
    return;  // Safety check
  }
  
  for (int i = 0; i < numFrames; i++) {
    // Interleaved stereo: L, R, L, R, ...
    float inL = buf[i * 2];
    float inR = buf[i * 2 + 1];
    
    // Calculate read index (circular buffer)
    int readIndex = writeIndex - delayLengthSamples;
    if (readIndex < 0) {
      readIndex += MAX_DELAY_SAMPLES;
    }
    
    // Read delayed samples
    float delayedL = delayBufferL[readIndex];
    float delayedR = delayBufferR[readIndex];
    
    // Write to delay buffer with feedback
    delayBufferL[writeIndex] = inL + delayedL * feedback_;
    delayBufferR[writeIndex] = inR + delayedR * feedback_;
    
    // Mix dry and wet signals
    float outL = inL * (1.0f - mix_) + delayedL * mix_;
    float outR = inR * (1.0f - mix_) + delayedR * mix_;
    
    // Write output
    buf[i * 2] = outL;
    buf[i * 2 + 1] = outR;
    
    // Advance write index (circular)
    writeIndex++;
    if (writeIndex >= MAX_DELAY_SAMPLES) {
      writeIndex = 0;
    }
  }
}
