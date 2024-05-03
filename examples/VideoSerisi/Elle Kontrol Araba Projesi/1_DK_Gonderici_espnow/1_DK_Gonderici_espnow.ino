#include "WiFi.h"                                                     // WiFi kütüphanesini ekler
#include "esp_now.h"                                                  // ESP-NOW kütüphanesini ekler, haberleşme için kullanılır
#include "lsm6dsm.h"                                                  // IMU kütüphanesi tanımlandı
uint8_t broadcastAddress[] = { 0xF4, 0x12, 0xFA, 0xDF, 0x40, 0xA8 };  // Veri yayın adresi tanımlanır( MAC adresi)
float accelX, accelY, accelZ, accAngleY, accAngleX;                   // IMU değeri değişken tanımlaması yapıldı

// Veri yapısı tanımı: x, y ve c değerlerini içerir

typedef struct struct_message {
  int x;
  int y;
  int c;
} struct_message;
struct_message myData;  // Alınan veriyi saklayacak struct değişkeni tanımlanır

esp_now_peer_info_t peerInfo; // ESP-NOW ile ilgili yapı tanımlanır 

// Veri gönderildiğinde çağrılacak fonksiyon
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Veri gönderme başarılı" : "Veri gönderme başarısız");
}
LSM6DSM IMU;  // IMU için class(nesne) tanımlanması 
void setup() {
  Serial.begin(115200); // Seri haberleşme başlatılır
  IMU.begin();  // IMU haberleşme başlatılır
  WiFi.mode(WIFI_STA); // WiFi istemci moduna ayarlanır
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW başlatılamadı");
    return;
  }
  esp_now_register_send_cb(OnDataSent); // Veri gönderme geri çağırma işlevi tanımlanır 

  // Veri yayın adresi tanımlanır

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // ESP-NOW eşleştirmesi yapılır
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Eşleşme başarısız");
    return;
  }
}
void loop() {
  imuread(); // IMU'dan veri okunur
  myData.x = accAngleX; // X değeri ayarlanır
  myData.y = accAngleY; // Y değeri ayarlanır

  // Veri gönderilir
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
  delay(150);
}
void imuread() {
  accelX = IMU.readFloatAccelX();  // sensörden gelen X, Y, Z verileri okundu
  accelY = IMU.readFloatAccelY();
  accelZ = IMU.readFloatAccelZ();

//Açı değeri hesaplanır
  accAngleX = atan(accelX / sqrt(pow(accelY, 2) + pow(accelZ, 2) + 0.001)) * 180 / PI;  // Matematik işlemi ile sensör verileri -90-+90 arasında açı değerlerine dönüştürüldi. // X ekseni açı değeri hesaplanır
  accAngleY = atan(-1 * accelY / sqrt(pow(accelX, 2) + pow(accelZ, 2) + 0.001)) * 180 / PI; // Y ekseni açı değeri hesaplanır
  
// Açı değerleri seri port üzerinden gösterilir
  Serial.print(" AngleY: " + String(accAngleY));
  Serial.println(" AngleX: " + String(accAngleX));
}
