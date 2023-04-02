/* FirebaseAkilliEv

github.com/pxsty0 tarafından eklendi.

Proje de Deneyap Mini, Deneyap Röle, Deneyap OLED Ekran ve Deneyap Sıcaklık ve Nem Ölçer kullanılmıştır

Bu uygulama ile evinize ait sıcaklık ve nem verilerini saniyelik öğrenebilirsiniz.
Uygulama üzerinden uzaktan bağlantı ile evinizin kapısını, klimasını, kombisini ve daha bir çok elektronik eşyanızı kontrol edebilirsiniz.

Kontrol arayüzü yakın da yayınlanacaktır.

==============================================================================
 Bu uygulama örneği için "Firebase ESP32 Client by Mobizt" kütüphanesi indirilmelidir.  ->https://github.com/mobizt/Firebase-ESP32<-
==============================================================================
*/

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Deneyap_Role.h>
#include <Deneyap_OLED.h>
#include <Deneyap_SicaklikNemOlcer.h>

#define WLAN_SSID       ""
#define WLAN_PASS       ""

#define API_KEY         ""
#define DATABASE_URL    ""

#define USER_EMAIL ""
#define USER_PASSWORD ""

Relay Role;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
OLED OLED;
TempHum TempHum;

void setup(){
  Serial.begin(115200);
  OLED.begin(0x7A);
  OLED.clearDisplay();
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
    OLED.setTextXY(0, 0);
    OLED.putString("****************");
    OLED.setTextXY(2, 0);
    OLED.putString("   Internete    ");
    OLED.setTextXY(4, 0);
    OLED.putString("  Baglaniliyor  ");
    OLED.setTextXY(5, 5);
    OLED.putString("");
    OLED.setTextXY(7, 0);
    OLED.putString("****************");
  Serial.print("Wi-Fi Baglanıyor...");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
     if(!Role.begin(0x0C)){
      delay(3000);
      Serial.println("I2C bağlantısı başarısız ");
      while(1);
   }
      if(!TempHum.begin(0x70)){
      delay(3000);
      Serial.println("I2C bağlantısı başarısız ");
   }

  Serial.println();
  Serial.print("Baglandı. IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;

  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  while ((auth.token.uid) == "") {

    Serial.print('.');
    delay(1000);
  }
  OLED.clearDisplay();
  OLED.setTextXY(0, 0);
  OLED.putString("****************");
  OLED.setTextXY(2, 0);
  OLED.putString("    Baglanti    ");
  OLED.setTextXY(4, 0);
  OLED.putString("    Saglandi    ");
  OLED.setTextXY(7, 0);
  OLED.putString("****************");
  Firebase.reconnectWiFi(true);

}
void loop(){
  if (Firebase.ready()){
    float Tempvalue = TempHum.getTempValue();
    float Humvalue = TempHum.getHumValue();

    String redLed = "Kapali";
    String blueLed = "Kapali";
    String greenLed = "Kapali";
    String door = "Kapali";

    Firebase.RTDB.set(&fbdo,"temp",Tempvalue);
    Firebase.RTDB.set(&fbdo,"humidity",Humvalue);

if (Firebase.RTDB.get(&fbdo, "door")){
if (fbdo.dataType() == "int"){

 int data = fbdo.intData();
 if(data == 1){
   Role.RelayDrive(1);
   door = "Acik";
 }
 else if(data == 0){
   Role.RelayDrive(0);
   door = "Kapali";
 }
}
}
if (Firebase.RTDB.get(&fbdo, "redLed")){
if (fbdo.dataType() == "int"){

 int data = fbdo.intData();
 if(data == 1){
   digitalWrite(LEDR, HIGH);
   redLed = "Acik";
 }
 else if(data == 0){
   digitalWrite(LEDR, LOW);
   redLed = "Kapali";
 }
}
}
if (Firebase.RTDB.get(&fbdo, "greenLed")){
if (fbdo.dataType() == "int"){

 int data = fbdo.intData();
 if(data == 1){
   digitalWrite(LEDG, HIGH);
   greenLed = "Acik";
 }
 else if(data == 0){
   digitalWrite(LEDG, LOW);
   greenLed = "Kapali";
 }
}
}
if (Firebase.RTDB.get(&fbdo, "blueLed")){
if (fbdo.dataType() == "int"){

 int data = fbdo.intData();
 if(data == 1){
   digitalWrite(LEDB, HIGH);
   blueLed = "Acik";
 }
 else if(data == 0){
   digitalWrite(LEDB, LOW);
   blueLed = "Kapali";
 }
}
}
  OLED.clearDisplay();
  OLED.setTextXY(0, 0);
  OLED.putString("Role : "+door);
  OLED.setTextXY(2, 0);
  OLED.putString("Kirmizi : "+redLed);
  OLED.setTextXY(4, 0);
  OLED.putString("Mavi : "+blueLed);
  OLED.setTextXY(6,0);
  OLED.putString("Yesil : "+greenLed);
  OLED.setTextXY(7, 0);
  OLED.putString("");
  }
  else{
    Serial.println("FireBase Bağlantısı Sağlanamadı");
  }
}
