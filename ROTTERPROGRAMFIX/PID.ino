void calc_PID(float setpoint, float input_val1, float input_val2, int arg){
  // PID constants
  int target;
  float kp1, ki1, kd1;
  float kp2, ki2, kd2;
  if(arg > 0){
    // set target position
    target = setpoint;
    kp1 = 1.55;
    kd1 = 0.025;
    ki1 = 0.001;
    kp2 = 1.5;
    kd2 = 0.0025;
    ki2 = 0.0;
  } else{
    // set target position
    //target = 25;
    // target = round(10*sin(prevT/1e6));
    // kp1 = 15;
    // kd1 = 0.25;
    // ki1 = 0.0;
    // kp2 = 15;
    // kd2 = 0.25;
    // ki2 = 0.00;
  }

  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;

  int pos1 = 0;
  int pos2 = 0;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    pos1 = input_val1;
    pos2 = input_val2;
  }
  
  // error
  int e1 = (target) - pos1;
  int e2 = (target) - pos2;

  // derivative
  float dedt1 = (e1-eprev1)/(deltaT);
  float dedt2 = (e2-eprev2)/(deltaT);

  // integral
  eintegral1 = eintegral1 + e1*deltaT;
  eintegral2 = eintegral2 + e2*deltaT;

  // control signal
  float u1 = kp1*e1 + kd1*dedt1 + ki1*eintegral1;
  float u2 = kp2*e2 + kd2*dedt2 + ki2*eintegral2;

  // motor power
  float pwr = fabs(u2);
  float pwl = fabs(u1);
  if( pwr > 160 ){
    pwr = 160;
  }
  if( pwl > 155 ){
    pwl = 155;
  }

  // motor direction
  int dir1 = 1;
  if(u1<0){
    dir1 = -1;
  }
  int dir2 = 1;
  if(u2<0){
    dir2 = -1;
  }

  // signal the motor
  movement(-dir1, dir2 ,pwl, pwr);

  // store previous error
  eprev1 = e1;
  eprev2 = e2;
}