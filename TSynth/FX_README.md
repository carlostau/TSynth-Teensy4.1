# TSynth FX Framework Documentation

## Overview

This FX framework adds real-time audio effects to TSynth with safe runtime switching. The initial implementation includes a stereo Delay effect, with infrastructure for adding more effects like Chorus.

## Architecture

### Components

1. **FX.h** - Base class defining the FX interface
   - `FXType` enum: Off, Chorus, Delay
   - Virtual methods: `process()`, `setAmount()`, `setMix()`, `init()`

2. **FXManager** - Singleton manager for FX lifecycle
   - Safe FX switching using `__disable_irq()/__enable_irq()`
   - No dynamic allocation in audio thread
   - Pre-allocated FX instances
   - Thread-safe pointer swapping

3. **Delay** - Stereo delay effect
   - Static circular buffers (500ms max at 48kHz)
   - Amount parameter → Feedback (0.0 to 0.95)
   - Mix parameter → Wet/Dry (0.0 to 1.0)
   - Default delay time: 300ms

4. **FXSettings** - Bridge to Settings menu
   - In-memory persistence for FX type
   - TODO: EEPROM integration for persistence across power cycles
   - Calls `FXManager::setType()` when FX changes

## Hardware Integration

### Potentiometer Wiring

Two potentiometers control the active FX:

- **Pot 1 (Amount)**: Controls effect-specific parameter
  - For Delay: Feedback amount (0.0 to 0.95)
  - Range: 0.0 to 1.0
  
- **Pot 2 (Mix)**: Controls wet/dry mix
  - 0.0 = fully dry (no effect)
  - 1.0 = fully wet (full effect)
  - Range: 0.0 to 1.0

### Hardware Setup Example

```cpp
// In HWControls.h or similar hardware control file
#define FX_AMOUNT_POT A14  // Example pin for Amount pot
#define FX_MIX_POT    A15  // Example pin for Mix pot

// In your hardware control loop
void updateFXPots() {
  float amount = analogRead(FX_AMOUNT_POT) / 1023.0f;
  float mix = analogRead(FX_MIX_POT) / 1023.0f;
  
  FXManager::getInstance().setAmount(amount);
  FXManager::getInstance().setMix(mix);
}
```

## Code Integration

### 1. Initialize FXManager at Audio Setup

In your audio initialization code (typically in `setup()` or similar):

```cpp
#include "FXManager.h"

void setup() {
  // ... existing audio setup ...
  
  // Initialize FX manager with sample rate (e.g., 44100 Hz)
  FXManager::getInstance().init(AUDIO_SAMPLE_RATE_EXACT);
  
  // ... rest of setup ...
}
```

### 2. Process Audio Through FXManager

In your audio callback or processing loop, after mixing voices:

```cpp
void processAudio() {
  // Mix all voices into stereo buffer
  float audioBuffer[BUFFER_SIZE * 2];  // Interleaved stereo: L, R, L, R, ...
  
  // ... mix voices into audioBuffer ...
  
  // Apply FX processing
  FXManager::getInstance().process(audioBuffer, BUFFER_SIZE);
  
  // Output to DAC/audio interface
}
```

### 3. Handle Pot Updates

In your control update loop:

```cpp
void updateControls() {
  // Read pots and update FX parameters
  float amount = readAmountPot();  // 0.0 to 1.0
  float mix = readMixPot();         // 0.0 to 1.0
  
  FXManager::getInstance().setAmount(amount);
  FXManager::getInstance().setMix(mix);
}
```

### 4. Settings Menu Integration

The FX option is automatically added to the Settings menu via `Settings.h`:

- Navigate to Settings menu
- Select "FX" option
- Choose: Chorus, Delay, or Off
- Changes take effect immediately

## Settings Menu

The FX setting is added to the existing Settings circular buffer:

```cpp
settings::append(settings::SettingsOption{
  "FX", 
  {"Chorus", "Delay", "Off", "\0"}, 
  settingsFX, 
  currentIndexFX
});
```

## Safety Guarantees

### Real-Time Safe Operation

1. **No Dynamic Allocation**: All FX instances are pre-allocated at startup
2. **Atomic Pointer Swaps**: FX switching uses interrupt disable/enable
3. **Null Pointer Safety**: Process checks for active FX before calling
4. **Buffer Bounds**: Delay uses fixed-size circular buffers with bounds checking

