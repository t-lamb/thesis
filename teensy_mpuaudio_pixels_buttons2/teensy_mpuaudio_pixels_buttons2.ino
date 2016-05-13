//teensy audio
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

float amp1 = 0.0;
float amp2 = 0.0;
float amp3 = 0.0;

//mpu6050
#include "I2Cdev.h"
#include "MPU6050.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif
MPU6050 accelgyro;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int sampleWindow = 50;
int delayval = 200;

//buttons
#define BUTTON_PIN0   0
#define BUTTON_PIN1   1
#define BUTTON_PIN2   2

int buttonState0 = 1;
int buttonState1 = 1;
int buttonState2 = 1;

bool pressed0 = false;
bool pressed1 = false;
bool pressed2 = false;
bool newSound0 = false;
bool newSound1 = false;
bool newSound2 = false;

float freqSine1 = 311.13; //E flat
float freqSine1New = 155.56;
float freqSine2 = 392.0; //G
float freqSine2New = 196.00;
float freqSine3 = 466.16; //A sharp
float freqSine3New = 233.08;

//neopixels
#include <Adafruit_NeoPixel.h>
#define PIXEL_PIN    20    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 7
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool firstTime = true;

void setup() {
  pinMode(BUTTON_PIN0, INPUT_PULLUP);
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);

  strip.begin();
  strip.setPixelColor(0, strip.Color(10, 10, 50));
  strip.setPixelColor(1, strip.Color(10, 10, 50));
  strip.setPixelColor(2, strip.Color(10, 10, 50));
  strip.show(); // Initialize all pixels to 'off'

  //mpu6050
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif
  accelgyro.initialize();

  //teensy audio
  Serial.begin(9600);
  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.32);

  sine1.frequency(freqSine1);
  sine2.frequency(freqSine2);
  sine3.frequency(freqSine3);
  sine1.amplitude(amp1);
  sine2.amplitude(amp2);
  sine3.amplitude(amp3);

  mixer1.gain(0, 1.0);
  mixer1.gain(1, 1.0);
  mixer1.gain(2, 1.0);
}

