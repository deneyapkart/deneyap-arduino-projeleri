/*  GuvenlikPaneli
 *  T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *  
 *  Bu uygulamada 1 Deneyap Geliştirme Kartı, 1 Deneyap Dokunmatik Tuş Takımı, 1 Deneyap Oled, 1 Deneyap Sıcaklık Nem Ölçer, 1 Deneyap Hareker Algılama, 1 Deneyap Hoparlör kullanılmıştır. 
 *  Ortamın sıcaklık ve nem bilgisini seri port ekranına ve oled ekranına yazdırmaktadır. Hareket algılanınca kullanıcıdan şifre girilmesi istenmektedir. 
 *  Dokunmatik tuş takımında girilen şifre doğru ise oled ve seri port ekranına "HAREKET ALGILANDI" yazdırmaktadır. Deneyap Kart 1A dahili RGBLED yeşil yanmaktadır. 
 *  Şifre yanlış ise hoparlör uyarı vermekte ve dahili RGBLED kırmızı led yanmaktadır. 
 *
 *  ==============================================================================
 *    Bu uygulama örneği için "Deneyap Dokunmatik Tuş Takımı by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-dokunmatik-tus-takimi-arduino-library<-
 *    Bu uygulama örneği için "Deneyap OLED by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-oled-ekran-arduino-library<-
 *    Bu uygulama örneği için "Deneyap Sicaklik Nem Olcer by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-sicaklik-nem-olcer-arduino-library<-  
 *    Bu uygulama örneği için "Deneyap Hareket Algilama by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-hareket-algilama-arduino-library<-
 *  ==============================================================================
*/
#include <Deneyap_DokunmatikTusTakimi.h>  // Deneyap Dokunmatik Tuş Takımı kütüphanesi eklenmesi
#include <Deneyap_OLED.h>                 // Deneyap OLED Ekran kütüphanesinin eklenmesi
#include <Deneyap_SicaklikNemOlcer.h>     // Deneyap Sıcaklık Nem Ölçer kütüphanesi eklenmesi
#include <Deneyap_HareketAlgilama.h>      // Deneyap Hareket Algılama kütüphanesi eklenmesi

int password[] = { 0, 3, 0, 3 };  // Belirlenen şifre
int inputvalue[4];                // Girilen şifre
int ArrayIndex = 0;               // Sayaç

Keypad Keypad;    // Keypad için class tanımlanması
OLED OLED;        // OLED için class tanımlanması
TempHum TempHum;  // TempHum için class tanımlanması
Gesture Gesture;  // Gesture için class tanımlanması

void setup() {
  Serial.begin(115200);  // Seri haberleşme başlatılması
  Keypad.begin(0x0E);    // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  TempHum.begin(0x70);   // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  Gesture.begin(0x32);   // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  OLED.begin(0x7A);      // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  OLED.clearDisplay();   // OLED ekrandaki verilerin silinmesi
  pinMode(D15, OUTPUT);  // hoparlör için çıkış pini ayarlanması
}

void loop() {
  float TempValue = TempHum.getTempValue();  // Sıcaklık değerinin okunması
  Serial.print("Sicaklik = ");
  Serial.print(TempValue);  // Sıcaklık değerinin seri port ekranına yazılması
  OLED.setTextXY(1, 0);
  OLED.putString("Sicaklik: ");
  OLED.setTextXY(1, 10);
  OLED.putFloat(TempValue);  // Sıcaklık değerinin oled ekranına yazılması

  float HumValue = TempHum.getHumValue();  // Nem değerinin okunması
  Serial.print("°C   Nem = %");
  Serial.println(HumValue);  // Nem değerinin seri port ekranına yazılması
  OLED.setTextXY(0, 0);
  OLED.putString("Nem: ");
  OLED.setTextXY(0, 5);
  OLED.putFloat(HumValue);  // Nem değerinin oled ekranına yazılması

  bool gestureState = Gesture.readGesture();  // Hareket durumunun okunması

  if (gestureState == 1) {  // Hareket algılanırsa
    Serial.println("Hareket ALGILANDI");
    OLED.setTextXY(7, 0);
    OLED.putString("Hareket ALGLANDI");

    Serial.println("Şifre giriniz");
    OLED.setTextXY(3, 0);
    OLED.putString("sifre giriniz");
  }

  int keyword = Keypad.KeypadRead();   // Tuş takımına basılan değeri okuması
  if (keyword != 0xFF) {               // Tuş takımına basıldığında
    inputvalue[ArrayIndex] = keyword;  // Birinci dizi sayısına tuş takımı değerini girilmesi
    ArrayIndex++;                      // Sayacı artırılması
    switch (ArrayIndex) {
      case 1:  // Birinci değer girilince ekrana * yazdırılması
        Serial.print("*");
        OLED.setTextXY(5, 0);
        OLED.putString("*");
        break;
      case 2:  // İkinci değer girilince ekrana * yazdırılması
        Serial.print("*");
        OLED.setTextXY(5, 1);
        OLED.putString("*");
        break;
      case 3:  // Üçüncü değer girilince ekrana * yazdırılması
        Serial.print("*");
        OLED.setTextXY(5, 2);
        OLED.putString("*");
        break;
      case 4:  // Dördüncü değer girilince ekrana * yazdırılması
        Serial.print("*");
        OLED.setTextXY(5, 3);
        OLED.putString("*");
        IsPassCorrect();
        ArrayIndex = 0;
        resetPass();
        break;
    }
  }
  delay(100);
}

void IsPassCorrect() {  // Şifre doğru mu?
  if (password[0] == inputvalue[0] && password[1] == inputvalue[1] && password[2] == inputvalue[2] && password[3] == inputvalue[3]) {
    neopixelWrite(RGBLED, 0, 128, 0);  // Şifre doğru ise yeşil led yanması
    OLED.setTextXY(3, 0);
    OLED.putString("sifre dogru   ");
    delay(2000);
  } else {
    neopixelWrite(RGBLED, 128, 0, 0);  // Şifre yanlış ise kırmızı led yanması
    Serial.println(" şifre yanlış");
    OLED.setTextXY(3, 0);
    OLED.putString("sifre yanlis   ");
    delay(2000);
    tone(D15, 500, 500);
    delay(500);
    tone(D15, 800, 500);
    delay(500);
  }
  digitalWrite(RGBLED, LOW);
  OLED.clearDisplay();  // OLED ekrandaki verilerin silinmesi
  delay(2500);
}

void resetPass() {  // Girilen şifre dizisinin sıfırlanması
  inputvalue[0] = NULL;
  inputvalue[1] = NULL;
  inputvalue[2] = NULL;
  inputvalue[3] = NULL;
  Serial.println(" yeniden şifre giriniz ");
  OLED.setTextXY(3, 0);
}
