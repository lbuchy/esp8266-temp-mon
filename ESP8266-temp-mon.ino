#include <OneWire.h>

#include <DallasTemperature.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Phant.h>

//////////////////////
// WiFi Definitions //
//////////////////////
extern const char WiFiSSID[]; // Add the SSID to a new tab
extern const char WiFiPSK[]; // Add the PSK to a new tab
#define HOST_PREFIX "esp8266-temperature-mon-"

/////////////////
// Post Timing //
/////////////////
const unsigned long postRate = 30000;
unsigned long lastPost = 0;

////////////////
// Phant Keys //
////////////////
const char PhantHost[] = "data.sparkfun.com";
extern const char PublicKey[]; // Add the sparkfun public key to a new tab
// vvvvv DON'T COMMIT ME vvvvvvv
extern const char PrivateKey[]; // Add the sparkfun private key to a new tab. Don't commit this.
// ^^^^^ DON'T COMMIT ME ^^^^^^^

#define ONE_WIRE_BUS 2  // DS18B20 pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Arduino OTA Test");

  Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
  Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());

  ConnectToWifi();
}

void loop() {
  if (lastPost + postRate <= millis())
  {
    if (PostToPhant())
      lastPost = millis();
    else
      delay(100);    
  }
}

void ConnectToWifi() {
  WiFi.begin(WiFiSSID, WiFiPSK);
  if(WiFi.waitForConnectResult() == WL_CONNECTED){
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

float GetTemperature(bool* bSuccess) {
  float temp;
  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(0);
  *bSuccess = (temp > -127.0f && temp < 85.0f);
  return temp;
}

int PostToPhant() {
  Phant phant(PhantHost, PublicKey, PrivateKey);

    // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String postedID = "Thing-" + macID;

  float temp;
  bool bSuccess;
  temp = GetTemperature(&bSuccess);
  if (!bSuccess) {
    Serial.println("Failure to get temperature");
    return 0;
  }

  //phant.add("thing_id", postedID);
  phant.add("temperature", temp);

  Serial.print("Temp: ");
  Serial.println(temp);
  
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(PhantHost, httpPort)) 
  {
    Serial.println("Failure to connect to data.sparkfun.com");
    // If we fail to connect, return 0.
    return 0;
  }

  // If we successfully connected, print our Phant post:
  client.print(phant.post());
  while(client.available()){
    String line = client.readStringUntil('\r');
    //Serial.print(line); // Trying to avoid using serial
  }

  return 1;
}