void loop() {
  //  if (firstTime) {
  //    !firstTime;
  //  }

  // read button states
  buttonState0 = digitalRead(BUTTON_PIN0);
  buttonState1 = digitalRead(BUTTON_PIN1);
  buttonState2 = digitalRead(BUTTON_PIN2);

  // read raw accel/gyro measurements from device
  accelgyro.getMotion6(&AcX, &AcY, &AcZ, &GyX, &GyY, &GyZ);
  unsigned long startMillis = millis();
  int prevReadingAcX = analogRead(AcX);
  int prevReadingAcY = analogRead(AcY);
  int prevReadingAcZ = analogRead(AcZ);
  int prevReadingGyX = analogRead(GyX);
  int prevReadingGyY = analogRead(GyY);
  int prevReadingGyZ = analogRead(GyZ);
  int sampleCount = 0;

  unsigned long totalDiffAcX = 0;
  unsigned long totalDiffAcY = 0;
  unsigned long totalDiffAcZ = 0;
  unsigned long totalDiffGyX = 0;
  unsigned long totalDiffGyY = 0;
  unsigned long totalDiffGyZ = 0;

  while (millis() - startMillis < sampleWindow) {
    int newReadingAcX = AcX;
    int newReadingAcY = AcY;
    int newReadingAcZ = AcZ;
    int newReadingGyX = GyX;
    int newReadingGyY = GyY;
    int newReadingGyZ = GyZ;

    // find absolute difference
    int differenceAcX = abs(prevReadingAcX - newReadingAcX);
    int differenceAcY = abs(prevReadingAcY - newReadingAcY);
    int differenceAcZ = abs(prevReadingAcZ - newReadingAcZ);
    int differenceGyX = abs(prevReadingGyX - newReadingGyX);
    int differenceGyY = abs(prevReadingGyY - newReadingGyY);
    int differenceGyZ = abs(prevReadingGyZ - newReadingGyZ);

    //add difference to total difference
    totalDiffAcX += differenceAcX;
    totalDiffAcY += differenceAcY;
    totalDiffAcZ += differenceAcZ;
    totalDiffGyX += differenceGyX;
    totalDiffGyY += differenceGyY;
    totalDiffGyZ += differenceGyZ;

    // save reading for next loop
    prevReadingAcX = newReadingAcX;
    prevReadingAcY = newReadingAcY;
    prevReadingAcZ = newReadingAcZ;
    prevReadingGyX = newReadingGyX;
    prevReadingGyY = newReadingGyY;
    prevReadingGyZ = newReadingGyZ;

    sampleCount++;
  }

  float averageAcX = (float) totalDiffAcX / sampleCount;
  float averageAcY = (float) totalDiffAcY / sampleCount;
  float averageAcZ = (float) totalDiffAcZ / sampleCount;
  float averageGyX = (float) totalDiffGyX / sampleCount;
  float averageGyY = (float) totalDiffGyY / sampleCount;
  float averageGyZ = (float) totalDiffGyZ / sampleCount;

  //    Serial.print(averageAcX);
  //    Serial.print(" ");
  //    Serial.print(averageAcY);
  //    Serial.print(" ");
  //    Serial.print(averageAcZ);
  //    Serial.print(" ");
  Serial.print(averageGyX);
  Serial.print(" ");
  Serial.print(averageGyY);
  Serial.print(" ");
  Serial.println(averageGyZ);

  amp1 = averageGyX * 4.0;
  amp2 = averageGyY * 4.0;
  amp3 = averageGyZ * 4.0;

  //  Serial.print(amp1);
  //  Serial.print(" ");
  //  Serial.print(amp2);
  //  Serial.print(" ");
  //  Serial.println(amp3);

  sine1.amplitude(amp1);
  sine2.amplitude(amp2);
  sine3.amplitude(amp3);

  //check button states
  if (buttonState0 == LOW) {
    pressed0 = true;
  }
  if (buttonState1 == LOW) {
    pressed1 = true;
  }
  if (buttonState2 == LOW) {
    pressed2 = true;
  }

  if (pressed0) {
    if (buttonState0 == LOW) {
      //still pressed
      pressed0 = true;
    }
    else {
      //change when released
      strip.setPixelColor(0, strip.Color(50, 50, 200));
      strip.show();
      !pressed0;
      //toggle between sounds
      if (!newSound0) {
        sine1.frequency(freqSine1New);
        newSound0;
      } else {
        sine1.frequency(freqSine1);
        !newSound0;
      }
    }
  }
  if (pressed1) {
    //still pressed
    if (buttonState1 == LOW) {
      pressed1 = true;
    }
    else {
      //change when released
      strip.setPixelColor(1, strip.Color(50, 50, 200));
      strip.show();
      !pressed1;
      //toggle between sounds
      if (!newSound1) {
        sine2.frequency(freqSine2New);
        newSound1;
      } else {
        sine2.frequency(freqSine2);
        !newSound1;
      }
    }
  }
  if (pressed2) {
    //still pressed
    if (buttonState2 == LOW) {
      pressed2 = true;
    }
    else {
      //change when released
      sine3.frequency(freqSine3New);
      strip.setPixelColor(2, strip.Color(50, 50, 200));
      strip.show();
      !pressed2;
      //toggle between sounds
      if (!newSound2) {
        sine3.frequency(freqSine3New);
        newSound2;
      } else {
        sine3.frequency(freqSine3);
        !newSound2;
      }
    }
  }

  int MoveX = averageGyX * 100;
  //  Serial.println(MoveX);

  for (int i = 0; i < MoveX; i++) {
    strip.setPixelColor(i + 3, strip.Color(10, 10, 50));
    strip.show();
  }

  delay(delayval);

  for (int i = 3; i < PIXEL_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
  }
}

