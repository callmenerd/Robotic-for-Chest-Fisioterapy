#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <SerialTransfer.h>
#include "DFRobot_ESP_EC.h"
#include "DFRobot_ESP_PH.h"

#include <WiFi.h>
#include <ArduinoOTA.h>
#include "PubSubClient.h"

//Message setting
#define phtopic "sensor/ph"
#define ectopic "sensor/ec"
#define uvtopic "sensor/uv"
#define watertopic1 "sensor/us_water1"
#define watertopic2 "sensor/us_water2"

#define nutrient_pump "actuator/pump"
#define light "actuator/lamp"
#define nutrient_pump_state "actuator/pump_state"
#define light_state "actuator/lamp_state"

#define I2C_SDA 21
#define I2C_SCL 22
#define PB_RED 4
#define PB_WHITE 5
#define LED_RED 25
#define LED_WHITE 33
#define RELAY_1 19
#define RELAY_2 18
#define Trig1 14
#define Echo1 27
#define Trig2 13
#define Echo2 12
#define LightSensor 32

#define PH_PIN 34
#define EC_PIN 35

#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value

#define light_thres 200
#define ec_thresh 100
#define PH_OFFSET -0.75

float  voltagePH,voltageEC,phValue,ecValue,temperature = 25;
// DFRobot_ESP_PH ph;
DFRobot_ESP_EC ec;

LiquidCrystal_I2C lcd(0x27, 16, 2);
SerialTransfer myTransfer;
unsigned long delay_prev = 0;
unsigned long prev_Time = 0;
unsigned long prev_LCD = 0;
unsigned long prevTime = 0;
volatile float prevVal = 0;

struct __attribute__((packed)) STRUCT {
  char z;
  float ECval;
} testStruct;

//Activate System
bool activate = false;
bool lampstate, pumpstate = false;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int idx = 0;

//WiFi Setting
const char *ssid = "Galaxy A02s7631";
const char *password = "qwerty989";

unsigned long prevT = 0;
float pH, EC, UV, water1, water2 = 0;
bool manual_lamp, manual_pump = false;

//MQTT Broker settings
const char *mqtt_broker = "broker.emqx.io";  // EMQX broker endpoint
const char *mqtt_topic = "capstone/test";     // MQTT topic
const char *mqtt_username = "test";  // MQTT username for authentication
const char *mqtt_password = "public";  // MQTT password for authentication
const int mqtt_port = 1883;  // MQTT port (TCP)
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;

static unsigned long timepoint = 0;

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void connectToWiFi();

void connectToMQTTBroker();

void mqttCallback(char *topic, byte *payload, unsigned int length);

float getDistance1();
float getDistance2();