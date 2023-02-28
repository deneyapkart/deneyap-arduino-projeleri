/*  UcusKayitCihazi
 *  T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *
 *  Bu uygulamada 1 Deneyap Geliştirme Kartı, 1 Deneyap Basınç Ölçer, 1 Deneyap Derinlik Ölçer, 1 Deneyap 9 Eksen Ataletsel Ölçüm Birimi, 1 Deneyap GPS ve GLONASS Konum Belirleyici kullanılmıştır.
 *  Modüllerden okunan basınç, derinlik, akselerometre X Y Z, gyro X Y Z, manyetometre X Y Z, konum, tarih ve saat verilerini webserver, sd karta ve seri port ekranına yazdırmaktadır.
 *
 *  ==============================================================================
 *    Bu uygulama örneği için "Deneyap Basinc Olcer by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-basinc-olcer-arduino-library<-
 *    Bu uygulama örneği için "Deneyap GPS ve GLONASS Konum Belirleyici by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-gps-glonass-konum-belirleyici-arduino-library<-
 *    Bu uygulama örneği için "Deneyap Derinlik Olcer by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-derinlik-olcer-arduino-library<-
 *    Bu uygulama örneği için "Deneyap 9-Eksen Ataletsel Olcum Birimi by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-9-eksen-ataletsel-olcum-birimi-arduino-library<-
 *    Bu uygulama örneği için "Deneyap 6 Eksen Alaletsel Olcum Birimi by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-6-eksen-ataletsel-olcum-birimi-arduino-library<-
 *  ==============================================================================
*/
#include <Deneyap_BasincOlcer.h>                   // Deneyap Basınç Ölçer kütüphanesi eklenmmesi
#include <Deneyap_GPSveGLONASSkonumBelirleyici.h>  // Deneyap GPS ve GLONASS Konum Belirleyici kutuphanesi eklenmesi
#include <Deneyap_DerinlikOlcer.h>                 // Deneyap Derinlik Ölçer kütüphanesi eklenmesi
#include <Deneyap_6EksenAtaletselOlcumBirimi.h>    // Deneyap 6 Eksen Ataletsel Olcum Birimi kütüphanesi eklenmmesi
#include <Deneyap_9EksenAtaletselOlcumBirimi.h>    // Deneyap 9 Eksen Ataletsel Olcum Birimi kütüphanesi eklenmmesi
#include "SD.h"                                    // SD kütüphanesi eklenmmesi
#include <WiFi.h>                                  // Wi-Fi kütüphanesi eklenmmesi
#include <WiFiClient.h>                            // WiFiClient kütüphanesi eklenmmesi
#include <WiFiAP.h>                                // WiFiAP kütüphanesi eklenmmesi

AtmosphericPressure AtmosphericPressure;  // AtmosphericPressure için class tanımlanması
TofRangeFinder TofRangeFinder;            // TofRangeFinder için class tanımlanması
LSM6DSM AccGyro;                          // LSM6DSM için class tanımlanması
MAGNETOMETER MAGNETOMETER;                // MAGNETOMETER için class tanımlanması
GPS GPS;                                  // GPS için class tanımlanması

String dataString = "";  // veriyi tutmak icin String degiskeni tanimlanması

const char *ssid = "UcusKayitCihazi";    // DeneyapKart Erisim Noktasi (AP) ismi
const char *password = "deneyapkart1a";  // DeneyapKart Erisim Noktasi (AP) sifresi

WiFiServer server(80);  // WiFi sunucusu icin class tanimlama

/*Dosya yazma fonksiyonu */
void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Dosya yaziliyor: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Yazma islemi icin dosya acilamadi");
    return;
  }
  if (file.print(message)) {
    Serial.println("Dosya yazildi");
  } else {
    Serial.println("Dosya yazma islemi basarisiz");
  }
  file.close();
}

/*Dosyaya ekleme yapma fonksiyonu */
void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Dosyaya eklendi: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Ekleme islemi icin dosya acilamadi");
    return;
  }
  if (file.print(message)) {
    Serial.println("Ekleme islemi basarili");
  } else {
    Serial.println("Ekleme islemi basarisiz");
  }
  file.close();
}

