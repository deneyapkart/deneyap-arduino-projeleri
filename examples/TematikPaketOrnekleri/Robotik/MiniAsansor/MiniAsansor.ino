/*  MiniAsansor örneği
 *  T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *
 *  Bu uygulamada 1 Deneyap Geliştirme Kartı, 1 Çift Kanallı Motor Sürücü ve 1 Step Motor kullanılmıştır. 
 *    28BYJ-48 step motoruna göre yazılmıştır. Farklı bir step motor kullanırsanız gerekli değişiklikleri gerçekleştirin.
 *    STEP MOTOR MAVİ     -> B0+ (Terminal(Sağ Üst) çıkışı)
 *    STEP MOTOR PEMBE    -> A0- (Terminal(Sağ Alt) çıkışı)
 *    STEP MOTOR SARI     -> B0- (Terminal(Sağ Üst) çıkışı)
 *    STEP MOTOR TURUNCU  -> A0+ (Terminal(Sağ Alt) çıkışı)   
 *  "Serial Bluetooth Terminal" mobil uygulamasında girilen rakam değeri kadar step motor adım atmaktadır.
 *
 *  ==============================================================================
 *    Bu uygulama örneği için "Deneyap Cift Kanalli Motor Surucu by Turkish Technnology Team Foundation (T3)" kütüphanesi indirilmelidir. ->https://github.com/deneyapkart/deneyap-cift-kanalli-motor-surucu-arduino-library<-
 *  ==============================================================================
*/
#include <Deneyap_CiftKanalliMotorSurucu.h>  // Deneyap Çift Kanallı Motor Sürücü kütüphanesi eklenmesi
#include <BluetoothSerial.h>                 // BluetoothSerial için gerekli kütüphane

const int stepsPerRevolution = 4076;  // Tam tur adım sayısı, kullandığınız step motora göre tur sayısı değişiklik gösterebilir
char motorPosition = 0;

Stepper Stepper(stepsPerRevolution);  // Step için class tanımlanması
BluetoothSerial SerialBT;             // Bluetooth için class tanımlanması

void setup() {
  Serial.begin(115200);             // Seri haberleşme başlatılması
  Stepper.begin(0x16);              // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması, varsayılan: begin(0x16,500), frekans:500Hz
  SerialBT.begin("DeneyapKart1A");  // Bluetooth cihaz adı
  Stepper.setSpeed(60);             // Motorunuzun hızını 15 RPM olarak ayarlanması
}

void loop() {
  motorPosition = (char)SerialBT.read();
  if (SerialBT.available()) {
    Stepper.step(motorPosition);  // Step motorunu belirtilen konuma taşınması
    Serial.printf("Step motor %c adım ilerliyor.\n", motorPosition);
    delay(10);  // 10 milisaniye beklenmesi
  }
}
