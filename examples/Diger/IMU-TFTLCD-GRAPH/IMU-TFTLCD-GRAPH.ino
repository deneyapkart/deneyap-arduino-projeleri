/*  IMU-TFTLCD-GRAPH
 *  T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *
 *  Bu uygulamada 1 Deneyap Geliştirme Kartı ve 1 TFTLCD Ekran kullanılmıştır. 
 *  Deneyap Kartta dahili 6-Eksen Ataletsel Ölçüm Birimi (IMU)dan alınan dönme eksen açılarını Roll, Pitch, Yaw değerlerini okumaktadır. 
 *  Okunan sensör verilerini TFTLCD ekranında grafiksel olarak göstermektedir.
 *
 *  ==============================================================================
 *    Bu uygulama örneği için "Adafruit ST7735 and ST7789 Library by Adafruit" kütüphanesi indirilmelidir.  ->https://github.com/adafruit/Adafruit-ST7735-Library<-
 *  ==============================================================================
*/
#include "lsm6dsm.h"
#include <Adafruit_ST7735.h>

#define TFT_CS D0
#define TFT_RST D4
#define TFT_DC D1

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int valueBlockRoll[250];
int valueBlockPitch[250];
int valueBlockYaw[250];
int timeBlock[250];
int locationBlockRoll[100];
int locationBlockPitch[100];
int locationBlockYaw[100];
int valuePosRoll;
int valuePosPitch;
int valuePosYaw;
int blockPos;

// Editable Variables
bool proDebug = 0;

uint16_t graphColor = ST77XX_BLUE;
uint16_t pointColor = ST77XX_WHITE;
uint16_t lineColor = ST77XX_GREEN;
uint16_t pitchLineColor = ST77XX_RED;
uint16_t yawLineColor = ST77XX_MAGENTA;

String graphName = "ROLL PITCH YAW Graph";

int graphRange = 360;
int markSize = 3;

// Calculate Values
const int numberOfMarks = 27;
const int originX = 25;
const int originY = 110;
const int sizeX = 270;
const int sizeY = 50;
const int deviation = 40;

int boxSize = (sizeX / numberOfMarks);
//int mark[] = {(boxSize + deviation), ((boxSize * 2) + deviation), ((boxSize * 3) + deviation), ((boxSize * 4) + deviation), ((boxSize * 5) + deviation), ((boxSize * 6) + deviation), ((boxSize * 7) + deviation), ((boxSize * 8) + deviation)};
int mark[] = { originX, originX + 5, originX + 10, originX + 15, originX + 20, originX + 25, originX + 30, originX + 35, originX + 40, originX + 45, originX + 50, originX + 55, originX + 60, originX + 65, originX + 70, originX + 75, originX + 80, originX + 85, originX + 90, originX + 95, originX + 100 };
const int minorSizeY = (originY + 5);
const int minorSizeX = (originX - 10);

int numberSize = (sizeY / 5);
int number[] = { numberSize, (numberSize * 2), (numberSize * 3), (numberSize * 4), (numberSize * 5), (numberSize * 6), (numberSize * 7), (numberSize * 8), (numberSize * 9) };

int numberValue = (graphRange / 4);
int val[] = { (-graphRange / 2), (-numberValue), (-numberValue / 2), (-numberValue / 9), 0, (numberValue / 9), (numberValue / 2), (numberValue), (graphRange / 2) };


typedef struct struct_message {
  float accel_x;
  float accel_y;
  float accel_z;
} struct_message;


LSM6DSM IMU;
struct_message imuData;

float gyroz;
float norm_gyroz;
float yaw = 0;
float tstep = 0.5;
unsigned long timer = 0;

