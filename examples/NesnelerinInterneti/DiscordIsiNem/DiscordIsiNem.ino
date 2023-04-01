/*DiscordIsıNem

Mustafa "pxsty" kök tarafından eklendi --> github.com/pxsty0

Bu projede discord kanalına DHT11 sensorü aracılığıyla sıcaklık ve nem Verisi istenilen zaman aralığından gönderilmektedir

Bu uygulama örneği için "DHTlib" kütüphanesi indirilmelidir.  -> https://github.com/RobTillaart/DHTlib <-
Bu uygulama örneği için "Usini Discord WebHook" kütüphanesi indirilmelidir.  -> https://github.com/usini/usini_discord_webHook <-
Webhook nasıl alınır -> https://github.com/pxsty0/pxsty0/blob/main/WebhookNasilAlinir.md <-



*/
#include <DHT.h>
#include <WiFi.h>
#include <Discord_WebHook.h>

#define DHT11PIN D8                // DHT11 Sensor Pini
#define WIFI_SSID ""       // Wifi Adı
#define WIFI_PASS ""       // Wifi Şifresi
#define DISCORD_WEBHOOK ""  // Discord Webhook Adresi
int delayVal = 30;                 // Kaç Dakikada Bir Mesaj Atılacağını Belirtiyoruz

Discord_Webhook discord;
DHT dht(DHT11PIN, DHT11);

void setup() {
  Serial.begin(9600);
  dht.begin();
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
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  discord.send("Sıcaklık : "+String(temp)+" °C | Nem : "+String(hum)+" %");
  delay(delayVal * 60 * 1000);
}