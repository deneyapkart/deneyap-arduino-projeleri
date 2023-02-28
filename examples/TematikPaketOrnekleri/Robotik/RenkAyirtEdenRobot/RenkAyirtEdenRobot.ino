/*  RobotAyirtEdenRobot
 *  T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *
 *  Bu uygulamada 1 Deneyap Geliştirme Kartı, 1 Deneyap Hareket, Işık, Renk Algılayıcı&Mesafe Ölçer modülü ve 2 Servo Motor kullanılmıştır.
 *  Modülden algılanan renge göre servo motor mili hareket etmektedir ve aynı renk Deneyap dahili adreslenebilir rgb ledinde yanmaktadır.
 *  Aynı zamanda okunan renk değerleri seri port ekranına yazdırmaktır
 *
 *  ==============================================================================
 *    Bu uygulama örneği için "Deneyap Hareket Isik Renk Algilayici Mesafe Olcer by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-hareket-isik-renk-algilayici-mesafe-olcer-arduino-library<-
 *  ==============================================================================
*/
#include <Deneyap_HareketIsikRenkAlgilayiciMesafeOlcer.h>  // Deneyap Hareket, Işık, Renk Algılayıcı, Mesafe Ölçer Algılayıcısı kütüphanesi eklenmesi
#include <Deneyap_Servo.h>                                 // Deneyap Servo kütüphanesi eklenmesi

APDS9960 Renk;  // APDS9960 için class tanımlanması
Servo servo1;   // 1. servo motor için class tanımlanması
Servo servo2;   // 2. servo motor için class tanımlanması

uint16_t r, g, b, c;

void setup() {
  Serial.begin(115200);    // Seri haberleşme başlatılması
  Renk.begin(0x39);        // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  servo1.attach(D4);       // 1. servo motorun D4 pinin ve 0 kanal ayarlanması  /*attach(pin, channel=0, freq=50, resolution=12) olarak belirlenmiştir. Kullandığınız motora göre değiştirebilirsiniz */
  servo2.attach(D5, 1);    // 2. servo motorun D5 pinin ve 1 kanal ayarlanması
  Renk.enableColor(true);  // Renk ayırt etme aktifleştirilmesi
}

void loop() {
  Renk.getColorData(&r, &g, &b, &c);  // Renk verilerinin okunması
  Serial.printf("Kırmızı: %d Yeşil: %d Mavi: %d Ortam: %d\n", r, g, b, c); // Renk verilerinin seri port ekranına yazdırılması
  if (r > 30) { // Ortamınızın ve algılanacak nesnenizin okunan değerine döre değişiklik yapınız.
    Serial.println("kirmizi");
    neopixelWrite(RGBLED, 250, 0, 0);  // Deneyap dahili adreslenebilir rgb ledin kırmızı yanması
    servo1.write(30);                  // 1. servo motorun milinin 30 derece dönmesi
    servo2.write(30);                  // 2. servo motorun milinin 30 derece dönmesi
    delay(3000);
  }
  if (g > 30) { // Ortamınızın ve algılanacak nesnenizin okunan değerine döre değişiklik yapınız.
    Serial.println("yeşil");
    neopixelWrite(RGBLED, 0, 250, 0);  // Deneyap dahili adreslenebilir rgb ledin yeşil yanması
    servo1.write(60);                  // 1. servo motorun milinin 60 derece dönmesi
    servo2.write(60);                  // 2. servo motorun milinin 60 derece dönmesi
    delay(3000);
  } else if (b > 23) { // Ortamınızın ve algılanacak nesnenizin okunan değerine döre değişiklik yapınız.
    Serial.println("mavi");
    neopixelWrite(RGBLED, 0, 0, 250);  // Deneyap dahili adreslenebilir rgb ledin mavi yanması
    servo1.write(45);                  // 1. servo motorun milinin 45 derece dönmesi
    servo2.write(45);                  // 2. servo motorun milinin 45 derece dönmesi
    delay(3000);
  }
  Serial.println("Renk ayirt edilmedi");
  neopixelWrite(RGBLED, 0, 0, 0);  // Deneyap dahili adreslenebilir rgb ledin sönmesi
  servo1.write(0);                 // 1. servo motorun milinin 0 derece dönmesi
  servo2.write(0);                 // 2. servo motorun milinin 0 derece dönmesi
  delay(10);
}
