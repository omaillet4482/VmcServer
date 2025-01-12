
#include <ArduinoJson.h>

long lastMsg = 0;
char msg[50];
int value = 0;

const char* mqtt_topic_tempT_config = "homeassistant/sensor/grenier/sensorT/config";
const char* payload_tempT_config = "{\"dev_cla\":\"temperature\",\
\"stat_t\":\"wifi/temp/house/state\",\
\"unit_of_meas\":\"°C\",\
\"val_tpl\":\"{{ value_json.temp}}\",\
\"unique_id\":\"gr01_temp01\",\
\"name\":\"grenier_conf1\",\
\"dev\":{\
\"name\":\"grenier\",\
\"ids\":[\
\"gr01\"\
]\
}\
}";

const char* mqtt_topic_tempH_config = "homeassistant/sensor/grenier/sensorH/config";
const char* payload_tempH_config = "{\"dev_cla\":\"humidity\",\
\"stat_t\":\"wifi/temp/house/state\",\
\"unit_of_meas\":\"%\",\
\"val_tpl\":\"{{ value_json.hum}}\",\
\"unique_id\":\"gr01_hum01\",\
\"name\":\"grenier_conf2\",\
\"dev\":{\
\"name\":\"grenier\",\
\"ids\":[\
\"gr01\"\
]\
}\
}";
 
const char* mqtt_topic_temp_out = "wifi/temp/house/state";
const char* payload_temp_out = "{\"temp\":\"%.2f\",\"hum\":\"%.2f\"}";

const char* mqtt_topic_fan_out = "wifi/fan/house/state";
const char* mqtt_topic_fan_in = "wifi/fan/house/set";

const char* mqtt_topic_fan_config = "homeassistant/fan/grenier/fan01/config";

const char* payload_fan_config = "{\"pct_cmd_t\": \"wifi/fan/house/set\",\
\"pct_cmd_tpl\": \"{{ (value_json.value / 255 * 100)}}\",\
\"pct_stat_t\": \"wifi/fan/house/state\",\
\"pct_stat_tpl\": \"{{ value_json.value }}\",\
\"dev\": {\
\"ids\": [\
\"grenier_fan_01\"\
],\
\"name\": \"GRENIER_FAN\"\
},\
\"name\":\"GRENIER_FAN\",\
\"unique_id\": \"GRENIER_FAN_SWITCH\"\
}";


void callback(char* topic, byte* payload, unsigned int length) {
  
  Serial.println("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if(length < 4) {
  char payload_string[length + 1];
  int tmpLevel = 0;
  memcpy(payload_string, payload, length);
  payload_string[length] = '\0';
  tmpLevel = atoi(payload_string);
  if(tmpLevel > 1) {
     fanLevel = (tmpLevel * 100 ) / 255;
     manageFan();
     }
  }
 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe(mqtt_topic_fan_in);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loopMqtt(void) {
   if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 20000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "%d", fanLevel);
    Serial.println(msg);
    client.publish(mqtt_topic_fan_out, msg);

    snprintf (msg, 50, payload_temp_out,temperature,humid);
    client.publish(mqtt_topic_temp_out, msg);

    if(value % 10 == 1){
        Serial.println(payload_tempT_config); 
        Serial.println(strlen(payload_tempT_config)); 
        client.publish(mqtt_topic_tempT_config, payload_tempT_config);
        delay(10);
        Serial.println(payload_tempH_config);
        client.publish(mqtt_topic_tempH_config, payload_tempH_config);
    }  
  } else if (now > 86400000) {
    ESP.restart();
  }
}
