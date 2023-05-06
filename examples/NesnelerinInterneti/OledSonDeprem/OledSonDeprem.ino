/*OledSonDeprem

Mustafa "pxsty" kök tarafından eklendi --> github.com/pxsty0
Bu projede Deneyap OLED Ekran kullanılarak son deprem ekrana yazdırılmıştır ekstra olarak her yeni deprem olduğunda buzzer 300 milisaniye boyunca ötmektedir.

Bu uygulama örneği için "Deneyap_OLED" kütüphanesi indirilmelidir.  -> https://github.com/deneyapkart/deneyap-oled-ekran-arduino-library <-
Bu uygulama örneği için "ArduinoJson" Versiyon 6.9.0 kütüphanesi indirilmelidir.  -> https://github.com/bblanchon/ArduinoJson <-

*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Deneyap_OLED.h>


#define WLAN_SSID ""  // Wifi SSID
#define WLAN_PASS ""  // Wifi Şifresi
#define BUZZER D9

HTTPClient http;
OLED oled;

String lastId = "0";

void connectionErr() {
  oled.clearDisplay();
  oled.setTextXY(2, 0);
  oled.setFont(font8x8);
  oled.putString("  Internete ");
  oled.setTextXY(4, 0);
  oled.putString(" Baglaniliyor ");
}

void setup() {
  Serial.begin(115200);
  oled.begin(0x7A);
  pinMode(BUZZER, OUTPUT);
  connectionErr();
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  Serial.print("Wi-Fi Baglanıyor...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Baglandı. IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop() {

  http.begin("http://pxstyearthquakes.glitch.me/");

  int httpCode = http.GET();

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println(response);
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, response);
    Serial.println(response);
    int code = doc["code"];
    if (code == 200) {
      String id = doc["afadDepremID"];
      String place = doc["data"]["en"]["place"];
      String size = doc["data"]["en"]["size"];
      if (id != lastId) {
        oled.clearDisplay();
        oled.setTextXY(0, 0);
        oled.setFont(font8x8);
        oled.putString("   Son Deprem   ");
        oled.setTextXY(2, 0);
        oled.putString("Yer : " + place);
        oled.setTextXY(7, 0);
        oled.putString("Buyukluk : " + size);
        digitalWrite(BUZZER, HIGH);
        delay(300);
        digitalWrite(BUZZER, LOW);
      }
      lastId = id;
    }

  } else {
    connectionErr();
  }

  http.end();

  delay(1000);
}