#include "header.hpp"

void declare_pin(){
  pinMode(PB_RED, INPUT_PULLDOWN);
  pinMode(PB_WHITE, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(Trig1, OUTPUT);
  pinMode(Echo1, INPUT);
  pinMode(Trig2, OUTPUT);
  pinMode(Echo2, INPUT);
  pinMode(LightSensor, INPUT);
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to the WiFi network");
}

void connectToMQTTBroker() {
    while (!mqtt_client.connected()) {
        String client_id = "esp32-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
            // Publish message upon successful connection
            mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP32 ^^");
        } else {
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

float getDistance1() {
  float distance, duration;
  delay_prev = micros();
  while(micros()-delay_prev<=2){
    digitalWrite(Trig1, LOW);
  }
  delay_prev = micros();
  while(micros()-delay_prev<=10){
    digitalWrite(Trig1, HIGH);
  }
  digitalWrite(Trig1, LOW);
  duration = pulseIn(Echo1, HIGH);
  distance = 17-(duration*.0343)/2;
  return distance;
}

float getDistance2() {
  float distance, duration;
  delay_prev = micros();
  while(micros()-delay_prev<=2){
    digitalWrite(Trig2, LOW);
  }
  delay_prev = micros();
  while(micros()-delay_prev<=10){
    digitalWrite(Trig2, HIGH);
  }
  digitalWrite(Trig2, LOW);
  duration = pulseIn(Echo2, HIGH);
  distance = 17-(duration*.0343)/2;
  return distance;
}

void display(int expression){
  lcd.clear();
  switch (expression)
  {
  case 0:
    lcd.setCursor(0,0);
    lcd.print("PH : ");
    lcd.setCursor(5,0);
    lcd.print(phValue);
    lcd.setCursor(0,1);
    lcd.print("TDS : ");
    lcd.setCursor(5,1);
    ecValue = round(ecValue*100)/100;
    lcd.print(ecValue);
    break;
  case 1:
    lcd.setCursor(0,0);
    lcd.print("Nut 1:");
    lcd.setCursor(9,0);
    lcd.print("L(UV):");
    lcd.print(lampstate);
    lcd.setCursor(6,0);
    lcd.print(round(getDistance1()));
    lcd.setCursor(0,1);
    lcd.print("Nut 2:");
    lcd.setCursor(6,1);
    lcd.print(round(getDistance2()));
  default:
    break;
  }
}

void mqtt_handler(float a, float b){
  ;
}