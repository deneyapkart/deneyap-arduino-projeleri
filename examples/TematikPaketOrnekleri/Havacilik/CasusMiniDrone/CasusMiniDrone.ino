// ---------->>>>>>>>>> YUKLEME YAPILAMDAN DIKKAT EDILMESI GEREKEN HUSUS <<<<<<<<<<----------
// "Araclar->Partition Scheme->Huge APP" secilmeli //
// "Tools->Partition Scheme->Huge APP" secilmeli //

/*  CasusMiniDrone
 *  T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *
 *  Bu uygulamada 1 Deneyap Geliştirme Kartı, 1 Deneyap Kamera, 1 Deneyap Basınç Ölçer, 1 Deneyap Derinlik Ölçer, 1 Deneyap 9 Eksen Ataletsel Ölçüm Birimi, 1 Deneyap GPS ve 1 GLONASS Konum Belirleyici kullanılmıştır.
 *  Wi-Fi haberleşme protokolu kullanılarak görüntü aktarılmaktadır. 
 *  Konum verisi, Tarih ve saat verisi, Basınç verisi, Sıcaklık verisi, Derinlik verisi, X-Y-Z eksen akselerometre verileri, X-Y-Z eksen gyro verileri, X-Y-Z eksen manyetometre verileri okunmaktadır. 
 *  Okunan bu veriler seri port ekranına yazdırılmaktadır.
 *
 *  ==============================================================================
 *    Bu uygulama örneği için "Deneyap Basinc Olcer by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-basinc-olcer-arduino-library<-
 *    Bu uygulama örneği için "Deneyap GPS ve GLONASS Konum Belirleyici by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-gps-glonass-konum-belirleyici-arduino-library<-
 *    Bu uygulama örneği için "Deneyap Derinlik Olcer by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-derinlik-olcer-arduino-library<-
 *    Bu uygulama örneği için "Deneyap 9-Eksen Ataletsel Olcum Birimi by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-9-eksen-ataletsel-olcum-birimi-arduino-library<-
 *    Bu uygulama örneği için "Deneyap 6 Eksen Alaletsel Olcum Birimi by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-6-eksen-ataletsel-olcum-birimi-arduino-library<-
 *  ==============================================================================
*/
#include <WiFi.h>                                  // Wi-Fi kütüphanesi eklenmmesi
#include <Deneyap_BasincOlcer.h>                   // Deneyap Basınç Ölçer kütüphanesi eklenmmesi
#include <Deneyap_GPSveGLONASSkonumBelirleyici.h>  // Deneyap GPS ve GLONASS Konum Belirleyici kutuphanesi eklenmesi
#include <Deneyap_DerinlikOlcer.h>                 // Deneyap Derinlik Ölçer kütüphanesi eklenmesi
#include <Deneyap_6EksenAtaletselOlcumBirimi.h>    // Deneyap 6 Ekse Ataletsel Olcum Birimi kütüphanesi eklenmmesi
#include <Deneyap_9EksenAtaletselOlcumBirimi.h>    // Deneyap 9 Eksen Ataletsel Olcum Birimi kütüphanesi eklenmmesi

AtmosphericPressure AtmosphericPressure;  // AtmosphericPressure için class tanımlanması
GPS GPS;                                  // GPS için class tanımlaması
TofRangeFinder TofRangeFinder;            // TofRangeFinder için class tanımlanması
LSM6DSM LSM6DSM;                          // LSM6DSM icin Class tanimlamasi
MAGNETOMETER MAGNETOMETER;                // MAGNETOMETER icin Class tanimlamasi

const char* ssidAP = "DeneyapKart";  // DeneyapKart Erisim Noktasi (AP) ismi
const char* passwordAP = NULL;       // DeneyapKart Erisim Noktasi (AP) sifresi

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  cameraInit();  // Kamera konfigurasyonu yapılması

  WiFi.softAP(ssidAP, passwordAP);
  //  WiFi.softAPConfig(apip, gateway, subnet);
  delay(100);

  startCameraServer();  // Kamera server başlatılması

  Serial.print("Kamera hazir! Baglanmak icin 'http://");  // Bağlantı sağlanması
  Serial.print(WiFi.softAPIP());                          // Görüntünün yayınlanacağı IP adresi seri port ekranına yazılması
  Serial.println("' adresini kullaniniz");

  AtmosphericPressure.begin(0x76);
  GPS.begin(0x2F);
  TofRangeFinder.begin(0x29);
  MAGNETOMETER.begin(0x60);
  LSM6DSM.begin();
}

void loop() {
  GPS.readGPS(RMC);             // NMEA protokolünün RMC mesaj türü ile verinin okunması
  Serial.print("\n\nKonum: ");  // Konum verilerinin okunması ve seri port ekranına yazdırılması
  Serial.printf("%.7f", GPS.readLocationLat());
  Serial.print(",");
  Serial.printf("%.7f\n", GPS.readLocationLng());

  Serial.print("Tarih: ");  // Tarih verilerinin okunması ve seri port ekranına yazdırılması
  Serial.print(GPS.readDay());
  Serial.print("/");

  Serial.print(GPS.readMonth());
  Serial.print("/");

  Serial.println(GPS.readYear());

  Serial.print("Saat:  ");  // Saat verilerinin okunması ve seri port ekranına yazdırılması
  Serial.print(GPS.readHour());
  Serial.print(":");

  Serial.print(GPS.readMinute());
  Serial.print(":");

  Serial.println(GPS.readSecond());
  Serial.println("");

  Serial.print("Basinc: ");
  Serial.println(AtmosphericPressure.getPressure());  // Basınç değerinin okunması ve seri port ekranına yazdırılması

  Serial.print("Sicaklik: ");
  Serial.println(AtmosphericPressure.getTemp());  // Sıcaklık değerinin okunması ve seri port ekranına yazdırılması

  Serial.print("Derinlik: ");
  Serial.println(TofRangeFinder.ReadDistance());  // Derinliğin okunması ve seri port ekranına yazdırılması

  Serial.println("\nAkselerometre degerleri");  // X-Y-Z eksen akselerometre verileri okunması ve seri port ekranına yazdırılması
  Serial.print("X ekseni: ");
  Serial.print(LSM6DSM.readFloatAccelX());
  Serial.print("\tY ekseni: ");
  Serial.print(LSM6DSM.readFloatAccelY());
  Serial.print("\tZ ekseni: ");
  Serial.println(LSM6DSM.readFloatAccelZ());

  Serial.println("\nGyro degerleri");  // X-Y-Z eksen gyro verileri okunması ve seri port ekranına yazdırılması
  Serial.print("X ekseni: ");
  Serial.print(LSM6DSM.readFloatGyroX());
  Serial.print("\tY ekseni: ");
  Serial.print(LSM6DSM.readFloatGyroY());
  Serial.print("\tZ ekseni: ");
  Serial.println(LSM6DSM.readFloatGyroZ());

  Serial.println("\nMagnetometre degerleri");  // X-Y-Z eksen manyetometre verileri okunması ve seri port ekranına yazdırılması
  Serial.print("X ekseni:");
  Serial.print(MAGNETOMETER.readMagnetometerX());
  Serial.print("\tY ekseni:");
  Serial.print(MAGNETOMETER.readMagnetometerY());
  Serial.print("\tZ ekseni:");
  Serial.println(MAGNETOMETER.readMagnetometerZ());

  delay(3000);  // 3 saniye bekleme süresi
}
