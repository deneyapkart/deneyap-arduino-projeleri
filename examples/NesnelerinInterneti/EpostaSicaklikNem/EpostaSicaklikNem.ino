/*EpostaSicaklikNem

Mustafa "pxsty" kök tarafından eklendi --> github.com/pxsty0

Bu projede istenilen e-posta adresine deneyap sıcaklık ve nem ölçer sensorü aracılığıyla sıcaklık ve nem Verisi istenilen zaman aralığından gönderilmektedir

Bu uygulama örneği için "Deneyap_SicaklikNemOlcer" kütüphanesi indirilmelidir.  -> https://github.com/deneyapkart/deneyap-sicaklik-nem-olcer-arduino-library <-
Bu uygulama örneği için "ESP_Mail_Client" kütüphanesi indirilmelidir.  -> https://github.com/mobizt/ESP-Mail-Client <-

*/
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <Deneyap_SicaklikNemOlcer.h>

#define WIFI_SSID ""        // Wifi Adı
#define WIFI_PASS ""  // Wifi Şifresi

#define SMTP_SERVER ""               // E-posta sunucusu adresi
#define SMTP_PORT esp_mail_smtp_port_587           // E-posta sunucusu portu
#define SENDER_EMAIL ""       // E-posta Adresi
#define SENDER_PASSWORD ""         // E-posta şifresi
#define RECEIVER_EMAIL ""  // Alıcı e-posta adresi

int sendDelay = 60 * 1000;  // Kaç milisaniyede bir e-posta gönderileceğini belirtiyoruz

SMTPSession smtp;
Session_Config config;
TempHum tempHum;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  tempHum.begin(0x70);
  Serial.print("Wifi Baglaniliyor ...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("Baglanildi");
  MailClient.networkReconnect(true);

  smtp.debug(0);

  smtp.callback(smtpCallback);

  config.server.host_name = SMTP_SERVER;
  config.server.port = SMTP_PORT;
  config.login.email = SENDER_EMAIL;
  config.login.password = SENDER_PASSWORD;
  config.login.user_domain = "mydomain.net";

  config.time.ntp_server = "0.tr.pool.ntp.org,1.tr.pool.ntp.org";
  config.time.gmt_offset = 3;
  config.time.day_light_offset = 0;
}

void loop() {
  SMTP_Message message;

  message.sender.name = "Deneyap Kart";  // Gönderici Adı
  message.subject = "Sensör Verileri";  // E-posta konusu
  message.sender.email = RECEIVER_EMAIL;
  message.addRecipient("", RECEIVER_EMAIL);

  String msg = "";
  msg += "<html>";
  msg += "<head>";
  msg += "<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">";
  msg += "<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>";
  msg += "<link href=\"https://fonts.googleapis.com/css2?family=Poppins&display=swap\" rel=\"stylesheet\">";
  msg += "<link href=\"https://fonts.googleapis.com/css2?family=Roboto+Mono&display=swap\" rel=\"stylesheet\">";
  msg += "</head>";
  msg += "<body>";
  msg += "<div align=\"center\">";
  msg += "<span style=\"font-family: 'Poppins', sans-serif;font-size:24px\">";
  msg += "Sıcaklık : <span style=\"color:#ff5252\">" + String(tempHum.getTempValue()) + "°C</span>";
  msg += "</span>";
  msg += "<br/>";
  msg += "<span style=\"font-family: 'Poppins', sans-serif;font-size:24px\">";
  msg += "Nem : <span style=\"color:#ff5252\">" + String(tempHum.getHumValue()) + "%</span>";
  msg += "</span>";
  msg += "<hr width=\"50%\">";
  msg += "<span style=\"font-family: 'Roboto Mono', monospace;font-size:16px\">Deneyap Kart</span>";
  msg += "</div>";
  msg += "</body> </html>";

  message.html.content = msg;
  message.html.charSet = "utf-8";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;

  if (!smtp.connect(&config)) {
    Serial.println("Mail Sunucusuna Bağlanılamadı");
    return;
  }

  if (!smtp.isAuthenticated())
    Serial.println("Kimlik Dogrulama Yapilamadi");

  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Mail Gonderilemedi");
  delay(sendDelay);
}

void smtpCallback(SMTP_Status status) {
  if (status.success())
    Serial.println("Mail Gonderildi");

  smtp.sendingResult.clear();
}
