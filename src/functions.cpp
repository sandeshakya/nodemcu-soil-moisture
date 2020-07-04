#include "secrets.h"
#include "headers.h"

RTC_DATA_ATTR WiFiClientSecure net = WiFiClientSecure();
RTC_DATA_ATTR PubSubClient client(net);
RTC_DATA_ATTR int sensors_id[NUMBER_OF_SENSORS] = {1, 2};
RTC_DATA_ATTR int water_cycle;
RTC_DATA_ATTR int sensor_1_threshold;
RTC_DATA_ATTR int sensor_2_threshold;
RTC_DATA_ATTR int time_to_sleep;
RTC_DATA_ATTR bool device_updated;
RTC_DATA_ATTR char *topic_char = (char *)malloc(sizeof(SENSOR_PUBLISH_TOPIC) + 8);
RTC_DATA_ATTR char *ntpServer = "pool.ntp.org";
RTC_DATA_ATTR bool isPumpEnabled;
RTC_DATA_ATTR int sensors_data[NUMBER_OF_SENSORS];
RTC_DATA_ATTR time_t now;
RTC_DATA_ATTR int battery_level;
RTC_DATA_ATTR char jsonObj[256];
RTC_DATA_ATTR String topic;

StaticJsonDocument<128> CreatePublishObj(int id, int timestamp, int value)
{
  StaticJsonDocument<128> doc;
  doc["id"] = id;
  doc["timestamp"] = timestamp;
  doc["data"] = value;
  return doc;
}

void InitializeVariables()
{
  // Setting pins
  pinMode(INITPIN, OUTPUT);
  pinMode(PUBLISHLEDPIN, OUTPUT);
  pinMode(SENSOR1PIN, INPUT_PULLDOWN);
  pinMode(SENSOR2PIN, INPUT_PULLDOWN);
  pinMode(PUMP1PIN, OUTPUT);
  pinMode(PUMP2PIN, OUTPUT);
  pinMode(BATTERY_LEVEL_PIN, INPUT_PULLDOWN);

  water_cycle = 0;
  sensor_1_threshold = 0;
  sensor_2_threshold = 0;
  time_to_sleep = 300;
  device_updated = false;
  isPumpEnabled = false;
}

/**
 * IOT Subscribe callback function.
 * update water cycle and sensors thresholds
 */
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println(length);
  StaticJsonDocument<128> doc;
  deserializeJson(doc, (char *)payload);
  water_cycle = doc["state"]["reported"]["water_cycle"];
  sensor_1_threshold = doc["state"]["reported"]["sensor_1_threshold"];
  sensor_2_threshold = doc["state"]["reported"]["sensor_2_threshold"];
  time_to_sleep = doc["state"]["reported"]["time_to_sleep"];
  device_updated = true;
}

void ConnectToWiFi()
{
tryagain:
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int tries = 0;
  while (!WiFi.isConnected() && tries < 10)
  {
    Serial.print(".");
    delay(500);
    tries++;
  }
  if (!WiFi.isConnected())
  {
    Serial.println("Trying again...");
    goto tryagain;
  }
  Serial.println("Connected to WIFI!");
}

void ConnectToAWS()
{
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.setServer(AWS_IOT_ENDPOINT, 8883);
  client.setBufferSize(1024);
  client.setCallback(callback);

  int retries = 0;
  Serial.println("Connecting to AWS IOT...");

  while (!client.connect(DEVICE_NAME) && retries < AWS_MAX_RECONNECT_TRIES)
  {
    Serial.print(".");
    delay(500);
    retries++;
  }

  if (!client.connected())
  {
    Serial.println(" Timeout!");
    return;
  }
  Serial.print("Subscribe ");
  Serial.println(client.subscribe(SHADOW_UPDATE_TOPIC));
  Serial.println("Connected to AWS!");
}