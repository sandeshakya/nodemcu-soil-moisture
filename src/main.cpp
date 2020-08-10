#include "secrets.h"
#include "headers.h"

void setup()
{
  Serial.begin(115200);
  InitializeVariables();
  Serial.println("Starting...");

  digitalWrite(INITPIN, HIGH);
  ConnectToWiFi();
  ConnectToAWS();
  delay(2000);
  client.publish(SHADOW_PING_TOPIC, "{}");

  /* Get Readings */
  sensors_data[0] = 4095 - analogRead(SENSOR1PIN);
  sensors_data[1] = 4095 - analogRead(SENSOR2PIN);
  battery_level = analogRead(BATTERY_LEVEL_PIN);

  /* Publish Readings */
  for (int i = 0; i < NUMBER_OF_SENSORS; i++)
  {
    serializeJson(CreatePublishObj(sensors_id[i], sensors_data[i]), jsonObj);
    topic = SENSOR_PUBLISH_TOPIC + String(sensors_id[i]);
    topic.toCharArray(topic_char, topic.length());
  }
  serializeJson(CreatePublishObj(1, battery_level), jsonObj);
  client.publish(BATTERY_LEVEL_TOPIC, jsonObj);
  digitalWrite(PUBLISHLEDPIN, HIGH);
  Serial.println("Published readings!");

  /* Updating device config */
  while (!device_updated)
  {
    client.loop();
  }
  Serial.println("Deviced Updated!");
  esp_sleep_enable_timer_wakeup(time_to_sleep * uS_TO_S_FACTOR);

  // Water pump if needed
  isPumpEnabled = sensors_data[0] < sensor_1_threshold || sensors_data[1] < sensor_2_threshold;
  digitalWrite(PUMP1PIN, sensors_data[0] < sensor_1_threshold);
  digitalWrite(PUMP2PIN, sensors_data[1] < sensor_2_threshold);

  if (isPumpEnabled)
  {
    Serial.println("Pumping...");
    delay(water_cycle * 1000);
  }
  Serial.println("Going to sleep now...");

  esp_deep_sleep_start();
}

void loop()
{
}