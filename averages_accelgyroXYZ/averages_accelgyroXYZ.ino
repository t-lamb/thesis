#include<Wire.h>

const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

int sampleWindow = 50;
int delayval = 50;

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
}
void loop() {

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
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

  float averageAcX = totalDiffAcX / sampleCount;
  float averageAcY = totalDiffAcY / sampleCount;
  float averageAcZ = totalDiffAcZ / sampleCount;
  float averageGyX = totalDiffGyX / sampleCount;
  float averageGyY = totalDiffGyY / sampleCount;
  float averageGyZ = totalDiffGyZ / sampleCount;
  
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
  delay(delayval);

}
