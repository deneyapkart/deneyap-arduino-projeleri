/*DiscordHareketAlgilama

Mustafa "pxsty" kök tarafından eklendi --> github.com/pxsty0

Bu projede discord kanalına deneyap Deneyap Hareket Algılama sensorü aracılığıyla ortamda hareket tespit edilmesi halin de mesaj göndermektedir

Bu uygulama örneği için "Deneyap_HareketAlgilama" kütüphanesi indirilmelidir.  -> https://github.com/deneyapkart/deneyap-hareket-algilama-arduino-library <-
Bu uygulama örneği için "Usini Discord WebHook" kütüphanesi indirilmelidir.  -> https://github.com/usini/usini_discord_webHook <-

Webhook nasıl alınır -> https://github.com/pxsty0/pxsty0/blob/main/WebhookNasilAlinir.md <-
*/
#include <WiFi.h>
#include <Discord_WebHook.h>
#include <Deneyap_HareketAlgilama.h>


#define WIFI_SSID ""        // Wifi Adı
#define WIFI_PASS ""        // Wifi Şifresi
#define DISCORD_WEBHOOK ""  // Discord Webhook Adresi

Discord_Webhook discord;
Gesture movement;

void setup() {
  Serial.begin(9600);
  movement.begin(0x32);
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
bool lastMovement;
void loop() {
  bool nowMovement = movement.readGesture();
  if(nowMovement != lastMovement){
    discord.send("**Hareket Tespit Edildi**");
    delay(2500);
  }
  lastMovement = nowMovement;
}
