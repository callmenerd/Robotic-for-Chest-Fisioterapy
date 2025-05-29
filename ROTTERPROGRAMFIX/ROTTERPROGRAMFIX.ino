#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <util/atomic.h>
#include "SimpleKalmanFilter.h"

//pengaturan motor
//motor 1
#define RPWM1 9
#define LPWM1 8
//motor 2
#define RPWM2 11
#define LPWM2 10
//Encoder
#define encA1 18
#define encB1 3
#define encA2 19
#define encB2 2

LiquidCrystal_I2C lcd(0x27,20,4);

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {30, 32, 34, 36}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {22, 24, 26, 28}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// KEYPAD
bool UP, DOWN, ENTER, BACK, ERASE, NEXT = false;

//Menu Display
String a, b, c, d = "";
String timemenu = "";
int halaman = 0;
int menu = 1;
int submenu = 1;
char arg = NULL;
//Timing
String secon, minute;
int TIMING = NULL;

//pengaturan millis
unsigned long previousMillis_lcd = 0;
const long interval_lcd = 250;

unsigned long previousMillis_timer = 0;
const long interval_timer = 1000;

unsigned long previousMillis_input = 0;
const long interval_input = 10;

unsigned long waiting = 0;
const long wait_time = 1000;

long vibrate = 0;
const long vib_time = 3000;

unsigned long delayvib = 0;
const long delaytotal = 50;

bool vib = false;
bool FIRST = true;

//Motor
int pos1 = 0;
int pos2 = 0;
int pwm = 0;
bool RUN = false;
int setpoint = 120;
bool rotate[2] = {true, true};
float prevT1, prevT2 = 0;
float velocity1, velocity2 = 0;
const float PPR = 134.4;

//PID
const float Kp, Ki, Kd;
float SetPoint = 4;
float error, total_error = 0;
float control_signal = 0;
float last_error, last_time, T = 0;
float max_control = 255;
float min_control = 25;
long prevT = 0;
float eprev1 = 0;
float eprev2 = 0;
float eintegral1 = 0;
float eintegral2 = 0;
int direct = 1;
int PULSE = 0;

//Stepper
#define EN1 47
#define dirPin1 45
#define stepPin1 43

#define EN2 53
#define dirPin2 51
#define stepPin2 49

bool STEPUP, STEPDOWN = false;
const int PB_UP = 50;
const int PB_DOWN = 52;
const int stepsPerRevolution = 1600;

// parameter kalman filter
float V1, V2 = 0;
/*e_mea: Measurement Uncertainty 
  e_est: Estimation Uncertainty 
  q: Process Noise*/
float e_meaL = 2;
float e_estL = 2;
float qL = 0.5;
float e_meaA = 0.4;
float e_estA = 0.4;
float qA = 0.5;
SimpleKalmanFilter Vel1;
SimpleKalmanFilter Vel2;

float prev=0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM1, OUTPUT);
  pinMode(RPWM2, OUTPUT);
  pinMode(LPWM2, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(PB_UP, INPUT_PULLUP);
  pinMode(PB_DOWN, INPUT_PULLUP);

  pinMode(encA1, INPUT_PULLUP);
  pinMode(encA2, INPUT_PULLUP);
  pinMode(encB1, INPUT_PULLUP);
  pinMode(encB2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encA1), motor1, RISING);
  attachInterrupt(digitalPinToInterrupt(encA2), motor2, RISING);
  analogWrite(RPWM1, 0);
  analogWrite(LPWM1, 0);
  analogWrite(RPWM2, 0);
  analogWrite(LPWM2, 0);
  Vel1 = SimpleKalmanFilter(e_meaL, e_estL, qL);
  Vel2 = SimpleKalmanFilter(e_meaA, e_estA, qA);
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  lcd.setCursor(0,1);
  lcd.print("    ---ROTTER---    ");
  lcd.setCursor(0,2);
  lcd.print("    ---UNSOED---    ");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("    ---WELCOME---   ");
  delay(1000);
}

void loop() {
  if(millis() - previousMillis_lcd >= interval_lcd){
    lcd_display(); // Selengkapnya lihat LCD.ino
    previousMillis_lcd = millis();
  }
  if(millis() - previousMillis_input >= interval_input){
    get_keypad();
    navigation();
    previousMillis_input = millis();
  }
  V1 = Vel1.updateEstimate(velocity1);
  V2 = Vel2.updateEstimate(velocity2);
  if(millis() - vibrate <= vib_time){
    movement(-direct, direct, 125, 125);
    if(millis() - delayvib >= delaytotal){
      if(direct > 0 || direct < 0){
        direct = -direct;
        delayvib = millis();
      }
    }
  }
}

/*
  Serial.print("V1:");
  Serial.print(velocity1);
  Serial.print(",");
  Serial.print("V2:");
  Serial.print(velocity2);
  Serial.print(",");
  Serial.print("V1filt:");
  Serial.print(abs(V1));
  Serial.print(",");
  Serial.print("V2filt:");
  Serial.print(V2);
  Serial.print(",");
  Serial.print("POS1:");
  Serial.print(pos1);
  Serial.print(",");
  Serial.print("POS2:");
  Serial.print(pos2);
  Serial.println("");
*/