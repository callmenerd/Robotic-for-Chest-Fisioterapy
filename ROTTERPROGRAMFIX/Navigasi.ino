void navigation(){
  if(halaman != 0){
    switch(menu){
      case 1:
        (!STEPUP || !STEPDOWN) ? rotate_stepper(123) : rotate_stepper(0);
        if(BACK) halaman = 0;
        break;
      case 2:
        SetTimer();
        break;
      case 3:
        if(FIRST){
          waiting = millis();
          FIRST = false;
        }
        if(ERASE){ vibrate = -1;TIMING = NULL;}
        if(BACK){secon = ""; minute = ""; halaman = 0; FIRST = true;}
        break;
    }
  } else{
    if(UP) menu--;
    if(DOWN) menu++;
    if(menu > 3) menu == 1;
    if(menu < 1) menu == 3;
    RUN = false;
  }
  if(NEXT) halaman = menu;
}

void clock(){
  if(millis() - previousMillis_timer >= interval_timer){
    TIMING--;
    previousMillis_timer = millis();
  }
  if(TIMING == 0) vibrate = millis();
  if(TIMING < -1){TIMING = -1;}
  if(TIMING > 0 && TIMING != NULL){
    calc_PID(PULSE, pos1, pos2, 150);
  } else{
    movement(0, 0, 0, 0);
  }
}
 
void SetTimer(){
  if(UP) submenu++;
  if(DOWN) submenu--;
  if(BACK){
    secon = "";
    minute = "";
    halaman = 0;
  } else if(ENTER){
    lcd.clear();
    lcd.setCursor(0,2);
    lcd.print("____TIMER IS SET____");
    delay(500);
    TIMING = minute.toInt()*60 + secon.toInt() + 1;
    PULSE = (TIMING - 1)*PPR*4;
    secon = "";
    minute = "";
    halaman = 0;
  }
  if(submenu > 2) submenu = 1;
  if(submenu < 1) submenu = 2;
  if(submenu == 1){
    if(arg != NULL) minute += arg;
    if(ERASE) minute.remove(minute.length()-1);
  } else if(submenu == 2){
    if(arg != NULL) secon += arg;
    if(ERASE) secon.remove(secon.length()-1);
  }
}