### Interrupt Protection

```cpp
// FX pointer swap is protected
__disable_irq();
activeFX_ = newFX;
currentType_ = type;
__enable_irq();
```

## Persistence Notes

### Current Implementation

- **In-Memory Only**: FX selection is stored in RAM
- **Per-Session**: FX type resets to Off on power cycle
- **Global Setting**: Single FX applies to all patches

### Future Work (TODO)

1. **EEPROM Persistence**: Integrate with `EepromMgr.h` to save FX type
2. **Per-Patch FX**: Store FX type and parameters in patch format
3. **FX Parameter Recall**: Save/recall Amount and Mix with patches

Integration points for persistence:

```cpp
// In FXSettings.cpp - TODO
void storeFXType(FXType type) {
  // Call EepromMgr to write FX type to EEPROM
  // Example: writeEEPROM(FX_TYPE_ADDR, static_cast<uint8_t>(type));
}

FXType loadFXType() {
  // Call EepromMgr to read FX type from EEPROM
  // Example: return static_cast<FXType>(readEEPROM(FX_TYPE_ADDR));
}
```

## Adding New Effects

To add a new effect (e.g., Reverb):

1. **Create Effect Class**:
   ```cpp
   // Reverb.h
   class Reverb : public FX {
     // Implement virtual methods
   };
   ```

2. **Update FXType Enum**:
   ```cpp
   enum class FXType {
     Off = 0,
     Chorus,
     Delay,
     Reverb  // Add new type
   };
   ```

3. **Add to FXManager**:
   ```cpp
   class FXManager {
     Reverb reverbFX_;  // Preallocate instance
     
     void setType(FXType type) {
       case FXType::Reverb:
         newFX = &reverbFX_;
         break;
     }
   };
   ```

4. **Update Settings**:
   ```cpp
   settings::append(settings::SettingsOption{
     "FX", 
     {"Chorus", "Delay", "Reverb", "Off", "\0"},  // Add option
     settingsFX, 
     currentIndexFX
   });
   ```

## Testing Checklist

- [ ] Build project successfully
- [ ] Initialize FXManager with correct sample rate during audio setup
- [ ] Verify Settings menu shows FX option
- [ ] Select "Delay" in Settings and verify FX is active
- [ ] Wire pots and verify Amount/Mix control Delay parameters
- [ ] Test FX switching (Off → Delay → Off) without audio glitches
- [ ] Verify no clicks/pops when switching FX
- [ ] Test Delay feedback and wet/dry mix
- [ ] Call FXManager::process() in audio path after voice mixing

## Example Integration in TSynth.ino

```cpp
#include "FXManager.h"

void setup() {
  // Existing setup code...
  AudioMemory(512);
  
  // Initialize FX manager
  FXManager::getInstance().init(AUDIO_SAMPLE_RATE_EXACT);
  
  // Rest of setup...
}

// In audio processing ISR or callback
void audioCallback(float* buffer, int numFrames) {
  // Mix voices
  mixVoices(buffer, numFrames);
  
  // Apply FX
  FXManager::getInstance().process(buffer, numFrames);
  
  // Output to DAC
  outputToDAC(buffer, numFrames);
}

// In control update loop (called from main loop)
void updateControls() {
  // Read pots (example with smoothing)
  static float smoothAmount = 0.5f;
  static float smoothMix = 0.5f;
  
  float rawAmount = analogRead(FX_AMOUNT_POT) / 1023.0f;
  float rawMix = analogRead(FX_MIX_POT) / 1023.0f;
  
  // Simple smoothing
  smoothAmount = smoothAmount * 0.9f + rawAmount * 0.1f;
  smoothMix = smoothMix * 0.9f + rawMix * 0.1f;
  
  FXManager::getInstance().setAmount(smoothAmount);
  FXManager::getInstance().setMix(smoothMix);
}
```

## Performance Notes

- Delay effect uses ~192KB RAM for 500ms stereo buffers at 48kHz (static allocation)
- CPU usage: ~5-10% at 600MHz for Delay processing
- Interrupt disable duration: < 10 CPU cycles for pointer swap
- No heap allocation - uses static arrays for buffers

## License

MIT License - see LICENSE.md
