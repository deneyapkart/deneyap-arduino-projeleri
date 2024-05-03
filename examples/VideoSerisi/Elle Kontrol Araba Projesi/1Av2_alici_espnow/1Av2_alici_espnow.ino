#include "WiFi.h" // wifi kütüphanesi
#include "esp_now.h" // 2 kart arasında wifi haberleşme başlatılması için kullandığımız kütüphane
#define MotorA1 D0 // Motor A1 kontrol pini tanımı
#define MotorA2 D1 // Motor A2 kontrol pini tanımı
#define MotorB1 D8 // Motor B1 kontrol pini tanımı
#define MotorB2 D9 // Motor B2 kontrol pini tanımı
int ileriHiz;  // İleri hızı tutacak değişken
// Veri yapısı tanımı: x ve y koordinatlarını içerir
typedef struct struct_message { 
  int x;
  int y;
} struct_message; 
struct_message myData;  // Alınan veriyi saklayacak struct değişkeni
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  // Alınan veriyi myData değişkenine kopyala
  memcpy(&myData, incomingData, sizeof(myData));
  // Y eksenine göre hareket kontrolü

  if (myData.y >= 10) { // Eğer Y koordinatı 10 veya daha büyükse
    myData.y = map(myData.y, 0, 90, 0, 255); // Y koordinatını 0-90 arasından 0-255 arasına yeniden haritalar
    if (myData.y >= 15) { // Eğer Y koordinatı 15 veya daha büyükse
      ileri();
    } else {
      dur(); // Değilse dur
    }
    ileriHiz = myData.y; // İleri hızı güncelle
    Serial.println("Y1: " + String(myData.y) + " Hiz: " + String(ileriHiz)); // Seri monitöre yazdırma
  } else if (myData.y <= -10) {  // Eğer Y koordinatı -10 veya daha küçükse
    myData.y = map(myData.y, -90, 0, 255, 0); // Y koordinatını -90-0 arasından 255-0 arasına yeniden haritalar
    if (myData.y >= 15) { // Eğer Y koordinatı 15 veya daha büyükse
      geri(); // Geri hareket 
    } else {
      dur(); // Değilse dur
    }
    ileriHiz = myData.y; // İleri hızı güncelle
    Serial.println("Y2: " + String(myData.y) + " Hiz: " + String(ileriHiz)); // Seri monitöre yazdırma
  }
  // X eksenine göre hareket kontrolü 
   else if (myData.x >= 10) {  // Eğer X koordinatı 10 veya daha büyükse
    myData.x = map(myData.x, 5, 85, 0, 255); // X koordinatını 5-85 arasından 0-255 arasına yeniden haritalar
    if (myData.x >= 15) { // Eğer X koordinatı 15 veya daha büyükse
      sol(); // Sola dönme
    } else {
      dur(); // Değilse dur 
    }
    Serial.println("X1: " + String(myData.x) + " Araç sola dönüyor."); // Seri monitöre yazdırma
  } else if (myData.x <= -10) { // Eğer X koordinatı -10 veya daha küçükse
    myData.x = map(myData.x, -85, -5, 255, 0);  // X koordinatını -85--5 arasından 255-0 arasına yeniden haritalar
    if (myData.x >= 15) {  // Eğer X koordinatı 15 veya daha büyükse
      sag(); // Sağa dönme
    } else {
      dur(); // Değilse Dur 
    }
    Serial.println("X2: " + String(myData.x) + " Araç sağa dönüyor.");  // Seri monitöre yazdırma
  } else { 
    dur();
  }
}
void setup() {
  Serial.begin(115200);  // Seri haberleşme başlatma
  WiFi.mode(WIFI_STA); // WiFi istemci modunu ayarlama
  // ESP-NOW başlatma
  if (esp_now_init() != ESP_OK) { // Eğer ESP-NOW başlatılamazsa
    Serial.println("ESP-NOW başlatılamadı");
    return;
  }
  // Veri alındığında çağrılacak fonksiyonun tanımlanması
  esp_now_register_recv_cb(OnDataRecv);
  
}
void loop() {
  delay(20); // 20 milisaniye bekleyerek döngüyü yavaşlatma
}
void ileri() {
  analogWrite(MotorA1, ileriHiz); 
  analogWrite(MotorA2, 0);
  analogWrite(MotorB1, ileriHiz);
  analogWrite(MotorB2, 0);
}
void geri() {
  analogWrite(MotorA1, 0);
  analogWrite(MotorA2, ileriHiz);
  analogWrite(MotorB1, 0);
  analogWrite(MotorB2, ileriHiz);
}
void sol() {
  analogWrite(MotorA1, 125);
  analogWrite(MotorA2, 0);
  analogWrite(MotorB1, 0);
  analogWrite(MotorB2, 125);
}
void sag() {
  analogWrite(MotorA1, 0);
  analogWrite(MotorA2, 125);
  analogWrite(MotorB1, 125);
  analogWrite(MotorB2, 0);
}
void dur() {
  analogWrite(MotorA1, 0);
  analogWrite(MotorA2, 0);
  analogWrite(MotorB1, 0);
  analogWrite(MotorB2, 0);
}
