/*  SeraAydinlatma
 *  T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *
 *  Bu uygulamada 1 Deneyap Geliştirme Kartı, 1 Deneyap Ultraviyole Işık Algılayıcı modülü kullanılmıştır.
 *  Ortamın ışık miktarını ölçmektedir. Ortamın ışık miktarına göre ledin parlaklığı seviyesini değiştirerek yakmaktadır. 
 *
 *  ==============================================================================
 *    Bu uygulama örneği için "Deneyap Ultraviyole Isik Algilayici by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-ultraviyole-isik-algilayici-arduino-library<-
 *  ==============================================================================
*/
#include <Deneyap_UltraviyoleIsikAlgilici.h>  // Deneyap Ultraviyole Işık Algılayıcı kütüphanesi eklenmmesi

UVlight UVlight;  // UVlight için class tanımlanması

int IsikMiktari;

void setup() {
  Serial.begin(115200);              // Seri haberleşme başlatılması
  UVlight.begin(0x53);               // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  UVlight.setMode(LTR390_MODE_ALS);  // Ortam ışığını okumaya ayarlanması.  LTR390_MODE_UVS: ultraviyole ışık verisi
}

void loop() {
  IsikMiktari = UVlight.getLUX();  // Ortam ışık miktarının okunması
  Serial.print("LUX: ");
  Serial.println(IsikMiktari);  // Ortam ışık miktarının seri port ekranına yazdırılması

  if (IsikMiktari < 50) neopixelWrite(RGBLED, 255, 255, 255);  // Ortam ışık miktarına göre adreslenebilir LED yüksek parlıklıkta yakması
  else if (IsikMiktari < 150) neopixelWrite(RGBLED, 128, 128, 128);
  else if (IsikMiktari < 200) neopixelWrite(RGBLED, 64, 64, 64);
  else if (IsikMiktari < 250) neopixelWrite(RGBLED, 32, 32, 32);  // Ortam ışık miktarına göre adreslenebilir LED düşük parlıklıkta yakması
  else neopixelWrite(RGBLED, 0, 0, 0);
  delay(50);  // 50 milisaniye bekletilmesi
}
