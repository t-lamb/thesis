// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

int sampleWindow = 50;
int delayval = 50;

#define OUTPUT_READABLE_ACCELGYRO
//#define OUTPUT_BINARY_ACCELGYRO


#define LED_PIN 13
bool blinkState = false;

void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // initialize serial communication
  // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
  // it's really up to you depending on your project)
  Serial.begin(38400);

  //    // initialize device
  //    Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  //    // verify connection
  //    Serial.println("Testing device connections...");
  //    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // configure Arduino LED for
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
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

//  #ifdef OUTPUT_READABLE_ACCELGYRO
//  Serial.print(totalDiffAcX);
//  Serial.print(" ");
//  Serial.print(totalDiffAcY);
//  Serial.print(" ");
//  Serial.print(totalDiffAcZ);
//  Serial.print(" ");
//  Serial.print(totalDiffGyX);
//  Serial.print(" ");
//  Serial.print(totalDiffGyY);
//  Serial.print(" ");
//  Serial.println(totalDiffGyZ);
//  //delay(delayval);
//  #endif
  
  #ifdef OUTPUT_READABLE_ACCELGYRO
  Serial.print(averageAcX);
  Serial.print(" ");
  Serial.print(averageAcY);
  Serial.print(" ");
  Serial.print(averageAcZ);
  Serial.print(" ");
  Serial.print(averageGyX);
  Serial.print(" ");
  Serial.print(averageGyY);
  Serial.print(" ");
  Serial.println(averageGyZ);
  //delay(delayval);
  #endif

//Serial.println(sampleCount);

//#ifdef OUTPUT_READABLE_ACCELGYRO
//  Serial.print(AcX);
//  Serial.print(" ");
//  Serial.print(AcY);
//  Serial.print(" ");
//  Serial.print(AcZ);
//  Serial.print(" ");
//  Serial.print(GyX);
//  Serial.print(" ");
//  Serial.print(GyY);
//  Serial.print(" ");
//  Serial.println(GyZ);
//#endif

  //    #ifdef OUTPUT_BINARY_ACCELGYRO
  //        Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
  //        Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
  //        Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
  //        Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
  //        Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
  //        Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
  //    #endif

  // blink LED to indicate activity
  blinkState = !blinkState;
  digitalWrite(LED_PIN, blinkState);
}
