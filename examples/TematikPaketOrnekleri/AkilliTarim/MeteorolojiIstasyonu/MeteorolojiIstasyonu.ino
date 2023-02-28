/*  MeteorolojiIstasyonu
 *  T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *
 *  Bu uygulamada 1 Deneyap Geliştirme Kartı, 1 Deneyap Sıcaklık Nem Basınç Ölçer, 1 Deneyap Toprak Nemi Ölçer, 1 Deneyap Ultraviyole Işık Algılıcı, 1 Deneyap Yağmur Algılıcı, 1 Deneyap Gerçek Zamanlı Saat ve 1 Deneyap Hoparlör kullanılmıştır. 
 *  Toprak nemi, Yağmur değeri, ultraviyolu ya da ortam ışık algılayıcı, tarih ve saati, sıcaklık nem ve basınç verilerini seri port ekranına ve SD karta yazdırmaktadır.
 *  Ortam ışığı düşük ise RGBLED yanmaktadır ve uyarı sesi verilmektedir. 
 *
 *  ==============================================================================
 *    Bu uygulama örneği için "Deneyap Sicaklik Nem Basinc Olcer by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-sicaklik-nem-basinc-olcer-arduino-library<-
 *    Bu uygulama örneği için "Deneyap Toprak Nemi Olcer by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-toprak-nemi-olcer-arduino-library<-
 *    Bu uygulama örneği için "Deneyap Ultraviyole Isik Algilayici by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-ultraviyole-isik-algilayici-arduino-library<-
 *    Bu uygulama örneği için "Deneyap Yagmur Algilayici by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-yagmur-algilayici-arduino-library<-
 *    Bu uygulama örneği için "Deneyap Gercek Zamanli Saat by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-gercek-zamanli-saat-arduino-library<-
 *  ==============================================================================
*/
#include <Deneyap_SicaklikNemBasincOlcer.h>   // Deneyap Sıcaklık Nem Basınç Ölçer kütüphanesi eklenmesi
#include <Deneyap_BasincOlcer.h>              // Deneyap Basınç Ölçer kütüphanesi eklenmesi
#include <Deneyap_ToprakNemiOlcer.h>          // Deneyap Toprak Nemi Ölçer kütüphanesi eklenmesi
#include <Deneyap_UltraviyoleIsikAlgilici.h>  // Deneyap Ultraviyole Işık Algılıcı kütüphanesi eklenmesi
#include <Deneyap_YagmurAlgilama.h>           // Deneyap Yağmur Algılıcı kütüphanesi eklenmesi
#include <Deneyap_GercekZamanliSaat.h>        // Deneyap Gerçek Zamanlı Saat kütüphanesi eklenmesi
#include "SD.h"                               // SD kütüphanesi eklenmesi

SHT4x SHT4x;                              // SHT4x için class tanımlanması
SoilMoisture SoilMoisture;                // SoilMoisture için class tanımlanması
AtmosphericPressure AtmosphericPressure;  // AtmosphericPressure için class tanımlanması
UVlight UVlight;                          // UVlight için class tanımlanması
Rain Rain;                                // Rain için class tanımlanması
RTC RTC;                                  // RTC için class tanımlanması

float sicaklik;
float nem;
float basinc;
float toprakNem;
float UltraviyoleIsikYogunlugu;
float OrtamIsikYogunlugu;
float yagmurDeger;
String dataString = "";

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
  Serial.begin(115200);             // Seri haberleşme başlatılması
  pinMode(D15, OUTPUT);             // hoparlör için çıkış pini ayarlanması
  SD.begin();                       // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  SHT4x.begin(0X44);                // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  SoilMoisture.begin(0x0F);         // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  UVlight.begin(0x53);              // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  Rain.begin(0x2E);                 // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  AtmosphericPressure.begin(0x76);  // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  RTC.begin();                      // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  Wire.setClock(50000);
  UVlight.setMode(LTR390_MODE_ALS);  // Ortam ışığını okumaya ayarlanması.  LTR390_MODE_UVS: ultraviyole ışık verisi
  RTC.deviceStart();
  RTC.adjust();
  writeFile(SD, "/YeniDosya.txt", "   VERİLER   \r\n");
}

