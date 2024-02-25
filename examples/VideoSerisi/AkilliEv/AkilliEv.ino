/*
 * Mimari Deneyap Kart'ta çalışacak şekilde hazırlanmıştır.
 * Eklenen Kütüphanelerin çoğu deneyap modüllerine özgüdür. Tüm DYK sensörlerinde I2C arayüzü kullanılmıştır.
 * Farklı bir geliştirme kartı ile entegre edilebilmesi için kütüphane class degişken ve parametrelerin ona göre düzenlenmesi gerekmektedir.
 * Kod tüklenmeden önce:
 *    Zorunlu   : WIFI_SSID - WIFI_PASSWORD - BOT_TOKEN - PRIVATE_CHAT_ID macro değerleri degişmelidir.
 *    Opsiyonel : Farklı koşullara göre değişiklik göstermesi durumunda TEMPRATURE_ALARM_VALUE - MIC_TRESHOLD gibi degerler düzenlenebilir.
 * Not:
 *    - Toprak nem sensörü alarmı pasife çekilmiştir(SOIL_MOISURE_ALARM_VALUE). İstenilen degere göre aktife çekilebilir.
 *    - Kart wifi baglantı bilgileri hatalı ise kendini sürekli resete götürecektir.
 * Warnings
 *    - Deneyap Kart v1 nodülünde dahili bulunan mp34dt05 mikrofon modülü kullanılmıştır. Kullandıgınız karta göre harici olarak baglayabilirsiniz.
 *    - Kod çalıştıgında I2C Initial fail durumunda I2C Baglantı hattınızı kontrol ediniz. Baglantıda sorun yoksa duman sensörünü pin header baglantı noktalarından ayrı olarak baglayınız.
 *    - Daha fazla sensör kullanılması gereken durumda I2c hattının bağlantısını ortak gnd ile haricen yapınız.
 * Auth: noumanimpra
 */
#include <Deneyap_BasincOlcer.h>
#include <Deneyap_SicaklikNemBasincOlcer.h>
#include <Deneyap_YagmurAlgilama.h>
#include <Deneyap_DumanDedektoru.h>
#include <Deneyap_OLED.h>
#include <Deneyap_ToprakNemiOlcer.h>
#include <Deneyap_HareketAlgilama.h>
#include <Deneyap_Role.h>
#include "WiFi.h"
#include "WiFiClientSecure.h"
#include "mp34dt05.h"
#include <UniversalTelegramBot.h>

// USR MACROS
#define WIFI_SSID "WIFI_ADI"                                 // Bağlantı kurulacak Wi-Fi ağı adı
#define WIFI_PASSWORD "PAROLA"                               // Bağlantı kurulacak Wi-Fi ağı şifresi
#define BOT_TOKEN "TELEGRAM:BOT:TOKEN:BOTFATHERDAN:ALINACAK" // Telegram BOT Token
#define PRIVATE_CHAT_ID "KENDI_CHAT_IDNIZ"
#define TEMPRATURE_ALARM_VALUE 30
#define SOIL_MOISURE_ALARM_VALUE 1000 // pasive
#define MIC_TRESHOLD 50

#define TEMPRATURE_LOG bot.sendMessage(chat_id, "🌡️ Ortam Sıcaklığı [" + temp_y + " °C]", "");
#define SOIL_LOG bot.sendMessage(chat_id, "🌱 Saksı nem oranı [" + temp_y + " ]", "");
#define MUTE_LOG bot.sendMessage(chat_id, "Sessiz Mod [AÇIK]", "")
#define UNMUTED_LOG bot.sendMessage(chat_id, "Sessiz Mod [KAPALI]", "")
#define SECURE_MODE_LOG bot.sendMessage(chat_id, "Güvenli Mod [AÇIK]", "")
#define UNSECURE_MODE_LOG bot.sendMessage(chat_id, "Güvenli Mod [KAPALI]", "")

#define ERR_LOGER(x) Serial.println("[" #x "] I2C CONNECTION ERRROR\n");
#define _INFO_LOGER(x) Serial.println(#x "\n");
#define RELAY_SET Role.RelayDrive(1);
#define RELAY_RESET Role.RelayDrive(0);

/*
 * bilinçli kullanım
 */
