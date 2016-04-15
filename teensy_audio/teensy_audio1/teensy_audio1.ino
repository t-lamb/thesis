#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=106,176
AudioSynthWaveformSine   sine2;          //xy=136,240
AudioSynthWaveformSine   sine3;          //xy=169,292
AudioMixer4              mixer1;         //xy=325,98
AudioOutputI2S           i2s1;           //xy=528,101
AudioConnection          patchCord1(sine1, 0, mixer1, 0);
AudioConnection          patchCord2(sine2, 0, mixer1, 1);
AudioConnection          patchCord3(sine3, 0, mixer1, 2);
AudioConnection          patchCord4(mixer1, 0, i2s1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=367,287
// GUItool: end automatically generated code

void setup() {
  Serial.begin(9600);
  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.32);

  sine1.frequency(311.13);
  sine2.frequency(392.0);
  sine3.frequency(466.16);
  sine1.amplitude(.5);
  sine2.amplitude(.5);
  sine3.amplitude(.5);

  mixer1.gain(0, 1.0);
  mixer1.gain(1, 1.0);
  mixer1.gain(2, 1.0);
}
void loop() {

}