float mapf(float value, float in_min, float in_max, float out_min, float out_max) {
  return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void drawGraph();

void setup(void) {
  Serial.begin(115200);
  IMU.begin();
  // OR use this initializer (uncomment) if using a 1.44" TFT:
  tft.initR(INITR_144GREENTAB);  // Init ST7735R chip, green tab
  tft.fillScreen(ST77XX_WHITE);
  drawGraph();
}

void loop() {
  //delay(1000);

  imuData.accel_x = IMU.readFloatAccelX();
  Serial.print("ROLL: ");
  imuData.accel_x = mapf(imuData.accel_x, -1.0, 1.0, -180.0, 180.0);
  Serial.println(imuData.accel_x);
  tft.setCursor(15, 120);
  tft.setTextColor(lineColor);
  tft.setTextSize(1);
  tft.println("ROLL");

  imuData.accel_y = IMU.readFloatAccelY();
  Serial.print("PITCH: ");
  imuData.accel_y = mapf(imuData.accel_y, -1.0, 1.0, -180.0, 180.0);
  Serial.println(imuData.accel_y);
  tft.setCursor(45, 120);
  tft.setTextColor(pitchLineColor);
  tft.setTextSize(1);
  tft.println("PITCH");

  timer = millis();
  gyroz = IMU.readFloatGyroZ();
  norm_gyroz = gyroz * 0.007633f;
  yaw = yaw + norm_gyroz * tstep;
  Serial.print("YAW: ");
  float yaw2 = mapf(yaw, -1.00, 1.00, -180.00, 180.00);
  Serial.println(yaw2);
  tft.setCursor(88, 120);
  tft.setTextColor(yawLineColor);
  tft.setTextSize(1);
  tft.println("YAW");

  delay((tstep * 1000) - (millis() - timer));

  timeBlock[valuePosYaw] = ((millis() - 4500) / 1000);


  float roll = imuData.accel_x;
  float pitch = imuData.accel_y;

  valueBlockRoll[valuePosRoll] = roll;
  valueBlockPitch[valuePosPitch] = pitch;
  valueBlockYaw[valuePosYaw] = yaw2;

  if (proDebug) {
    Serial.println(timeBlock[valuePosRoll]);
    Serial.println(timeBlock[valuePosPitch]);
    Serial.println(timeBlock[valuePosYaw]);
  }

  if (blockPos < 21) {
    // print the time
    /*  tft.setCursor((mark[valuePosRoll] - 5), (originY + 16));
    tft.setTextColor(graphColor, ST77XX_RED);
    tft.setTextSize(1);
    tft.println(timeBlock[valuePosRoll]); */

    // map the value
    locationBlockPitch[valuePosPitch] = map(pitch, 0, graphRange, 63, 110);
    locationBlockRoll[valuePosRoll] = map(roll, 0, graphRange, 63, 110);
    locationBlockYaw[valuePosYaw] = map(yaw2, 0, graphRange, 63, 110);

    // draw point
    tft.fillRect((mark[valuePosRoll] - 1), (locationBlockRoll[valuePosRoll] - 1), markSize, markSize, pointColor);
    tft.fillRect((mark[valuePosPitch] - 1), (locationBlockPitch[valuePosPitch] - 1), markSize, markSize, pointColor);
    // tft.fillRect(60, 280, markSize, markSize, pointColor);
    // try connecting to previous point
    if (valuePosRoll != 0) {
      tft.drawLine(mark[valuePosPitch], locationBlockPitch[valuePosPitch], mark[(valuePosPitch - 1)], locationBlockPitch[(valuePosPitch - 1)], pitchLineColor);
      tft.drawLine(mark[valuePosRoll], locationBlockRoll[valuePosRoll], mark[(valuePosRoll - 1)], locationBlockRoll[(valuePosRoll - 1)], lineColor);
      tft.drawLine(mark[valuePosYaw], locationBlockYaw[valuePosYaw], mark[(valuePosYaw - 1)], locationBlockYaw[(valuePosYaw - 1)], yawLineColor);
      //tft.fillRect(60, 280, markSize, markSize, pointColor);
    }

    blockPos++;
  } else {
    // clear the graph's canvas
    //tft.fillRect((originX + 2), (originY - sizeY), sizeX, sizeY, ST77XX_WHITE);
    tft.fillRect((originX + 1), (originY - 90), 350, 90, ST77XX_WHITE);

    // map the value - current point
    locationBlockPitch[valuePosPitch] = map(pitch, 0, graphRange, 63, 130);
    locationBlockRoll[valuePosRoll] = map(roll, 0, graphRange, 63, 130);
    locationBlockYaw[valuePosYaw] = map(yaw2, 0, graphRange, 63, 130);


    // draw point - current point
    tft.fillRect((mark[7]), (locationBlockPitch[valuePosPitch] - 1), markSize, markSize, pointColor);
    tft.fillRect((mark[7]), (locationBlockRoll[valuePosRoll] - 1), markSize, markSize, pointColor);
    tft.fillRect((mark[7]), (locationBlockYaw[valuePosYaw] - 1), markSize, markSize, pointColor);

    // draw all points
    for (int i = 0; i < 20; i++) {
      tft.fillRect((mark[(blockPos - (i + 1))] - 1), (locationBlockPitch[(valuePosRoll - i)] - 1), markSize, markSize, pointColor);
      tft.fillRect((mark[(blockPos - (i + 1))] - 1), (locationBlockRoll[(valuePosPitch - i)] - 1), markSize, markSize, pointColor);
      tft.fillRect((mark[(blockPos - (i + 1))] - 1), (locationBlockYaw[(valuePosYaw - i)] - 1), markSize, markSize, pointColor);
    }

    // draw all the lines
    for (int i = 0; i < 20; i++) {
      tft.drawLine(mark[blockPos - (i + 1)], locationBlockPitch[valuePosPitch - i], mark[blockPos - (i + 2)], locationBlockPitch[valuePosPitch - (i + 1)], pitchLineColor);
      tft.drawLine(mark[blockPos - (i + 1)], locationBlockRoll[valuePosRoll - i], mark[blockPos - (i + 2)], locationBlockRoll[valuePosRoll - (i + 1)], lineColor);
      tft.drawLine(mark[blockPos - (i + 1)], locationBlockYaw[valuePosRoll - i], mark[blockPos - (i + 2)], locationBlockYaw[valuePosRoll - (i + 1)], yawLineColor);
    }

    // change time lables
    /* for(int i = 0; i <= 7; i++)
    {
      tft.setCursor((mark[(7 - i)] - 5), (originY + 16));
      tft.setTextColor(graphColor, ST77XX_WHITE);
      tft.setTextSize(1);
      tft.println(timeBlock[valuePosRoll - i]);
    }*/
  }
  valuePosRoll++;
  valuePosPitch++;
  valuePosYaw++;
}

void drawGraph() {
  // draw title
  tft.setCursor(5, 5);             // set the cursor
  tft.setTextColor(ST77XX_BLACK);  // set the colour of the text
  tft.setTextSize(1);              // set the size of the text
  tft.println(graphName);

  // draw outline
  tft.drawLine(originX, originY, (originX + sizeX), originY, graphColor);
  tft.drawLine(originX, originY, originX, 15, graphColor);

  // draw lables
  for (int i = 0; i < numberOfMarks; i++) {
    tft.drawLine(mark[i], originY, mark[i], minorSizeY, graphColor);
  }

  // draw numbers
  for (int i = 0; i < 10; i++) {
    //  tft.drawLine(originX, (originY - number[i]), minorSizeX, (originY - number[i]), graphColor);
  }

  // draw number values
  for (int i = 0; i < 9; i++) {
    tft.setCursor((minorSizeX - 12), (number[i] + numberSize));
    tft.setTextColor(graphColor);
    tft.setTextSize(1);
    tft.println(val[i]);
  }
}