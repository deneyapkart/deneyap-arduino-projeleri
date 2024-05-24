#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"
#include <Deneyap_HareketAlgilama.h>
#include <DeneyapCam.h>               //Zip olarak eklenmelidir. https://github.com/byrmeng/DeneyapCam
#include <UniversalTelegramBot.h>

static const char* WIFI_SSID = "************";
static const char* WIFI_PASS = "***********";

DeneyapCam::Resolution initialResolution;
String BOTtoken = "**********************";
String CHAT_ID = "*************";

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

bool sendPhoto = false;
int moveAlarm = 0;
bool flashState = LOW;

int move, timer1;
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

extern DeneyapCam::Resolution initialResolution;
Gesture movement;
void setup() {
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  Serial.begin(115200);
  Serial.println();
  movement.begin(0x32);
  delay(2000);
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi failure %d\n", WiFi.status());
    delay(5000);
    ESP.restart();
  }
  Serial.println("WiFi connected");
  {
    using namespace DeneyapCam;
    initialResolution = Resolution::find(1280, 720);  // Kamera pixel çözünürlüğü yazılır.
                                                      //  (800,600),(1024,768),(1280,720),(1280,1024) tavsiye edilir.
    Config cfg;
    cfg.setPins(pins::DeneyapKart1Av2);  // Kullandığınız kartın adını yazınız. Deneyap Kart için "DeneyapKart", 1A için "DeneyapKart1A" yazınız.
    cfg.setResolution(initialResolution);
    cfg.setJpeg(80);
    bool ok = Camera.begin(cfg);
    if (!ok) {
      Serial.println("camera initialize failure");
      delay(5000);
      ESP.restart();
    }
    Serial.println("camera initialize success");
  }
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, flashState);
  String welcome = "DK Telegrama bağlandı.\n";
  bot.sendMessage(CHAT_ID, welcome, "");
}
void loop() {
  move = movement.readGesture();
  if (moveAlarm == 1) {
    if (move == 1) {
      if (millis() - timer1 >= 2500) {
        Serial.println("Fotograf cekme talebi");
        String welcome = "Hareket algılandı, fotoğraf gönderiliyor.\n";
        bot.sendMessage(CHAT_ID, welcome, "");
        delay(50);
        sendPhotoTelegram();
        timer1 = millis();
      }
    }
  } else if (moveAlarm == 0) {
    delay(50);
  }
  if (sendPhoto) {
    Serial.println("Fotograf hazırlanıyor");
    sendPhotoTelegram();
    sendPhoto = false;
  }
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("cevabi geldi");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
void handleNewMessages(int numNewMessages) {
  Serial.print("Yeni mesaj: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Deneyap Telegram Botuna hoşgeldin " + from_name + ",\n";
      welcome += "Deneyap Karttan fotoğraf çekmek ve hırsız alarmı komutları; \n";
      welcome += "/flash : dahili ledi durumunu değiştirir,\n";
      welcome += "/SensorAktif : Hareket sensörü aktif edilir,\n";
      welcome += "/SensorPasif : Hareket sensörü pasife alınır,\n";
      welcome += "/photo : fotograf çeker.\n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }
    if (text == "/flash") {
      flashState = !flashState;
      digitalWrite(LED_BUILTIN, flashState);
      Serial.println("Led durumunu degistir");
    }
    if (text == "/photo") {
      sendPhoto = true;
      Serial.println("Fotograf cekme talebi");
    }
    if (text == "/SensorAktif") {
      moveAlarm = 1;
      String welcome = "Hareket sensörü aktif edildi.\n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }
    if (text == "/SensorPasif") {
      moveAlarm = 0;
      String welcome = "Hareket sensörü pesife alındı.\n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }
  }
}
String sendPhotoTelegram() {
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";

  camera_fb_t* fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Fotograf cekme basarisiz");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }
  Serial.println("Baglan " + String(myDomain));
  if (clientTCP.connect(myDomain, 443)) {
    Serial.println("Baglanti basarili");
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + CHAT_ID + "\r\n--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;
    clientTCP.println("POST /bot" + BOTtoken + "/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    clientTCP.println();
    clientTCP.print(head);
    uint8_t* fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n = 0; n < fbLen; n = n + 1024) {
      if (n + 1024 < fbLen) {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      } else if (fbLen % 1024 > 0) {
        size_t remainder = fbLen % 1024;
        clientTCP.write(fbBuf, remainder);
      }
    }
    clientTCP.print(tail);
    esp_camera_fb_return(fb);
    int waitTime = 10000;
    long startTimer = millis();
    boolean state = false;
    while ((startTimer + waitTime) > millis()) {
      Serial.print(".");
      delay(100);
      while (clientTCP.available()) {
        char c = clientTCP.read();
        if (state == true) getBody += String(c);
        if (c == '\n') {
          if (getAll.length() == 0) state = true;
          getAll = "";
        } else if (c != '\r')
          getAll += String(c);
        startTimer = millis();
      }
      if (getBody.length() > 0) break;
    }
    clientTCP.stop();
    Serial.println(getBody);
  } else {
    getBody = "Connected to api.telegram.org failed.";
    Serial.println("api.telegram.org baglanamadi ");
  }
  return getBody;
}