void loop() {
  DateTime now = RTC.now();  // Tarih ve Saat okunması ve seri port ekranına yazdırılması
  Serial.print("\nTarih:");
  Serial.printf("%02d/%02d/%04d", now.day(), now.month(), now.year());
  Serial.print("\tSaat:");
  Serial.printf("%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  delay(10);

  SHT4x.measure();
  sicaklik = SHT4x.TtoDegC();  // Sıcaklık değerinin okunması ve seri port ekranına yazdırılması
  Serial.print("\nSıcaklık: ");
  Serial.print(sicaklik);
  Serial.println(" °C");
  delay(10);

  nem = SHT4x.RHtoPercent();  // Bağıl Nem değerinin okunması ve seri port ekranına yazdırılması
  Serial.print("Bağıl nem: ");
  Serial.print(nem);
  Serial.println("%");
  delay(10);

  basinc = AtmosphericPressure.getPressure();  // Basınç değerinin okunması ve seri port ekranına yazdırılması
  Serial.print("Basinc: ");
  Serial.println(basinc);
  delay(10);

  toprakNem = SoilMoisture.ReadSoilMoisture();  // Toprak Nem değerinin okunması ve seri port ekranına yazdırılması
  Serial.print("Toprak Nemi Degeri: ");
  Serial.println(toprakNem);
  delay(10);

  yagmurDeger = Rain.ReadRainAnalog();  // Yağmur değerinin okunması ve seri port ekranına yazdırılması
  Serial.print("Yagmur Degeri: ");
  Serial.println(yagmurDeger);
  delay(10);

  /*UltraviyoleIsikYogunlugu = UVlight.getUVI();  // ultraviyole ışık verisi okunursa
  Serial.print("Ultraviyole Işık Yoğunluğu: ");
  Serial.println(UltraviyoleIsikYogunlugu); */

  OrtamIsikYogunlugu = UVlight.getLUX();  // ortam ışık verisi okunursa
  Serial.print("Ortam Işık Yoğunluğu: ");
  Serial.println(OrtamIsikYogunlugu);

  if (OrtamIsikYogunlugu < 200) {  // Ortam ışığı verisi 200den küçükse
    tone(D15, 500, 500);           // uyarı sesi çalması
    delay(500);
    tone(D15, 800, 500);
    delay(500);
    Serial.println("\nORTAM IŞIK YOĞUNLUĞU AZ");  // "ORTAM IŞIK YOĞUNLUĞU AZ" seri port ekranına yazdırılması
    tone(D15, 500, 500);
    delay(500);
    tone(D15, 800, 500);
    delay(500);
    Serial.println("led yanıyor ...");
    dataString += "ORTAM IŞIK YOĞUNLUĞU AZ";  // "ORTAM IŞIK YOĞUNLUĞU AZ" sd karta yazdırılması
    dataString += "\r\n";
    dataString += "LED YANIYOR";
    dataString += "\r\n";
    dataString += "\r\n";
    dataString += "\r\n";
    digitalWrite(LED_BUILTIN, HIGH);  // RGBLEDin beyaz yanması
  } else {
    digitalWrite(LED_BUILTIN, LOW);  // RGBLEDin yanmaması
  }
  delay(10);

  Serial.println("");

  dataString += String(now.hour());
  dataString += ":";
  dataString += String(now.minute());
  dataString += ":";
  dataString += String(now.second());
  dataString += "  ";
  dataString += String(now.day());
  dataString += "/";
  dataString += String(now.month());
  dataString += "/";
  dataString += String(now.year());
  dataString += "\r\n";

  dataString += "Sıcaklık: ";  // Sıcaklık değerinin sd karta yazdırılması
  dataString += String(SHT4x.TtoDegC());
  dataString += "\r\n";

  dataString += "Bağıl nem: ";  // Bağıl nem değerinin sd karta yazdırılması
  dataString += String(SHT4x.RHtoPercent());
  dataString += "\r\n";

  dataString += "Basinc: ";  // Basınç değerinin sd karta yazdırılması
  dataString += String(AtmosphericPressure.getPressure());
  dataString += "\r\n";

  dataString += "Toprak Nemi Degeri: ";  // Toprak Nemi değerinin sd karta yazdırılması
  dataString += String(SoilMoisture.ReadSoilMoisture());
  dataString += "\r\n";

  dataString += "Yagmur Degeri: ";  // Yağmur değerinin sd karta yazdırılması
  dataString += String(Rain.ReadRainAnalog());
  dataString += "\r\n";

  /*dataString += "Ultraviyole Işık Yoğunluğu: ";
  dataString += String(UVlight.getUVI());
  dataString += "\r\n"; */

  dataString += "Ortam Işık Yoğunluğu: ";  // Ortam Işık Yuğunluğu değerinin sd karta yazdırılması
  dataString += String(UVlight.getLUX());
  dataString += "\r\n";
  dataString += "\r\n";
  dataString += "\r\n";

  appendFile(SD, "/YeniDosya.txt", dataString.c_str());  // Verilerin SD karta yazdırılması

  delay(1000);
}
