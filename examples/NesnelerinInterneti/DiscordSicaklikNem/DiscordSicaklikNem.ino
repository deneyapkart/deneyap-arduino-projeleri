/*DiscordSicaklikNem

Mustafa "pxsty" kök tarafından eklendi --> github.com/pxsty0

Bu projede discord kanalına deneyap sıcaklık ve nem ölçer sensorü aracılığıyla sıcaklık ve nem Verisi istenilen zaman aralığından gönderilmektedir

Bu uygulama örneği için "Deneyap_SicaklikNemOlcer" kütüphanesi indirilmelidir.  -> https://github.com/deneyapkart/deneyap-sicaklik-nem-olcer-arduino-library <-
Bu uygulama örneği için "Usini Discord WebHook" kütüphanesi indirilmelidir.  -> https://github.com/usini/usini_discord_webHook <-
Webhook nasıl alınır -> https://github.com/pxsty0/pxsty0/blob/main/WebhookNasilAlinir.md <-



*/
#include <WiFi.h>
#include <Discord_WebHook.h>
#include <Deneyap_SicaklikNemOlcer.h>


#define WIFI_SSID ""        // Wifi Adı
#define WIFI_PASS ""        // Wifi Şifresi
#define DISCORD_WEBHOOK ""  // Discord Webhook Adresi
int delayVal = 30;          // Kaç Dakikada Bir Mesaj Atılacağını Belirtiyoruz

Discord_Webhook discord;
TempHum tempHum;

void setup() {
  Serial.begin(9600);
  tempHum.begin(0x70);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Wi-Fi Baglanıyor...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  discord.begin(DISCORD_WEBHOOK);
  Serial.println("Baglanti Saglandi");
  discord.send("Deneyap Kart Aktif :white_check_mark:");
}

void loop() {
  float hum = tempHum.getHumValue();
  float temp = tempHum.getTempValue();
  discord.send("Sıcaklık : " + String(temp) + " °C | Nem : " + String(hum) + " %");
  delay(delayVal * 60 * 1000);
}
