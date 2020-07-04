#ifndef HEADERS_H
#define HEADERS_H

#include <WiFi.h>
#include <WiFiClientSecure.h> //handle encryption of certs
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

#define INITPIN 23
#define PUBLISHLEDPIN 22
#define SENSOR1PIN 35
#define SENSOR2PIN 34
#define PUMP1PIN 16
#define PUMP2PIN 17
#define BATTERY_LEVEL_PIN 32
#define uS_TO_S_FACTOR 1000000
#define NUMBER_OF_SENSORS 2

extern RTC_DATA_ATTR WiFiClientSecure net;
extern RTC_DATA_ATTR PubSubClient client;
extern RTC_DATA_ATTR int sensors_id[NUMBER_OF_SENSORS];
extern RTC_DATA_ATTR int water_cycle;
extern RTC_DATA_ATTR int sensor_1_threshold;
extern RTC_DATA_ATTR int sensor_2_threshold;
extern RTC_DATA_ATTR int time_to_sleep;
extern RTC_DATA_ATTR bool device_updated;
extern RTC_DATA_ATTR int sensors_data[NUMBER_OF_SENSORS];
extern RTC_DATA_ATTR int battery_level;
extern RTC_DATA_ATTR char jsonObj[256];
extern RTC_DATA_ATTR String topic;
extern RTC_DATA_ATTR char *topic_char;
extern RTC_DATA_ATTR char *ntpServer;
extern RTC_DATA_ATTR time_t now;
extern RTC_DATA_ATTR bool isPumpEnabled;

StaticJsonDocument<128> CreatePublishObj(int id, int timestamp, int value);
void callback(char *topic, byte *payload, unsigned int length);
void ConnectToWiFi();
void ConnectToAWS();
void InitializeVariables();
#endif