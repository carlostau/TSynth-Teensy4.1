# FX: Delay + Wiring Notes

This PR adds a small FX framework and a Delay effect controlled by two pots: Amount and Mix.

Wiring (pots):
- Pot A -> FX Amount (maps 0..1 to feedback 0..0.95)
- Pot B -> FX Mix (maps 0..1 to wet/dry mix)

How to integrate in code:
- Initialize manager once (e.g. in setup/audio init):
  FXManager::init(sampleRate);
- In your audio processing path, after mixing voices into the output buffer and before DAC/clipping, call:
  FXManager::process(mixBuffer, numFrames);
  // mixBuffer: interleaved stereo float L,R,...
- Read pots (ADC) and call:
  FXManager::setAmount(potA_normalized); // 0.0 .. 1.0
  FXManager::setMix(potB_normalized);    // 0.0 .. 1.0
- Settings menu includes an "FX" option to pick Chorus/Delay/Off. When changed via Settings, FXManager will be switched automatically.

Persistence:
- FX type is stored in EEPROM at address EEPROM_FX_TYPE (13) and persists across restarts.
- The FX type is automatically loaded from EEPROM on first access via getGlobalFXType().
