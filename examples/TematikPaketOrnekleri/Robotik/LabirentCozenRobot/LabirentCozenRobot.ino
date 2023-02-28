/*  LabirentCozenRobot
 *  T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *
 *  Bu uygulamada 1 Deneyap Geliştirme Kartı, 1 Deneyap Çift Kanallı Motor Sürücü, 3 adet Ultrasonik Mesafe Ölçer  kullanılmıştır. Deneyap Çift Kanallı Motor Sürücü için 9V pil ya da  güç kaynağı gerekmektedir.
 *  Ultrasonik mesafe ölçer ile önündeki engelleri algılmaktadır. Bu engelleri algılayarak labirenti çözmektedir.
 *  Kurduğunuz robot düzenine ve labirente göre belirlenen mesafe, hız ve dönüş yönü değerlerini değiştiriniz.
 *
 *  ==============================================================================
 *    Bu uygulama örneği için "Deneyap Cift Kanalli Motor Surucu by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-cift-kanalli-motor-surucu-arduino-library<-
 *    Bu uygulama örneği için "HCSR04 by Martin Sosic" kütüphanesi indirilmelidir. ->https://github.com/Martinsos/arduino-lib-hc-sr04<-
 *  ==============================================================================
*/
#include <Deneyap_CiftKanalliMotorSurucu.h>  // Deneyap Çift Kanallı Motor Sürücü için gerekli kütüphane
#include <HCSR04.h>                          // Ultrasonik mesafe ölçer sensörü için gerekli kütüphane

DualMotorDriver DualMotorDriver;  // DualMotorDriver için class tanımlanması

UltraSonicDistanceSensor UltraSonicDistanceSensor1(D0, D1);    // Ultrasonik sensor pin konfigurasyonu (trigPin, echoPin)
UltraSonicDistanceSensor UltraSonicDistanceSensor2(D4, D5);    // Ultrasonik sensor pin konfigurasyonu (trigPin, echoPin)
UltraSonicDistanceSensor UltraSonicDistanceSensor3(D12, D13);  // Ultrasonik sensor pin konfigurasyonu (trigPin, echoPin)

void setup() {
  Serial.begin(115200);         // Seri haberleşme başlatılması
  DualMotorDriver.begin(0x16);  // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması, varsayılan: begin(0x16,500), frekans:500Hz
}

void loop() {
  int RightMeasure = UltraSonicDistanceSensor1.measureDistanceCm();  // Sağ yöndeki ultrasonik mesafe okunması
  int LeftMeasure = UltraSonicDistanceSensor2.measureDistanceCm();   // Sol yöndeki ultrasonik mesafe okunması
  int FrontMeasure = UltraSonicDistanceSensor3.measureDistanceCm();  // Ön yöndeki ultrasonik mesafe okunması
  Serial.printf("Sağ mesafe: %d\n", RightMeasure);                   // Seri port ekranına sağ mesafe verisinin yazdırılması
  Serial.printf("Sol mesafe: %d\n", LeftMeasure);                    // Seri port ekranına sol mesafe verisinin yazdırılması
  Serial.printf("Ön mesafe: %d\n", FrontMeasure);                    // Seri port ekranına ön mesafe verisinin yazdırılması
  DualMotorDriver.MotorDrive(MOTOR1, 0, 0);                          // Birinci motor çalıştırılması. %0 dutycyle hızı ile geri yönde hareket edilmesi
  DualMotorDriver.MotorDrive(MOTOR2, 0, 0);                          // İkinci motor çalıştırılması.  %0 dutycyle hızı ile geri yönde hareket edilmesi

  /* kurduğunuz robot düzenine ve labirente göre belirlenen mesafe, hız ve dönüş yönü değerlerini değiştiriniz */
  if (FrontMeasure > 8) {
    if (RightMeasure > 7 && RightMeasure < 13) {
      DualMotorDriver.MotorDrive(MOTOR1, 45, 1);  // Birinci motor çalıştırılması. %45 dutycyle hızı ile ileri yönde hareket edilmesi
      DualMotorDriver.MotorDrive(MOTOR2, 50, 0);  // İkinci motor çalıştırılması.  %50 dutycyle hızı ile geri yönde hareket edilmesi
    }
    if (RightMeasure >= 13) {
      DualMotorDriver.MotorDrive(MOTOR1, 100, 1);  // Birinci motor çalıştırılması. %100 dutycyle hızı ile ileri yönde hareket edilmesi
      DualMotorDriver.MotorDrive(MOTOR2, 30, 0);   // İkinci motor çalıştırılması.  %30 dutycyle hızı ile geri yönde hareket edilmesi
    }
    if (RightMeasure <= 7) {
      DualMotorDriver.MotorDrive(MOTOR1, 30, 1);
      DualMotorDriver.MotorDrive(MOTOR2, 100, 0);
    }
  }
  if (LeftMeasure <= 20 && RightMeasure > 20 && FrontMeasure <= 8) { Right(); }
  if (LeftMeasure > 20 && RightMeasure > 20 && FrontMeasure <= 8) { Right(); }
  if (RightMeasure <= 20 && LeftMeasure > 20 && FrontMeasure <= 8) { Left(); }
  if (RightMeasure <= 20 && LeftMeasure <= 20 && FrontMeasure <= 8) { FullTurn(); }
  delay(10);
}

/* kurduğunuz robot düzenine ve labirente göre belirlenen hız ve dönüş yönü değerlerini değiştiriniz */
void Left() {
  DualMotorDriver.MotorDrive(MOTOR1, 45, 0);
  DualMotorDriver.MotorDrive(MOTOR2, 45, 0);
  delay(700);
}

/* kurduğunuz robot düzenine ve labirente göre belirlenen hız ve dönüş yönü değerlerini değiştiriniz */
void Right() {
  DualMotorDriver.MotorDrive(MOTOR1, 45, 1);
  DualMotorDriver.MotorDrive(MOTOR2, 45, 1);
  delay(800);
}

/* kurduğunuz robot düzenine ve labirente göre belirlenen hız ve dönüş yönü değerlerini değiştiriniz */
void FullTurn() {
  DualMotorDriver.MotorDrive(MOTOR1, 45, 1);
  DualMotorDriver.MotorDrive(MOTOR2, 45, 1);
  delay(1200);
}