void setup() {
  Serial.begin(115200);                                  // Seri haberleşme başlatılması
  GPS.begin(0x2F);                                       // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  AccGyro.begin();                                       // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  MAGNETOMETER.begin(0x60);                              // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  AtmosphericPressure.begin(0x76);                       // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  TofRangeFinder.begin(0x29);                            // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  SD.begin();                                            // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  writeFile(SD, "/YeniDosya.txt", "   VERİLER   \r\n");  //YeniDosya.txt uzantılı dosya oluşturuldu.
  Serial.println();

  WiFi.softAP(ssid, password);  // Cihaz Erisim Noktasi (AP) olarak baslatildi
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Kamera hazir! Baglanmak icin 'http://");  // Bağlantı sağlanması IP adresi seri port ekranına yazdırılıyor  ->192.168.4.1<-
  Serial.println(myIP);                                   // Görüntünün yayınlanacağı IP adresi seri port ekranına yazılması
  server.begin();

  Serial.println("Server baslatildi");
}

void loop() {
  GPS.readGPS(RMC);

  /* Okunan verilerin seri port ekranına yazdırılması */
  Serial.print("Konum: ");
  float lat;
  lat = GPS.readLocationLat();
  Serial.printf("%.7f", lat);
  Serial.print(",");
  float lng;
  lng = GPS.readLocationLng();
  Serial.printf("%.7f\n", lng);

  Serial.print("Tarih: ");
  int day;
  day = GPS.readDay();
  Serial.print(day);
  Serial.print("/");

  int month;
  month = GPS.readMonth();
  Serial.print(month);
  Serial.print("/");

  int year;
  year = GPS.readYear();
  Serial.println(year);

  Serial.print("Saat:  ");
  int hour;
  hour = GPS.readHour();
  Serial.print(hour);
  Serial.print(":");

  int minute;
  minute = GPS.readMinute();
  Serial.print(minute);
  Serial.print(":");

  int second;
  second = GPS.readSecond();
  Serial.println(second);

  Serial.println("\nAkselerometre degerleri");
  Serial.print("X ekseni: ");
  Serial.print(AccGyro.readFloatAccelX());
  Serial.print("\tY ekseni: ");
  Serial.print(AccGyro.readFloatAccelY());
  Serial.print("\tZ ekseni: ");
  Serial.println(AccGyro.readFloatAccelZ());
  delay(50);

  Serial.println("\nGyro degerleri");
  Serial.print("X ekseni: ");
  Serial.print(AccGyro.readFloatGyroX());
  Serial.print("\tY ekseni: ");
  Serial.print(AccGyro.readFloatGyroY());
  Serial.print("\tZ ekseni: ");
  Serial.println(AccGyro.readFloatGyroZ());
  delay(50);

  Serial.println("\nManyetometre degerleri");
  Serial.print("X ekseni:");
  Serial.print(MAGNETOMETER.readMagnetometerX());
  Serial.print("\tY ekseni:");
  Serial.print(MAGNETOMETER.readMagnetometerY());
  Serial.print("\tZ ekseni:");
  Serial.println(MAGNETOMETER.readMagnetometerZ());
  delay(50);

  Serial.println("\nSicaklik degerleri");
  Serial.print("Celsius: ");
  Serial.print(AccGyro.readTempC());
  Serial.print("\tFahrenheit: ");
  Serial.println(AccGyro.readTempF());
  delay(50);

  Serial.print("\nBasinc: ");
  Serial.println(AtmosphericPressure.getPressure());
  delay(50);

  Serial.print("\nUzaklık: ");
  Serial.println(TofRangeFinder.ReadDistance());
  delay(50);

  Serial.println(" ");

  /* Okunan verilerin SD karta yazdırılması */
  dataString += "------------------------------------------------------------";
  dataString += "\r\n";
  dataString += "Konum: ";
  dataString += String(GPS.readLocationLat());
  dataString += ",";
  dataString += String(GPS.readLocationLng());
  dataString += "\r\n";

  dataString += "Tarih: ";
  dataString += String(GPS.readDay());
  dataString += "/";
  dataString += String(GPS.readMonth());
  dataString += "/";
  dataString += String(GPS.readYear());
  dataString += "\r\n";

  dataString += "Saat: ";
  dataString += String(GPS.readHour());
  dataString += ":";
  dataString += String(GPS.readMinute());
  dataString += ":";
  dataString += String(GPS.readSecond());
  dataString += "\r\n";

  dataString += "Akselerometre degerleri: ";
  dataString += "X ekseni: ";
  dataString += String(AccGyro.readFloatAccelX());

  dataString += " Y ekseni: ";
  dataString += String(AccGyro.readFloatAccelY());

  dataString += " Z ekseni: ";
  dataString += String(AccGyro.readFloatAccelZ());
  dataString += "\n";
  dataString += "\r\n";

  dataString += "Gyro degerleri: ";
  dataString += "X ekseni: ";
  dataString += String(AccGyro.readFloatGyroX());

  dataString += " Y ekseni: ";
  dataString += String(AccGyro.readFloatGyroY());

  dataString += " Z ekseni: ";
  dataString += String(AccGyro.readFloatGyroZ());
  dataString += "\n";
  dataString += "\r\n";

  dataString += "Manyetometre degerleri: ";
  dataString += "X ekseni: ";
  dataString += String(MAGNETOMETER.readMagnetometerX());

  dataString += " Y ekseni: ";
  dataString += String(MAGNETOMETER.readMagnetometerY());

  dataString += " Z ekseni: ";
  dataString += String(MAGNETOMETER.readMagnetometerZ());
  dataString += "\n";
  dataString += "\r\n";

  dataString += "Sıcaklık degerleri: ";
  dataString += "Celsius: ";
  dataString += String(AccGyro.readTempC());

  dataString += " Fahrenheit: ";
  dataString += String(AccGyro.readTempF());
  dataString += "\n";
  dataString += "\r\n";

  dataString += "Basınç: ";
  dataString += String(AtmosphericPressure.getPressure());
  dataString += "\r\n";
  dataString += "\r\n";

  dataString += "Derinlik Ölçer: ";
  dataString += String(TofRangeFinder.ReadDistance());
  dataString += "\r\n";
  dataString += "\r\n";
  dataString += "\r\n";
  dataString += "\r\n";

  appendFile(SD, "/YeniDosya.txt", dataString.c_str());

  /* Okunan verilerin web server yazdırılması */
  WiFiClient client = server.available();  // Baglanti talepleri dinlenmesi

  if (client) {               // Istemci varligi kontrol edilmesi
    String currentLine = "";  // Istemciden gelen veriyi tutmak icin String degiskeni tanimlanmasi
    while (client.connected()) {
      if (client.available()) {  // Istemciden veri kontrolu yapilmasi
        client.print("Konum:");
        client.print(GPS.readLocationLat());
        client.print(",");
        client.println(GPS.readLocationLng());

        client.print("Tarih:");
        client.print(GPS.readDay());
        client.print("/");
        client.print(GPS.readMonth());
        client.print("/");
        client.println(GPS.readYear());

        client.print("Saat:");
        client.print(GPS.readHour());
        client.print(":");
        client.print(GPS.readMinute());
        client.print(":");
        client.println(GPS.readSecond());

        client.println("Akselerometre degerleri");
        client.print("X ekseni: ");
        client.print(AccGyro.readFloatAccelX());
        client.print(" Y ekseni: ");
        client.print(AccGyro.readFloatAccelY());
        client.print(" Z ekseni: ");
        client.println(AccGyro.readFloatAccelZ());

        client.println("Gyro degerleri");
        client.print("X ekseni: ");
        client.print(AccGyro.readFloatGyroX());
        client.print(" Y ekseni: ");
        client.print(AccGyro.readFloatGyroY());
        client.print(" Z ekseni: ");
        client.println(AccGyro.readFloatGyroZ());

        client.println("Manyetometre degerleri");
        client.print("X ekseni: ");
        client.print(MAGNETOMETER.readMagnetometerX());
        client.print(" Y ekseni: ");
        client.print(MAGNETOMETER.readMagnetometerY());
        client.print(" Z ekseni: ");
        client.println(MAGNETOMETER.readMagnetometerZ());

        client.print("Sicaklik degerleri:");
        client.print("Celsius: ");
        client.println(AccGyro.readTempC());
        client.print("Fahrenheit: ");
        client.println(AccGyro.readTempF());

        client.print("Basinc: ");
        client.println(AtmosphericPressure.getPressure());

        client.print("Derinlik: ");
        client.println(TofRangeFinder.ReadDistance());
        client.println(" ");
      }
    }
    client.stop();  // Yanittan sonra http baglantisi durdurulması ve yeni istemci talebi beklenmesi
  }
  Serial.println("");
  delay(1000);
}
