// Bu proje Deneyap Kart'ın dahili IMU(ivme ölçer-Jiroskop) sensörü kullanılarak yapılmıştır.

#include "lsm6dsm.h"
float accelX, accelY, accelZ, accAngleY, accAngleX; // IMU değeri değişken tanımlaması yapıldı
LSM6DSM IMU;                                        // IMU için class tanımlanması
void setup()
{
    Serial.begin(115200); // Seri port haberleşme başlatıldı
    IMU.begin();          // IMU haberleşme başlatıldı
}
void loop()
{
    imudeprem();
}
void imudeprem()
{
    accelX = IMU.readFloatAccelX(); // sensörden gelen X, Y, Z verileri okundu
    accelY = IMU.readFloatAccelY();
    accelZ = IMU.readFloatAccelZ();
    accAngleX = atan(accelX / sqrt(pow(accelY, 2) + pow(accelZ, 2) + 0.001)) * 180 / PI; // Matematik işlemi ile sensör verileri -90-+90 arasında açı değerlerine dönüştürüldi.
    accAngleY = atan(-1 * accelY / sqrt(pow(accelX, 2) + pow(accelZ, 2) + 0.001)) * 180 / PI;
    Serial.print(" AngleY: " + String(accAngleY)); // AngelY de okunan değere göre eşik değerinizi belirleyebilirsiniz.
    Serial.println(" AngleX: " + String(accAngleX));
}