#define RAIN_LOG                                       \
    if (!m_rainProc())                                 \
    {                                                  \
        bot.sendMessage(chat_id, "🌧 yağmur var", ""); \
    }                                                  \
    else                                               \
    {                                                  \
        bot.sendMessage(chat_id, "😎 yağmur yok", ""); \
    }
#define CONN_ERR_HANDLER \
    if (m_connCnt > 125) \
    {                    \
        ESP.restart();   \
    }

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
AtmosphericPressure Pressure;
SHT4x TempHum;
Rain YagmurSensor;
OLED OLED;
SoilMoisture ToprakNemi;
Relay Role;
SmokeDetector DumanSensor;
Gesture Hareket;
FilterBeHp2 filter;

String t_photo_url = "https://cdnmagaza.deneyapkart.org/media/upload/userFormUpload/D5MZdGVOyg2hIhN6e2eeh1WUoLLjst3x.jpg";

typedef bool flag;

const unsigned long BOT_MTBS = 1000;
unsigned long bot_lasttime;

int m_connCnt = 0;
int samples[BUFFER_SIZE];
uint16_t temp_n;
float basinc;
float sicaklik;
float temp_x;
float displayTemp;

flag f_tempratureAlarm = false;
flag f_rainAlarm = false;
flag f_soilAlarm = false;
flag f_muteMode = false;
flag f_homeSecure = false;
flag f_relayOpen = false;
flag f_coldStart = true;
flag f_12cerrsmoke = false;

