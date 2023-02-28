/*    TelegramFotoGonder
 *    T3 DeneyapKart Teknik Ekibi tarafında yapıldı.->https://forum.deneyapkart.org/<-, ->https://deneyapkart.org/en/projeler.html?sayfa=1<-
 *
 *   Bu uygulamada 1 kamera soketi olan Deneyap Geliştirme Kartı ve 1 Deneyap Kamera kullanılmıştır. 
 *   Telegramdan girilen komut ile Deneyap Kameradan çekilen fotoğrafı telegrama göndermektedir.
 *   Telegramdan girilen komut ile Deneyap Kartın dahili ledin durumunu değiştirmektedir.
 *
 *    Telegram Botu Oluşturma
 *    - @BotFather botuna giriş yapılır. "/start" yazılır.
 *    - "/newbot" komutu girilir. Bot adı seçilir. Bot adını daha önce seçilmemiş olmaması ve sonunu bot ile bitmesi gerekmektedir.
 *    - Bot onaylandıktan sonra bot token HTTP API verilir ve bu Arduino koduna eklenir.
 *    - Kod  yükledikten sonra oluşturulan bot ekranına girilip "/start" yazılıp kullanılmaya başlanır. 
 *
 *    CHAT ID Alma
 *    - @myidbot botuna giriş yapılır. "/start" yazılır.
 *    - "/getid" komutu girilir. ID numarası alınır.
 *  ==============================================================================
 *    Bu uygulama örneği için "UniversalTelegramBot by Brian Lough" kütüphanesi indirilmelidir.  ->https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot<-
 *  ==============================================================================
*/
#include "WiFi.h"
#include "WiFiClientSecure.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <UniversalTelegramBot.h>

const char* ssid = "*****";      // Bağlantı kurulacak Wi-Fi ağı adı
const char* password = "*****";  // Bağlantı kurulacak Wi-Fi ağı şifresi
String BOTtoken = "*****";       // Telegram BOT Token (HTTP API Botfather tarafından alınacak)
String CHAT_ID = "*****";        // Chat ID numarası

bool sendPhoto = false;

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

bool flashState = LOW;

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void configInitCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = CAMD2;
  config.pin_d1 = CAMD3;
  config.pin_d2 = CAMD4;
  config.pin_d3 = CAMD5;
  config.pin_d4 = CAMD6;
  config.pin_d5 = CAMD7;
  config.pin_d6 = CAMD8;
  config.pin_d7 = CAMD9;
  config.pin_xclk = CAMXC;
  config.pin_pclk = CAMPC;
  config.pin_vsync = CAMV;
  config.pin_href = CAMH;
  config.pin_sscb_sda = CAMSD;
  config.pin_sscb_scl = CAMSC;
  config.pin_pwdn = -1;
  config.pin_reset = -1;
  config.xclk_freq_hz = 15000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Kamera baglantisi basarisiz 0x%x", err);
    return;
  }

  sensor_t* s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
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
      String welcome = "Deneyap Telegram Botuna hosgeldin , " + from_name + "\n";
      welcome += "Deneyap Karttan fotograf cekmek icin bu komutlari kullan \n";
      welcome += "/photo : fotograf ceker\n";
      welcome += "/flash : dahili ledi durumunu değiştirir \n";
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

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, flashState);

  configInitCamera();

  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Baglaniyor... ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Deneyap Kamera IP Adresi: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (sendPhoto) {
    Serial.println("Fotograf hsazirlaniyor");
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