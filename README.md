ESP8266 Temperature Monitor to Phant (sparkfun)
===============================================

This is an arduino sketch to upload directly to an ESP8266
connected with a Dallas temperature probe (on pin 2).

The default behaviour is to simply update the data stream
every 30 seconds with the current temperature.

You will need to install the esp8266 board configurations
into your Arduino IDE (tested with 1.6.5).  Add this to
Preferences->Additional Board Manager URLs

    http://arduino.esp8266.com/staging/package_esp8266com_index.json

You will also need to create a new tab (top right corner in IDE) and
fill out

    const char WiFiSSID[] = "SSID HERE";
    const char WiFiPSK[] = "PASSWORD HERE";

    const char PublicKey[] = "Public key here";
    const char PrivateKey[] = "Private key here";
