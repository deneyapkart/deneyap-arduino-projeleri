/*   KapiKoluSistemi
 *   T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *
 *   Bu uygulamada 1 Deneyap Geliştirme Kartı, 1 Deneyap Dokunmatik Tuş Takımı modülü, 1 Deneyap OLED Ekran modülü ve 1 Deneyap Röle kullanılmıştır. 
 *   4 haneli rakam girilerek şifre belirlenir.
 *   Kullanıcı Oled ekranında "sifre giriniz" yazısını gözlemleyecektir. Her girdiği şifre karakterinde Oled ekranda * karakteri yazılacaktır.
 *   4 haneli doğru şifreyi girdiğinde oled ekranda "sifre dogru" yazacaktır, Röle açılacaktır, Deneyap Kart 1A dahili RGB ledi yeşil yanacaktır. Bu süreç 2 saniye sürecektir.
 *   2 saniyeden sonra led sönecek röle kapanacak ve tekrar şifre girilmesi istenecektir.
 *   Şifre yanlış girildiğinde oled ekranda "sifre yanlis" yazacaktır. Dahili RGB ledi olan Deneyap Geliştrme Kartın kırmızı yanacaktır. Bu süreç 2 saniye sürecektir. 
 *   Dokunmatik Tuş Takımında basma süresine bağlı olarak birden fazla okuma yapabilir.
 *
 *  ==============================================================================
 *    Bu uygulama örneği için "Deneyap Dokunmatik Tuş Takımı by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-dokunmatik-tus-takimi-arduino-library<-
 *    Bu uygulama örneği için "Deneyap Röle by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-role-arduino-library<-
 *    Bu uygulama örneği için "Deneyap OLED by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-oled-ekran-arduino-library<-
 *  ==============================================================================
*/
#include <Deneyap_DokunmatikTusTakimi.h>  // Deneyap Dokunmatik Tuş Takımı kütüphanesi eklenmesi
#include <Deneyap_Role.h>                 // Deneyap Röle kütüphanesi eklenmesi
#include <Deneyap_OLED.h>                 // Deneyap OLED Ekran kütüphanesinin eklenmesi

int password[] = { 0, 3, 0, 3 };  // Belirlenen şifre
int inputvalue[4];                // Girilen şifre
int ArrayIndex = 0;               // Sayaç

Keypad Keypad;  // Keypad için class tanımlanması
Relay Role;     // Relay için class tanımlanması
OLED OLED;      // OLED için class tanımlanması

//void IsPassCorrect();  // IsPassCorrect fonksiyonunun prototipi
//void resetPass();      // resetPass fonksiyonunun prototipi

void setup() {
  Serial.begin(115200);  // Seri haberleşme başlatılması
  Keypad.begin(0x0E);    // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  Role.begin(0x0C);      // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  OLED.begin(0x7A);      // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
  OLED.clearDisplay();   // OLED ekrandaki verilerin silinmesi
}

void loop() {
  Serial.println("Şifre giriniz");
  OLED.setTextXY(3, 0);
  OLED.putString("sifre giriniz");
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
    Serial.println(" kapı açılıyor ... ");  // Şifre doğru ise kapı açılması
    Role.RelayDrive(1);                     // Röle açılması
    neopixelWrite(RGBLED, 0, 128, 0);       // Şifre doğru ise yeşil led yanması
    OLED.setTextXY(3, 0);
    OLED.putString("sifre dogru   ");
    delay(2000);
  } else {
    neopixelWrite(RGBLED, 128, 0, 0);  // Şifre yanlış ise kırmızı led yanması
    Serial.println(" şifre yanlış");
    OLED.setTextXY(3, 0);
    OLED.putString("sifre yanlis   ");
    delay(2000);
  }
  Serial.println("kapı kapalı");
  neopixelWrite(RGBLED, 0, 0, 0);
  Role.RelayDrive(0);   // Röle kapanması
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
