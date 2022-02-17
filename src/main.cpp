#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "FirebaseESP8266.h"

#define SSID "VEnus Z20"
#define PWD "azsxdcfv447"
#define DHTPIN 14
#define DHTTYPE DHT21

#define FIREBASE_HOST "nodemcu-9b1c1-default-rtdb.firebaseio.com/" // http:// veya https:// olmadan yazın
#define FIREBASE_AUTH "WNcSJaL959DpNdx15nkqa0DtPknsVyPDDH4kGK8u"

FirebaseData veritabanim;

ESP8266WebServer server(80);

IPAddress local_IP(192, 168, 1, 31);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

float h, t, f, hif, hic;

DHT dht(DHTPIN, DHTTYPE);

void dhtPrint();
void getDHT();
void cliPrint();
void handleFirebase();

void setup()
{
    Serial.begin(115200);
    Serial.println("DHTxx test!");

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PWD);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    /*
        if (!WiFi.config(local_IP, gateway, subnet))
        {
            Serial.println("Wifi Failed to configure");
        }
        */
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

    // 4. Ağ bağlantısı kesilirse tekrar bağlanmasına izin veriyoruz
    Firebase.reconnectWiFi(true);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    server.on("/test", HTTP_GET, cliPrint);
    server.begin();

    dht.begin();
}

void loop()
{
    server.handleClient();
    handleFirebase();
}

void dhtPrint()
{
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");

    delay(1000);
}

void getDHT()
{
    h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    f = dht.readTemperature(false);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f))
    {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Compute heat index in Fahrenheit (the default)
    hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    hic = dht.computeHeatIndex(t, h, false);
}

void get()
{
    getDHT();
}

void cliPrint()
{
    server.send(200, "text / plain", "test");
}

void handleFirebase()
{
    if (Firebase.getString(veritabanim, "/led")) // Alınacak veri tipine göre getInt, getBool, getFloat, getDouble, getString olarak kullanılabilir.
    {
        // bağlantı başarılı ve veri geliyor ise
        // Serial.print("String tipinde veri alımı başarılı, veri = ");
        // Serial.println(veritabanim.stringData());
        if (veritabanim.stringData() == "1")
        {
            digitalWrite(LED_BUILTIN, LOW);
            Firebase.setBoolAsync(veritabanim, "test", true);
        }
        else
        {
            digitalWrite(LED_BUILTIN, HIGH);
            Firebase.setBoolAsync(veritabanim, "test", false);
        }
    }
    else
    {
        // hata varsa hata mesajı ve nedeni yazdırılıyor

        Serial.print("Str verisi çekilemedi, ");
        Serial.println(veritabanim.errorReason());
    }
}