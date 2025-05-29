void get_keypad(){
  reset_keypad();
  char key = keypad.getKey();

  (key >= '0' && key <= '9') ? arg = key : arg = NULL;
  switch(key){
    case 'A':
      UP = true;
      break;
    case 'B':
      DOWN = true;
      break;
    case 'C':
      NEXT = true;
      break;
    case 'D':
      BACK = true;
      break;
    case '#':
      ENTER = true;
      break;
    case '*':
      ERASE = true;
      break;
  }
  STEPUP = digitalRead(PB_UP);
  STEPDOWN = digitalRead(PB_DOWN);
}

void reset_keypad(){
  UP = false;
  DOWN = false;
  ENTER = false;
  BACK = false;
  ERASE = false;
  NEXT = false;
  STEPUP = false;
  STEPDOWN = false;
}