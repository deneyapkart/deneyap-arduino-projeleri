#include <DeneyapKartQRCodeReader.h>  //kütüphanenin içinde kamere  ile ilgili fonksiyonlar da ekli bu nedenle harici kamera kütüphanesi eklememize gerek yoktur.
                                      // kamera pin dahil
#include <Deneyap_Servo.h>
DKQRCodeReader reader(CAMERA_MODEL_DENEYAP_KART);
String  QRcode;
int i=0;
int b=0;
Servo servo1;  // 1. servo motor için class tanımlanması
Servo servo2;  // 2. servo motor için class tanımlanması

void QRCodeCalistir(void *data) {
  struct QRCodeData qrCodeData; //Qr codu okuyabilmemiz için class tanımlaması yapıyoruz.
  while (true) {
    if (reader.receiveQrCode(&qrCodeData, 100))  // bool değişkeni saydırıyor, milisaniye saydırıyor
    {
      Serial.println("QRCode Valid");
      if (qrCodeData.valid) //Qr code okuması yapabiliyorsa if döngüsü içerisine girecek
      {
        Serial.print("Payload: ");
        QRcode = (const char *)qrCodeData.payload;//QR code içerisinden okuma yapar
      } else {
        Serial.print("Invalid: ");
        //Serial.println((const char *)qrCodeData.payload);
        digitalWrite(D13, LOW); //Dijital pinlerimizi kapalı olarak başlatıyoruz
        digitalWrite(D12, LOW);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void setup() {
  Serial.begin(115200);
  Serial.println();
  servo1.attach(D12); //Servoların pin atamasını yapıyoruz
  servo2.attach(D13, 1); //Servoların pin atamasını yapıyoruz
  servo1.write(90); //Servoları 90 dereceden  başlatıyoruz
  servo2.write(90);
  reader.setup();  //reader fonksiyonu başlatıldı
  Serial.println("Setup QRCode Reader");
  reader.beginOnCore(1);
  Serial.println("Begin on Core 1");
  xTaskCreate(QRCodeCalistir, "onQrCode", 4 * 1024, NULL, 4, NULL); 
  //pinMode(2,OUTPUT);
}
void loop() {
  if (QRcode == "987654321010") // Eğer qr code içerisinde okunan değer buradaki belirtilene eşitse döngü içersine giriyor 
  {
   i=1; 
   b=0;// for döngüsü sayesinde iki kere aynı qr codu okumasını engelliyoruz
    
      if (i==1&b==0){
      neopixelWrite(RGBLED, 0, 255, 0); // Yeşil renk yanacak
      Serial.println(QRcode); //Ekrana içerisinde bulunan metni yazdıracak
      servo1.write(180); // Servo 90 dereceden 180 deerece konumuna gelecek ve kapı açılacak
      Serial.println("Door opened");
      delay(10);
      b=b+1;
    }
    else {
    Serial.println("Unvalid QR Code"); 
    }
    if (i==1&b==1){
      neopixelWrite(RGBLED, 0, 0, 255);
      Serial.println(QRcode);
      servo1.write(90);
      Serial.println("Door closed");
      delay(10);

    }
    else {
    Serial.println("Unvalid QR Code");
     }
  } 
   
  if (QRcode == "23146216612") {
    for (int i = 1; i < 2; i++) {
      neopixelWrite(RGBLED, 250, 0, 0);
      Serial.println(QRcode);
      servo2.write(150);
      Serial.println("Door locked");
      delay(10);
    }
    Serial.println("You have already read this QR COde");
  } else {
    Serial.println("Unvalid QR Code");
  }
}