#include "function_file.hpp"

void setup() {
  Serial.begin(115200);
  declare_pin();
  digitalWrite(RELAY_1, HIGH);
  pinMode(PH_PIN, INPUT);
  // ph.begin();
  ec.begin();
  connectToWiFi();
  ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
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
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(mqttCallback);
  connectToMQTTBroker();
  lcd.init(I2C_SDA, I2C_SCL);
	lcd.backlight();
	lcd.setCursor(0,0);
	lcd.print("SMART_HIDROPONIK");
	lcd.setCursor(0,1);
	lcd.print("----------------");
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    String msg;
    for (unsigned int i = 0; i < length; i++) {
        msg += (char) payload[i];
        Serial.print(msg);
    }
    if((char)payload[0] == '1'){
      manual_pump = true;
    } else if ((char)payload[0] == '2'){
      manual_pump = false;
    }

    if((char)payload[0] == '3'){
      manual_lamp = true;
    } else if ((char)payload[0] == '4'){
      manual_lamp = false;
    }
    Serial.println();
    Serial.println("-----------------------");
}

void get_display(){
  if (digitalRead(PB_RED)){
    idx++;
  } else if(digitalRead(PB_WHITE)){
    idx--;
  }
  if(idx>1){
    idx = 0;
  } else if(idx<0){
    idx = 1;
  }
}

void activate_light_pump(){
  if(manual_pump){
    pumpstate = 1;
    digitalWrite(RELAY_1, LOW);
  } else{
    Serial.println(ecValue);
    if(ecValue<ec_thresh){
      pumpstate = 1;
      if((millis()-prevTime)<15000){
        if((millis()-prevTime)<4000){
          digitalWrite(RELAY_1, LOW);
        } else{
          digitalWrite(RELAY_1, HIGH);
        }
      } else{
        prevTime = millis();
      }
    } else{
      digitalWrite(RELAY_1, HIGH);
      pumpstate = 0;
    }
  }

  if(manual_lamp){
    lampstate = 1;
    digitalWrite(RELAY_2, LOW);
  } else{
    if(UV){
      lampstate = 1;
      digitalWrite(RELAY_2, LOW);
    } else{
      lampstate = 0;
      digitalWrite(RELAY_2, HIGH);
    }
  }
}

void loop() {
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(PH_PIN);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*3.3 / 4096.0 / 6; //convert the analog into millivolt
  phValue=3.5*phValue;                      //convert the millivolt into pH value

  phValue = phValue + PH_OFFSET;
  ArduinoOTA.handle();
  float a = getDistance1();
  float b = getDistance2();
  char cmd[10];
  if(millis()-timepoint>100U){                          //time interval: 1s
    timepoint = millis();
           // convert voltage to pH with temperature compensation
    Serial.print("pH:");
    Serial.print(phValue,2);
    voltageEC = analogRead(EC_PIN)/ESPADC * ESPVOLTAGE;
    ecValue = ec.readEC(voltageEC,temperature)*1000/2.2;       // convert voltage to EC with temperature compensation
    Serial.print(manual_lamp);
    Serial.print(" | ");
    Serial.println(UV);
  }
  if (!mqtt_client.connected()) {
    connectToMQTTBroker();
  }
  mqtt_client.loop();
  if(millis() - prevT > 100){
    pH = phValue;
    EC = ecValue;
    UV = digitalRead(LightSensor);
    water1 = a;
    water2 = b;
    mqtt_client.publish(phtopic, String(pH).c_str());
    mqtt_client.publish(ectopic, String(EC).c_str());
    mqtt_client.publish(uvtopic, String(UV).c_str());
    mqtt_client.publish(watertopic1, String(water1).c_str());
    mqtt_client.publish(watertopic2, String(water2).c_str());

    snprintf (msg, MSG_BUFFER_SIZE, "%d", pumpstate);
    mqtt_client.publish(nutrient_pump_state, msg);
    snprintf (msg, MSG_BUFFER_SIZE, "%d", lampstate);
    mqtt_client.publish(light_state, msg);
  }
  mqtt_client.subscribe(nutrient_pump);
  mqtt_client.subscribe(light);
  get_display();
  if(millis()-prev_LCD>250){
    lcd.clear();
    switch (idx)
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
    prev_LCD = millis();
  }
  activate_light_pump();
  if(phValue>8 || phValue<6.5){
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_WHITE, LOW);
  } else{
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_WHITE, HIGH);
  }
}