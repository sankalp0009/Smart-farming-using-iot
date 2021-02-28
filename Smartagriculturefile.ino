#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <dht.h>

dht DHT;
#define DHT11_PIN 2
#define FIREBASE_HOST "smart-farming-84881-default-rtdb.firebaseio.com"
int WET= 16; // Wet Indicator at Digital PIN D0
int DRY= 2;  // Dry Indicator at Digital PIN D4
float temp,mois,hum;
int sense_Pin= 0; // Soil Sensor input at Analog PIN A0
int value= 0;

#ifndef STASSID
#define STASSID " "// Enter your WiFi Network's SSID
#define STAPSK  " " // Enter your WiFi Network's Password
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

void setup() {
  Serial.begin(9600);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    
    delay(1000);
    ESP.restart();
  }
  Firebase.begin(FIREBASE_HOST);
  delay(2000);

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
  temp=5;
  Serial.print("MOISTURE LEVEL : ");
   value= analogRead(sense_Pin);
   value=100.0 - (100.0 * (value / 1023.0));
   Serial.println(value);
   if(value<50)
   {
      digitalWrite(WET, HIGH);
   }
   else
   {
      digitalWrite(DRY,HIGH);
   }
  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  temp=DHT.temperature;
  mois=value;
  hum=DHT.humidity;
  Firebase.setFloat("/Members/UMleEiWKHCdI1oAnMfHQM0P8d7H2/temperature",temp);
   Firebase.setFloat("/Members/UMleEiWKHCdI1oAnMfHQM0P8d7H2/moisture",mois);
    Firebase.setFloat("/Members/UMleEiWKHCdI1oAnMfHQM0P8d7H2/humidity",hum);
   Serial.print("Humidity = ");
  Serial.println(DHT.humidity);

  if(Firebase.failed()){
    Serial.println(Firebase.error());
    return;
  }

   delay(1000);

}
