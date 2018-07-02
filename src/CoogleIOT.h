/*
  +----------------------------------------------------------------------+
  | CoogleIOT for ESP8266                                                |
  +----------------------------------------------------------------------+
  | Copyright (c) 2017-2018 John Coggeshall                              |
  +----------------------------------------------------------------------+
  | Licensed under the Apache License, Version 2.0 (the "License");      |
  | you may not use this file except in compliance with the License. You |
  | may obtain a copy of the License at:                                 |
  |                                                                      |
  | http://www.apache.org/licenses/LICENSE-2.0                           |
  |                                                                      |
  | Unless required by applicable law or agreed to in writing, software  |
  | distributed under the License is distributed on an "AS IS" BASIS,    |
  | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      |
  | implied. See the License for the specific language governing         |
  | permissions and limitations under the License.                       |
  +----------------------------------------------------------------------+
  | Authors: John Coggeshall <john@thissmarthouse.com>                   |
  +----------------------------------------------------------------------+
*/

#ifndef COOGLEIOT_H
#define COOGLEIOT_H

#include "EEPROM_map.h"
#include "CoogleIOTConfig.h"
#include <FS.h>

#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Adafruit_NeoPixel.h"


#ifndef ARDUINO_ESP8266_ESP01
#include "DNSServer/DNSServer.h"
#endif

#include "LUrlParser/LUrlParser.h"

#include "CoogleEEPROM.h"
#include "CoogleIOTWebserver.h"

#include <user_interface.h>

typedef enum {
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	CRITICAL
} CoogleIOT_LogSeverity;

typedef void (*sketchtimer_cb_t)();

extern "C" void __coogle_iot_firmware_timer_callback(void *);
extern "C" void __coogle_iot_heartbeat_timer_callback(void *);
extern "C" void __coogle_iot_sketch_timer_callback(void *);

class CoogleIOTWebserver;

#define COOGLE_STATUS_PIXEL 0
#define COOGLE_STATUS_NEOPIXEL 1

#define STATUS_INIT         Adafruit_NeoPixel::Color(255,255,255) // bright white
#define STATUS_OK           Adafruit_NeoPixel::Color(127,127,127) // white
#define STATUS_ERROR        Adafruit_NeoPixel::Color(150,0,0)     // red
#define STATUS_WARNING      Adafruit_NeoPixel::Color(150,150,0)   // yellow 
#define STATUS_INIT_WIFI    Adafruit_NeoPixel::Color(0,0,50)      // pale blue
#define STATUS_WORKING_WIFI Adafruit_NeoPixel::Color(0,0,200)     // bright blue
#define STATUS_INIT_MQTT    Adafruit_NeoPixel::Color(0,50,0)      // pale green
#define STATUS_WORKING_MQTT Adafruit_NeoPixel::Color(0,200,0)     // bright green
#define STATUS_COMUNICATING Adafruit_NeoPixel::Color(150,0,150)   // purple

class CoogleIOT
{
    public:
		bool firmwareUpdateTick = false;
		bool heartbeatTick = false;
		bool sketchTimerTick = false;
		bool _restarting = false;

        CoogleIOT(int, int=COOGLE_STATUS_PIXEL);
        CoogleIOT();
        ~CoogleIOT();
        void loop();
        bool initialize();
        CoogleIOT& enableSerial(int, SerialConfig, SerialMode);
        CoogleIOT& enableSerial(int, SerialConfig);
        CoogleIOT& enableSerial(int);
        CoogleIOT& enableSerial();
        PubSubClient* getMQTTClient();
        bool serialEnabled();
        CoogleIOT& flashStatus(int, uint32_t = STATUS_OK);
        CoogleIOT& flashStatus(int, int, uint32_t);
        CoogleIOT& flashSOS();
        CoogleIOT& resetEEProm();
        void restartDevice();
        CoogleIOT& flashPixel(uint32_t);

        String getRemoteAPName();
        String getRemoteAPPassword();
        String getMQTTHostname();
        String getMQTTUsername();
        String getMQTTPassword();
        String getMQTTClientId();
        String getMQTTLWTTopic();
        String getMQTTLWTMessage();
        String getAPName();
        String getAPPassword();
        String getKonkerDeviceId();
        String getKonkerDevicePassword();

        String filterAscii(String);
        int getMQTTPort();
        String getFirmwareUpdateUrl();
        String getWiFiStatus();
        int getWiFiStatusCode();
        String getTimestampAsString();

        bool verifyFlashConfiguration();

        CoogleIOT& setMQTTPort(int);
        CoogleIOT& setMQTTHostname(String);
        CoogleIOT& setMQTTUsername(String);
        CoogleIOT& setMQTTPassword(String);
        CoogleIOT& setMQTTLWTTopic(String);
        CoogleIOT& setMQTTLWTMessage(String);
        CoogleIOT& setRemoteAPName(String);
        CoogleIOT& setRemoteAPPassword(String);
        CoogleIOT& setMQTTClientId(String);
        CoogleIOT& setAPName(String);
        CoogleIOT& setAPPassword(String);
        CoogleIOT& setFirmwareUpdateUrl(String);
        CoogleIOT& syncNTPTime(int, int);
        CoogleIOT& setKonkerDeviceId(String);
        CoogleIOT& setKonkerDevicePassword(String);

        CoogleIOT& warn(String);
        CoogleIOT& error(String);
        CoogleIOT& critical(String);
        CoogleIOT& log(String, CoogleIOT_LogSeverity);
        CoogleIOT& logPrintf(CoogleIOT_LogSeverity, const char *format, ...);
        CoogleIOT& debug(String);
        CoogleIOT& info(String);

        CoogleIOT& registerTimer(int, sketchtimer_cb_t);

        String buildLogMsg(String, CoogleIOT_LogSeverity);
        String getLogs(bool);
        String getLogs();
        File& getLogFile();

        bool mqttActive();
        bool dnsActive();
        bool ntpActive();
        bool firmwareClientActive();
        bool apStatus();

        void checkForFirmwareUpdate();
        int getRemoteWifiStatus(); 

        void setVersion(String);
        String getVersion();

    private:

        bool _serial;
        int _statusPin;
        Adafruit_NeoPixel* _statusPixel;
        String _version;

        HTTPUpdateResult firmwareUpdateStatus;
        time_t now;

#ifndef ARDUINO_ESP8266_ESP01
        DNSServer dnsServer;
#endif

        WiFiClient espClient;
        PubSubClient *mqttClient;
        CoogleEEProm eeprom;
        CoogleIOTWebserver *webServer;
        File logFile;
        int _remoteWifiStatus = 0;

        os_timer_t firmwareUpdateTimer;
        os_timer_t heartbeatTimer;
        os_timer_t sketchTimer;

        int sketchTimerInterval = 0;
        sketchtimer_cb_t sketchTimerCallback;

        int wifiFailuresCount;
        int mqttFailuresCount;

        bool mqttClientActive = false;
        bool dnsServerActive = false;
        bool ntpClientActive = false;
        bool _firmwareClientActive = false;
        bool _apStatus = false;

        void initializeLocalAP();
        void enableConfigurationMode();
        bool connectToSSID();
        bool initializeMQTT();
        bool connectToMQTT();
        
        String getEncryptionType(int thisType);
};

#endif
