/*  TemassizAnahtar
 *  T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *
 *  Bu uygulamada 1 Deneyap Geliştirme Kartı, 1 Deneyap 5x7 LED Matris; 1 Deneyap Hareket, Işık, Renk Algılayıcı&Mesafe Ölçer kullanılmıştır.
 *  Deneyap Hareket, Işık, Renk Algılayıcı&Mesafe Ölçer'de okunan yön Deneyap 5x7 Led Matis'de o yöndeki ok karakteri ile yazdırmaktadır.
 *  Deneyap geliştirme kartlarındaki dahili adreslenebilir RGBLED'de her okunan yöne göre farklı bir renk ile yanmaktadır.
 *
 *  ==============================================================================
 *    Bu uygulama örneği için "Deneyap Hareket Isik Renk Algilayici Mesafe Olcer by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-hareket-isik-renk-algilayici-mesafe-olcer-arduino-library<-
 *    Bu uygulama örneği için "Deneyap 5x7 LED Matris by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-5x7-led-matris-arduino-library<-
 *  ==============================================================================
*/
#include <Deneyap_HareketIsikRenkAlgilayiciMesafeOlcer.h>  // Deneyap Hareket, Işık, Renk Algılayıcı&Mesafe Ölçer kütüphanesi eklenmesi
#include <Deneyap_5x7LedEkran.h>                           // Deneyap 5x7 LED Matris kütüphanesi eklenmesi

APDS9960 APDS9960;    // APDS9960 için class tanımlanması
DotMatrix DotMatrix;  // DotMatrix için class tanımlanması

void setup() {
  Serial.begin(115200);   // Seri haberleşme başlatılması
  APDS9960.begin(0x39);   // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  DotMatrix.begin(0x0A);  // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  APDS9960.enableProximity(true);
  APDS9960.enableGesture(true);  // Hareket okunması aktifleşmesi
}

void loop() {
  uint8_t gestureState = APDS9960.readGesture();  // Hareket yönünün okunması
  if (gestureState == APDS9960_DOWN) {            // Hareket yönü aşağı ise
    Serial.println("Asagi");
    neopixelWrite(RGBLED, 255, 255, 255);  // RGBLED beyaz renkte yanması
    for (int i = 0; i < 250; i++) {        // Led Matriste aşağı ok karakterinin yazılması
      DotMatrix.dotrow1(0, 0, 1, 0, 0);
      DotMatrix.dotrow2(0, 0, 1, 0, 0);
      DotMatrix.dotrow3(0, 0, 1, 0, 0);
      DotMatrix.dotrow4(0, 0, 1, 0, 0);
      DotMatrix.dotrow5(1, 1, 1, 1, 1);
      DotMatrix.dotrow6(0, 1, 1, 1, 0);
      DotMatrix.dotrow7(0, 0, 1, 0, 0);
    }
  }
  if (gestureState == APDS9960_UP) {     // Hareket yönü yukarı ise
    neopixelWrite(RGBLED, 255, 255, 0);  // RGBLED sarı renkte yanması
    Serial.println("Yukari");
    for (int i = 0; i < 250; i++) {  // Led Matriste yukarı ok karakterinin yazılması
      DotMatrix.dotrow1(0, 0, 1, 0, 0);
      DotMatrix.dotrow2(0, 1, 1, 1, 0);
      DotMatrix.dotrow3(1, 1, 1, 1, 1);
      DotMatrix.dotrow4(0, 0, 1, 0, 0);
      DotMatrix.dotrow5(0, 0, 1, 0, 0);
      DotMatrix.dotrow6(0, 0, 1, 0, 0);
      DotMatrix.dotrow7(0, 0, 1, 0, 0);
    }
  }
  if (gestureState == APDS9960_LEFT) {   // Hareket yönü sol ise
    neopixelWrite(RGBLED, 0, 255, 255);  // RGBLED cyan renkte yanması
    Serial.println("Sol");
    for (int i = 0; i < 250; i++) {  // Led Matriste sol ok karakterinin yazılması
      DotMatrix.dotrow1(0, 0, 1, 0, 0);
      DotMatrix.dotrow2(0, 1, 1, 0, 0);
      DotMatrix.dotrow3(1, 1, 1, 1, 1);
      DotMatrix.dotrow4(0, 1, 1, 0, 0);
      DotMatrix.dotrow5(0, 0, 1, 0, 0);
      DotMatrix.dotrow6(0, 0, 0, 0, 0);
      DotMatrix.dotrow7(0, 0, 0, 0, 0);
    }
  }
  if (gestureState == APDS9960_RIGHT) {  // Hareket yönü sağ ise
    neopixelWrite(RGBLED, 255, 0, 255);  // RGBLED magente renkte yanması
    Serial.println("Sag");
    for (int i = 0; i < 250; i++) {  // Led Matriste sağ ok karakterinin yazılması
      DotMatrix.dotrow1(0, 0, 1, 0, 0);
      DotMatrix.dotrow2(0, 0, 1, 1, 0);
      DotMatrix.dotrow3(1, 1, 1, 1, 1);
      DotMatrix.dotrow4(0, 0, 1, 1, 0);
      DotMatrix.dotrow5(0, 0, 1, 0, 0);
      DotMatrix.dotrow6(0, 0, 0, 0, 0);
      DotMatrix.dotrow7(0, 0, 0, 0, 0);
    }
  }
  digitalWrite(RGBLED, LOW);
}
