void rotate_stepper(int arg){
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
  if(arg > 10){
    if(!STEPUP){ digitalWrite(dirPin1, HIGH); digitalWrite(dirPin2, HIGH);}
    if(!STEPDOWN){ digitalWrite(dirPin1, LOW); digitalWrite(dirPin2, LOW);}
    for(int i = 1; i <= stepsPerRevolution; i++){
      digitalWrite(stepPin1, HIGH);
      digitalWrite(stepPin2, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin1, LOW);
      digitalWrite(stepPin2, LOW);
      delayMicroseconds(500);
    }
  } else{
    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, HIGH);
  }
}