float m_rainProc()
{
    /*
    ReadRainDigital = 0 --> Yagmur Var
    ReadRainDigital = 1 --> Yagmur Yok
    */
    bool yagmurDurum = YagmurSensor.ReadRainDigital();
    uint16_t yagmurDeger = YagmurSensor.ReadRainAnalog();
    delay(10);
    if (!yagmurDurum)
        f_rainAlarm = true;
    return yagmurDurum;
}
int m_tempProc()
{
    TempHum.measure();
    sicaklik = TempHum.TtoDegC();
    return sicaklik;
}
int m_tempratureAlarmProc()
{
    if (m_tempProc() > TEMPRATURE_ALARM_VALUE)
        return 1;
    else
        return 0;
}
int m_soilMesureProc()
{
    uint16_t m_nem = ToprakNemi.ReadSoilMoisture();
    return m_nem;
}
int m_noiceDetectorAlarm()
{
    delay(10);
    size_t num_bytes_read = 0;
    micRead(&num_bytes_read, samples);
    if (num_bytes_read > 0)
    {
        int data = dataFilter(filter, num_bytes_read, samples);
        if (data > MIC_TRESHOLD)
            return 1;
        else
            return 0;
    }
}
int m_soilMesureAlarm()
{
    if (m_soilMesureProc() > SOIL_MOISURE_ALARM_VALUE)
        return 1;
    else
        return 0;
}
int m_smokeAlarmProc()
{
    bool dumanDurum = DumanSensor.ReadSmokeDigital();
    if (dumanDurum)
        return 1;
    else
        return 0;
}
int m_gestureAlarmProc()
{
    bool gestureState = Hareket.readGesture();
    if (gestureState)
        return 1;
    else
        return 0;
}
void handleNewMessages(int numNewMessages)
{
    for (int i = 0; i < numNewMessages; i++)
    {
        String chat_id = bot.messages[i].chat_id;
        String text = bot.messages[i].text;
        String temp_y;
        String from_name = bot.messages[i].from_name;
        if (from_name == "")
            from_name = "Guest";

        if (text == "/start")
        {
            String welcome = "🚀 Deneyap Kart Telegram Botuna hosgeldin " + from_name + ".\n";
            welcome += "🤖 Bu bot Deneyap Kart ekibi tarafından hazırlanmış olan akıllı ev sisteminin kontrol ve bilgi panelidir.\n\n";
            welcome += "/help komutu ile kullanabilecegin komutları inceleyebilirsin! \n";
            bot.sendPhoto(chat_id, t_photo_url, welcome);
        }
        if (text == "/yagmur")
        {
            RAIN_LOG;
        }
        if (text == "/sicaklik")
        {
            temp_x = m_tempProc();
            temp_y = String(temp_x);
            TEMPRATURE_LOG;
        }
        if (text == "/saksi")
        {
            temp_n = m_soilMesureProc();
            temp_y = String(temp_n);
            SOIL_LOG;
        }
        if (text == "/isikac")
        {
            RELAY_SET;
            f_relayOpen = true;
        }
        if (text == "/isikkapa")
        {
            RELAY_RESET;
            f_relayOpen = false;
        }
        if (text == "/mutealarm")
        {
            if (f_muteMode)
            {
                f_muteMode = false;
                bot.sendMessage(PRIVATE_CHAT_ID, "🔊 Sessiz Mod [Pasif]", "Markdown");
            }
            else
            {
                f_muteMode = true;
                bot.sendMessage(PRIVATE_CHAT_ID, "🔇 Sessiz Mod [Aktif]", "Markdown");
            }
        }
        if (text == "/securemode")
        {
            if (f_homeSecure)
            {
                f_homeSecure = false;
                bot.sendMessage(PRIVATE_CHAT_ID, "🥷🏻 Güvenli Mod [Pasif]", "Markdown");
            }
            else
            {
                f_homeSecure = true;
                bot.sendMessage(PRIVATE_CHAT_ID, "🥷🏻 Güvenli Mod [Aktif]", "Markdown");
            }
        }
        if (text == "/status")
        {
            RAIN_LOG;
            temp_x = m_tempProc();
            temp_y = String(temp_x);
            TEMPRATURE_LOG;
            temp_n = m_soilMesureProc();
            temp_y = String(temp_n);
            SOIL_LOG;
            if (f_muteMode)
                MUTE_LOG;
            else
                UNMUTED_LOG;
            if (f_homeSecure)
                SECURE_MODE_LOG;
            else
                UNSECURE_MODE_LOG;
        }
        if (text == "/isik")
        {
            String keyboardJson = "[[\"/isikac\", \"/isikkapa\"]]";
            bot.sendMessageWithReplyKeyboard(chat_id, "Işıklandırma sistemi için bir seçenek seçiniz.", "", keyboardJson, true);
        }
        if (text == "/help")
        {
            String keyboardJson = "[[{ \"text\" : \"DeneyapKart Web Sitesi\", \"url\" : \"https://deneyapkart.org\" }]]";
            String welcome = "🤖 Merhaba " + from_name + ".\n";
            welcome += "Akıllı ev komut seti.\n\n";
            welcome += "[🌧] - /yagmur    : Yağmur durrumu kontrolü\n";
            welcome += "[🌡️] - /sicaklik : Ortam sıcaklığı\n";
            welcome += "[🌱] - /saksi    : Sensör bilgileri\n";
            welcome += "[💡] - /isik     : Aydınlatma Menüsü\n";
            welcome += "[💡] - /isikac   : Işığı Aç\n";
            welcome += "[💡] - /isikkapa : Işığı Kapat\n";
            welcome += "[📃] - /status : Sensör bilgileri\n";
            welcome += "[🔇] - /mutealarm : Alarm modunu aktif/pasif \n";
            welcome += "[🥷🏻] - /securemode : Güvenlik modu aktif/pasif \n";
            welcome += "[📜] - /help : Tüm komut seti ve açıklamaları.\n";
            bot.sendMessage(chat_id, welcome, "Markdown");
            bot.sendMessageWithInlineKeyboard(chat_id, "Daha Fazlası İçin Denayap Kart Web Sitesini ziyaret edebilirsin.", "", keyboardJson);
        }
    }
}
void m_I2C_Init(void)
{
    Pressure.begin(0x76);
    if (!TempHum.begin(0X44))
    {
        ERR_LOGER(Sicaklik Sensoru);
        while (1)
            ;
    }
    if (!YagmurSensor.begin(0x2E))
    {
        delay(3000);
        ERR_LOGER(Yagmur Algilayici);
        while (1)
            ;
    }
    if (!ToprakNemi.begin(0x0F))
    {
        delay(3000);
        ERR_LOGER(Toprak Nem Algilayici);
        while (1)
            ;
    }
    if (!Role.begin(0x0C))
    {
        delay(3000);
        ERR_LOGER(ROLE);
        while (1)
            ;
    }
    if (!DumanSensor.begin(0x20))
    {
        delay(3000);
        ERR_LOGER(Duman Dedektoru); // while(1);
        f_12cerrsmoke = true;
    }
    if (!Hareket.begin(0x32))
    {
        delay(3000);
        ERR_LOGER(Hareket sensoru);
        while (1)
            ;
    }

    _INFO_LOGER(I2C Connections State-- PASS);
}
void m_WiFi_Init(void)
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        configTime(0, 0, "pool.ntp.org");
        time_t now = time(nullptr);
        while (now < 24 * 3600)
        {
            Serial.print(".");
            delay(100);
            now = time(nullptr);
            m_connCnt++;
            CONN_ERR_HANDLER;
        }
        Serial.println(now);
    }
}
void m_oled_Init(void)
{
    OLED.begin(0x7A);
    OLED.clearDisplay();
}
void d_displayProc(void)
{
    displayTemp = m_tempProc();
    OLED.setTextXY(2, 0);
    OLED.putString("Sicaklik:");
    OLED.setTextXY(2, 10);
    OLED.putInt(displayTemp);
    if (!m_rainProc())
    {
        OLED.setTextXY(3, 0);
        OLED.putString("Hava Yagmurlu");
    }
    else
    {
        OLED.setTextXY(3, 0);
        OLED.putString("Hava Acik     ");
    }
    OLED.setTextXY(4, 0);
    OLED.putString("Saksi nem:");
    OLED.setTextXY(4, 11);
    OLED.putInt(m_soilMesureProc());
    OLED.setTextXY(5, 0);
    OLED.putString("Isik: ");
    OLED.setTextXY(5, 7);
    if (f_relayOpen)
        OLED.putString("Acik  ");
    else
        OLED.putString("Kapali");
    OLED.setTextXY(6, 0);
    OLED.putString("Guvenli M.: ");
    if (f_homeSecure)
        OLED.putString("ON ");
    else
        OLED.putString("OFF");
}

