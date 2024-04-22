#include <DeneyapTelegram.h>
#include <Deneyap_Servo.h>
#include "time.h"

String ID = "***********";                  // Kullanıcının telegram ID numarası
String begin = "************************";  // Oluşturulan botun HTTP ID numarası
#define Wlanssid "*****************"        // Wifi Adı
#define Wlanpassword "*****************"    // Wifi şifresi
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;
char timeHour[3];
char timeDay[3];
char timeMinute[3];
char timeYear[5];
char timeMonth[3];
int Month;
int Hour;
int timer1, timer2, Time, losttime, firsttime;
int led = RGBLED;
bool light = true;
Servo Servo1;
DeneyapTelegram telegram;
void setup() {
  Serial.begin(115200);                    // Seri port haberleşme başlatıldı
  WiFi.begin(Wlanssid, Wlanpassword);      // WiFi haberleşme başlatılması
  while (WiFi.status() != WL_CONNECTED) {  // bağlantının while döngüsü ile kontrol edilmesi
    delay(250);
    Serial.print(".");
  }
  Serial.println("Baglandi");
  telegram.begin(begin);  // Telegram Bot Tokeniniz
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  telegram.send(ID, "Mama Kabı Aktif Saat: " + String(timeDay) + "/" + String(timeMonth) + "/" + String(timeYear) + " Saat: " + String(Hour) + ":" + String(timeMinute));
  pinMode(led, OUTPUT);
  Servo1.attach(D12);
  Servo1.write(55);
}

void loop() {
  printLocalTime();

  if (!firsttime) {
    firsttime = Hour;  // Saat verisi kullanılarak mama vereceği saatin belirlenmesi
  }
  losttime = Hour;
  if ((abs(losttime - firsttime)) >= 1) {
    Time = abs(losttime - firsttime);
    if (Time == 2) {                  // 2 Saatte bir mama vermesi komutu süreyi değiştirebilirsiniz.
      if (millis() - timer1 > 500) {  // Her 100 ms de led ve buzzer'in durumu değişerek ses ve ışık yayması sağlandı
        timer1 = millis();
        light = !light;
        digitalWrite(led, light);
      }
    }
    if (Time >= 12) {
      cateattime();
    }
  }
}
void cateattime() {
  Servo1.write(100);
  telegram.send(ID, "Mama verildi. Saat: " + String(timeDay) + "/" + String(timeMonth) + "/" + String(timeYear) + " Saat: " + String(Hour) + ":" + String(timeMinute));
  digitalWrite(led, LOW);
  delay(1000);
  Servo1.write(55);
  firsttime = losttime;
}
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  strftime(timeDay, 3, "%d", &timeinfo);
  strftime(timeMonth, 3, "%D", &timeinfo);
  strftime(timeYear, 5, "%Y", &timeinfo);
  strftime(timeHour, 3, "%H", &timeinfo);
  strftime(timeMinute, 3, "%M", &timeinfo);
  Hour = atoi(timeHour);
  Hour = Hour + 2;
}
