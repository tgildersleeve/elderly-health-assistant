/* this program senses whether a person has fallen
 * product concept: 
 * a small device containing an accelerometer, which you can clip to your chest
 * device records average x, y and z accelerations 
 * if there is a rapid acceleration change, a fall is recorded
 */

#include <Wire.h>
#include <SparkFun_MMA8452Q.h>

#define ACC_FILTER_COUNTS 10
#define FILTER_2 50

float accsZ[ACC_FILTER_COUNTS];
int countAccZ = 0;
float accsY[ACC_FILTER_COUNTS];
int countAccY = 0;
float accsX[ACC_FILTER_COUNTS];
int countAccX = 0;

float totZ[FILTER_2];
int countTotZ = 0;
float totY[FILTER_2];
int countTotY = 0;
float totX[FILTER_2];
int countTotX = 0;

unsigned long countPasses;

float avgAccZ, avgAccY, avgAccX;
float avgTotZ, avgTotY, avgTotX;
float threshZPeak, threshZValley, threshYPeak, threshYValley, threshXPeak, threshXValley;
bool fallX1, fallX2, fallY1, fallY2, fallZ1, fallZ2;

float tryDiff = 0.5;

int delta = 20;
unsigned long timeCount = 0;

MMA8452Q accel;

// regardless of orientation, when a person falls
// there occurs a sort of "heart beat" pattern in one coordinate value's acceleration,
// where there is a peak well above the average (x or y or z) value and a valley well below the average.
// whether the peak or valley occurrs first depends on the orientation.

void setup()
{
  Serial.begin(9600);
  accel.init();
}


void loop()
{
  if (millis() > timeCount) {
    runProg();
    timeCount += delta;
  }
}

void runProg() {
  if (accel.available())
  {
    accel.read();
    updateAllAvgs();
    updateAllTots();
    threshold();
    setFallChecks();
    fallOccurred();
  }
}

void updateAllAvgs() {
  float accZ = accel.cz * 1000;
  accsZ[countAccZ % ACC_FILTER_COUNTS] = accZ;
  countAccZ += 1;

  float sumAccZ = 0;
  for (int i = 0; i < 10; i++) {
    sumAccZ += accsZ[i];
  }
  avgAccZ = (sumAccZ / 10) / 1000;

  float accY = accel.cy * 1000;
  accsY[countAccY % ACC_FILTER_COUNTS] = accY;
  countAccY += 1;

  float sumAccY = 0;
  for (int i = 0; i < 10; i++) {
    sumAccY += accsY[i];
  }
  avgAccY = (sumAccY / 10) / 1000;

  float accX = accel.cx * 1000;
  accsX[countAccX % ACC_FILTER_COUNTS] = accX;
  countAccX += 1;

  float sumAccX = 0;
  for (int i = 0; i < 10; i++) {
    sumAccX += accsX[i];
  }
  avgAccX = (sumAccX / 10) / 1000;
}

void updateAllTots() {
  float totZA = accel.cz;
  totZ[countTotZ % FILTER_2] = totZA;
  countTotZ += 1;

  float sumTotZ = 0;
  for (int i = 0; i < FILTER_2; i++) {
    sumTotZ += totZ[i];
  }
  avgTotZ = (sumTotZ / FILTER_2);

  float totYA = accel.cy;
  totY[countTotY % FILTER_2] = totYA;
  countTotY += 1;

  float sumTotY = 0;
  for (int i = 0; i < FILTER_2; i++) {
    sumTotY += totY[i];
  }
  avgTotY = (sumTotY / FILTER_2);

  float totXA = accel.cx;
  totX[countTotX % FILTER_2] = totXA;
  countTotX += 1;

  float sumTotX = 0;
  for (int i = 0; i < FILTER_2; i++) {
    sumTotX += totX[i];
  }
  avgTotX = (sumTotX / FILTER_2);
}


void threshold(){
    threshZPeak = avgTotZ + tryDiff;
    threshZValley = avgTotZ - tryDiff;
    threshYPeak = avgTotY + tryDiff;
    threshYValley = avgTotY - tryDiff;
    threshXPeak = avgTotX + tryDiff;
    threshXValley = avgTotX - tryDiff;
}


bool findZPeak() { return (avgAccZ > threshZPeak); }
bool findZValley() { return (avgAccZ < threshZValley); }
bool findYPeak() { return (avgAccY > threshYPeak); }
bool findYValley() { return (avgAccY < threshYValley); }
bool findXPeak() { return (avgAccX > threshXPeak); }
bool findXValley() { return (avgAccX < threshXValley); }

void setFallChecks() {
  if (findXPeak()) { fallX1 = true; }
  if (findXValley()) { fallX2 = true; }
  if (findYPeak()) { fallY1 = true; }
  if (findYValley()) { fallY2 = true; }
  if (findZPeak()) { fallZ1 = true; }
  if (findZValley()) { fallZ2 = true; }
}


void fallOccurred() {
  if ((fallX1 && fallX2) || (fallY1 && fallY2) || (fallZ1 && fallZ2)) {
    Serial.write("fall");
    fallX1 = false; fallX2 = false; fallY1 = false; fallY2 = false; fallZ1 = false; fallZ2 = false;
  }
}
