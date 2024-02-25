// Bu proje Deneyap Kart'ın dahili IMU(ivme ölçer-Jiroskop) sensörü kullanılarak yapılmıştır.

#include "lsm6dsm.h"                                             // IMU kütüphanesi tanımlandı
#include <DeneyapTelegram.h>                                     // Telegram kütüphanesi tanımlandı(sadece Deneyap Kart'lar ile çalışmaktadır)
#define Wlanssid "************"                                  // Wifi Adı
#define Wlanpassword "*************"                             // Wifi şifresi
String ID = "*********";                                         // Kullanıcının telegram ID numarası
String begin = "**********************************************"; // Oluşturulan botun HTTP API numarası
unsigned long timer1 = 0;                                        // Belirli aralıklarda veri gönderimi için
unsigned long timer2 = 0;                                        // 2 Adet timer tanımlandı
int buzzer = D12;
int led = D0;
int a = 0;
int b = 0;
int q = 0;
bool light = true;
float accelX, accelY, accelZ, accAngleY, accAngleX; // IMU değeri değişken tanımlaması yapıldı
float firstquake, lostquake, quake, timer3, timer4; // Deprem verisi değişkenleri
LSM6DSM IMU;                                        // IMU için class tanımlanması
DeneyapTelegram telegram;                           // Telegram kütüphanesi için class tanımlanması yapıldı
void setup()
{
    Serial.begin(115200); // Seri port haberleşme başlatıldı
    IMU.begin();          // IMU haberleşme başlatıldı
    pinMode(led, OUTPUT);
    pinMode(buzzer, OUTPUT);
    WiFi.begin(Wlanssid, Wlanpassword); // WiFi haberleşme başlatıldı
    while (WiFi.status() != WL_CONNECTED)
    { // bağlantının while döngüsü ile kontrol ediliyor
        delay(250);
        Serial.print(".");
    }
    Serial.println("Baglandi");
    telegram.begin(begin);                       // Telegram Bot Tokeniniz
    telegram.send(ID, "DK Telegrama bağlandı."); // Telegrama botuna bağlandı mesajı gönderildi.
}
void loop()
{
    imudeprem();
    if (b == 1)
    {
        if (millis() - timer2 > 100)
        {                      // Oluşturulan timer fonksiyonları ile kodda delay olmadan
            timer2 = millis(); // arkaplanda bekleme süreleri saymaktadır.
            a++;
            Serial.println(" A: " + String(a));
        }
        if (a == 60)
        {          // her 100 ms de a değişmekte, a=60 olduğunda değişkenler sıfırlanmaktadır.
            a = 0; // A 60 olma değeri alarmın çalma süresidir. Siz bu değeri azaltıp artırarak
            q = 0; // süreyi değiştirebilirsiniz.
            b = 0;
            digitalWrite(buzzer, LOW);
            digitalWrite(led, LOW);
        }
    }
    if (q == 1)
    {
        if (millis() - timer1 > 100)
        { // Her 100 ms de led ve buzzer'in durumu değişerek ses ve ışık yayması sağlandı
            timer1 = millis();
            light = !light;
            digitalWrite(buzzer, light);
            digitalWrite(led, light);
        }
    }
}
void imudeprem()
{
    accelX = IMU.readFloatAccelX(); // sensörden gelen X, Y, Z verileri okundu
    accelY = IMU.readFloatAccelY();
    accelZ = IMU.readFloatAccelZ();
    accAngleX = atan(accelX / sqrt(pow(accelY, 2) + pow(accelZ, 2) + 0.001)) * 180 / PI; // Matematik işlemi ile sensör verileri -90-+90 arasında açı değerlerine dönüştürüldi.
    accAngleY = atan(-1 * accelY / sqrt(pow(accelX, 2) + pow(accelZ, 2) + 0.001)) * 180 / PI;
    if (!firstquake)
    {
        firstquake = abs(accAngleY);
    }
    lostquake = abs(accAngleY);
    if ((abs(lostquake - firstquake)) >= 0.5)
    {
        quake = abs(lostquake - firstquake);
        if (quake >= 2.75)
        {
            if (millis() - timer3 > 100)
            {
                timer3 = millis();
                telegram.send(ID, "Sarsıntı Şiddeti: " + String(quake));
            }
            b = 1;
            q = 1;
            if (a >= 1)
            {
                a = 0;
            }
        }
        Serial.print("Deprem Değeri: " + String(quake));
        Serial.print(" Firstquake: " + String(firstquake));
        Serial.println(" Lostquake: " + String(lostquake));
        firstquake = lostquake;
    }
    Serial.print(" AngleY: " + String(accAngleY));
    Serial.println(" AngleX: " + String(accAngleX));
}
