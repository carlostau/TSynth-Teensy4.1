#include "Delay.h"
#include <cstring>
#include <algorithm>

Delay::Delay(float sampleRate)
  : sampleRate_(sampleRate), maxSamples_(0), bufferL_(nullptr), bufferR_(nullptr),
    writeIndex_(0), delaySamples_(0), feedback_(0.35f), mix_(0.35f) {
  const float maxMs = 1500.0f; // 1.5s max
  maxSamples_ = static_cast<int>((maxMs / 1000.0f) * sampleRate_) + 4;
  bufferL_ = new float[maxSamples_];
  bufferR_ = new float[maxSamples_];
  std::memset(bufferL_, 0, sizeof(float) * maxSamples_);
  std::memset(bufferR_, 0, sizeof(float) * maxSamples_);
  writeIndex_ = 0;
  setDelayMs(400.0f);
}

Delay::~Delay() {
  if (bufferL_) delete[] bufferL_;
  if (bufferR_) delete[] bufferR_;
}

void Delay::setDelayMs(float ms) {
  int d = static_cast<int>((ms / 1000.0f) * sampleRate_);
  if (d < 0) d = 0;
  if (d >= maxSamples_) d = maxSamples_ - 1;
  delaySamples_ = d;
}

void Delay::setAmount(float v) {
  v = std::clamp(v, 0.0f, 1.0f);
  feedback_ = 0.0f + v * 0.95f;
}

void Delay::setMix(float v) {
  mix_ = std::clamp(v, 0.0f, 1.0f);
}

void Delay::process(float *buf, int numFrames) {
  for (int i = 0; i < numFrames; ++i) {
    float inL = buf[2*i + 0];
    float inR = buf[2*i + 1];

    int readIndex = writeIndex_ - delaySamples_;
    if (readIndex < 0) readIndex += maxSamples_;

    float delayL = bufferL_[readIndex];
    float delayR = bufferR_[readIndex];

    bufferL_[writeIndex_] = inL + delayL * feedback_;
    bufferR_[writeIndex_] = inR + delayR * feedback_;

    buf[2*i + 0] = inL * (1.0f - mix_) + delayL * mix_;
    buf[2*i + 1] = inR * (1.0f - mix_) + delayR * mix_;

    writeIndex_++;
    if (writeIndex_ >= maxSamples_) writeIndex_ = 0;
  }
}
