#ifndef Oscilloscope_h_
#define Oscilloscope_h_
#include "AudioStream.h"
#include "ST7735_t3.h"
#include <Arduino.h>

// NOTE: original file used a few globals at top. Keep similar layout but
// add a few static flags to avoid multiple-definition across translation units.
uint8_t bufferBlock = 0;
uint8_t bufcount = 0;
uint8_t pixel_x = 0;
int16_t pixel_y = 0;
int16_t prev_pixel_y = 0;

// visibility / activity helpers (static to avoid multiple-def symbols)
static bool osc_visible = false;
static uint32_t osc_last_active = 0;
static uint16_t osc_hold_ms = 250;         // milliseconds to keep the osc visible after last activity
static int32_t osc_activity_threshold = 500; // amplitude threshold to consider "sound" active

class Oscilloscope : public AudioStream {
  public:
    Oscilloscope(void) : AudioStream(1, inputQueueArray) {
    }
    virtual void update(void);
    void ScreenSetup(ST7735_t3*);
    void Display(void);
    void AddtoBuffer(int16_t*);
    // controls to tweak detection if needed
    void SetActivityThreshold(int32_t t) { osc_activity_threshold = t; }
    void SetHoldTime(uint16_t ms) { osc_hold_ms = ms; }
    bool IsVisible() const { return osc_visible; }

  private:
    audio_block_t *inputQueueArray[1];
    ST7735_t3 *display = nullptr;
    int16_t buffer[AUDIO_BLOCK_SAMPLES];

    // clear the osc drawing area when not visible
    void ClearDisplayArea() {
      if (!display) return;
      // waveform is drawn between y=30..100 and starts at x=15
      // width = AUDIO_BLOCK_SAMPLES + small margin
      uint16_t x = 15;
      uint16_t y = 30;
      uint16_t w = AUDIO_BLOCK_SAMPLES + 2;
      uint16_t h = (100 - 30) + 1;
      display->fillRect(x, y, w, h, 0x0000); // black background
    }

    // check if the current buffer contains activity above threshold
    bool BufferHasActivity() {
      int32_t maxv = 0;
      for (uint16_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int32_t v = buffer[i];
        if (v < 0) v = -v;
        if (v > maxv) maxv = v;
      }
      return maxv >= osc_activity_threshold;
    }
};
#endif

void Oscilloscope::ScreenSetup(ST7735_t3 *screen) {
  display = screen;
}

void Oscilloscope::Display() {
  pixel_x = 0;
  prev_pixel_y = map(buffer[0], 32767, -32768, -120, 120) + 63;
  if (prev_pixel_y < 30) prev_pixel_y = 30;
  if (prev_pixel_y > 100)prev_pixel_y = 100;

  for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES - 1; i++) {
    pixel_y = map(buffer[i], 32767, -32768, -120, 120) + 63;
    if (pixel_y < 30) pixel_y = 30;
    if (pixel_y > 100)pixel_y = 100;
    display->drawLine(pixel_x + 15, prev_pixel_y, pixel_x + 16, pixel_y, 0x07B0);
    prev_pixel_y = pixel_y;
    pixel_x++;
  }
}

void Oscilloscope::AddtoBuffer(int16_t *audio) {
  audio++;
  if (bufferBlock == 0) {
    if (*(audio - 1) > -16 && *(audio + 3) < 16) {
      bufferBlock = 1;
      bufcount = 0;
    }
  }
  else {
    for (uint16_t i = 0; i < 32; i++) {
      buffer[bufcount++] = *audio;
      audio += 4;
    }
    bufferBlock++;
    if (bufferBlock >= 5) {
      bufferBlock = 0;
    }
  }
}

void Oscilloscope::update(void) {
  if (!display) return;
  audio_block_t *block;
  block = receiveReadOnly(0);
  if (block) {
    AddtoBuffer(block->data);
    release(block);
    // when bufferBlock wraps to 0, buffer[] holds the assembled samples and is ready to display
    if (bufferBlock == 0) {
      // check activity in the assembled buffer
      if (BufferHasActivity()) {
        osc_last_active = millis();
        if (!osc_visible) {
          // became visible now -> clear area first to avoid remnants
          ClearDisplayArea();
        }
        osc_visible = true;
      } else {
        // no immediate activity; if past hold time, hide
        if (osc_visible && (millis() - osc_last_active) > osc_hold_ms) {
          osc_visible = false;
        }
      }

      if (osc_visible) {
        Display();
      } else {
        // ensure area is cleared while not visible so it doesn't overlap UI
        ClearDisplayArea();
      }
    }
  }
}
