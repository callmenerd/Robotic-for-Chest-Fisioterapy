void motor1(){
  int b = digitalRead(encB1);
  int increment = 0;
  if(b > 0){
    increment = 1;
  }
  else{
    increment = -1;
  }
  pos1 += increment;
  long currT1 = micros();
  float deltaT1 = ((float) (currT1 - prevT1))/1.0e6;
  velocity1 = (increment/deltaT1)/PPR;
  prevT1 = currT1;
}

void motor2(){
  int b = digitalRead(encB2);
  int increment = 0;
  if(b > 0){
    increment = 1;
  }
  else{
    increment = -1;
  }
  pos2 += increment;
  long currT2 = micros();
  float deltaT2 = ((float) (currT2 - prevT2))/1.0e6;
  velocity2 = (increment/deltaT2)/PPR;
  prevT2 = currT2;
}

void reset_enc(){
  pos1 = 0;
  pos2 = 0;
}

void movement(int dir1, int dir2, int PWML, int PWMR){
  if(dir1 < 0){
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, PWML);
  } else if(dir1 > 0){
    analogWrite(RPWM1, PWML);
    analogWrite(LPWM1, 0);
  } else{
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, 0);
  }
  if(dir2 < 0){
    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, PWMR);
  } else if(dir2 > 0){
    analogWrite(RPWM2, PWMR);
    analogWrite(LPWM2, 0);
  } else{
    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, 0);
  }
}