void setup()
{
    Serial.begin(115200);
    m_I2C_Init();
    m_oled_Init();
    m_WiFi_Init();
    micBegin();
}

void loop()
{
    m_rainProc();
    m_tempratureAlarmProc();
    m_soilMesureProc();
    d_displayProc();

    if (millis() - bot_lasttime > BOT_MTBS)
    {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        while (numNewMessages)
        {
            handleNewMessages(numNewMessages);

            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        bot_lasttime = millis();
    }
    if (m_tempratureAlarmProc() && !f_muteMode)
    {
        // f_tempratureAlarm = false;
        bot.sendMessage(PRIVATE_CHAT_ID, "🌡️ 🔥 Anormal sıcaklık düzeyi ❗❗❗", "");
    }
    if (f_rainAlarm && !f_muteMode)
    {
        f_rainAlarm = false;
        bot.sendMessage(PRIVATE_CHAT_ID, "🌧 Yagmur alarmı ! ", "");
    }
    if (m_soilMesureAlarm() && !f_muteMode)
    {
        f_soilAlarm = false;
        bot.sendMessage(PRIVATE_CHAT_ID, "🌱 Çiçekleri Sula ! ", "");
    }
    if (m_smokeAlarmProc() && !f_muteMode && !f_12cerrsmoke)
    {
        bot.sendMessage(PRIVATE_CHAT_ID, "🔥 Duman algılandı ❗❗❗", "");
    }
    if (m_gestureAlarmProc() && !f_muteMode && f_homeSecure)
    {
        bot.sendMessage(PRIVATE_CHAT_ID, "🥷🏻 Evde hareket algılandı ❗❗❗", "");
    }
    if (m_noiceDetectorAlarm() && !f_muteMode && f_homeSecure)
    {
        bot.sendMessage(PRIVATE_CHAT_ID, "🗣️ Evde ses algılandı ❗❗❗", "");
    }

    if (f_coldStart)
    {
        f_coldStart = false;
        String welcome = "🚀 Deneyap Kart Telegram Botuna hosgeldin Nouman.\n";
        welcome += "🤖 Bu bot Deneyap Kart ekibi tarafından hazırlanmış olan akıllı ev sisteminin kontrol ve bilgi panelidir.\n\n";
        welcome += "/help komutu ile kullanabilecegin komutları inceleyebilirsin! \n";
        bot.sendPhoto(PRIVATE_CHAT_ID, t_photo_url, welcome);
    }
}
