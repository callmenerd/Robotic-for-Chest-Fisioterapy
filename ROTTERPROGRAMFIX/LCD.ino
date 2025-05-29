void lcd_display(){
  lcd.clear();
  switch(halaman){
    case 0:
      if (menu == 1){
        a = "> Position Setting";
        b = "  Timer Setting";
        c = "    Next ";
      } else if(menu == 2){
        a = "  Position Setting";
        b = "> Timer Setting";
        c = "    Next ";
      } else if(menu == 3){
        a = "  Position Setting";
        b = "  Timer Setting";
        c = ">   Next ";
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("   ---MAIN MENU---  ");
      lcd.setCursor(0,1);
      lcd.print(a);
      lcd.setCursor(0,2);
      lcd.print(b);
      lcd.setCursor(0,3);
      lcd.print(c);
      break;
    
    case 1:
        lcd.setCursor(0,0);
        lcd.print("  ----POSITION----  ");
        lcd.setCursor(0,2);
        lcd.print("[UP] naik [DN] turun");
        if (!STEPUP){
          d = "^";
        } else if(!STEPDOWN){
          d = "v";
        } else{
          d ="stop";
        }
        lcd.setCursor(0,3);
        lcd.print(d);
      break;

    case 2:
      lcd.setCursor(0,0);
      lcd.print("  ------TIMER------  ");
      switch(submenu){
        case 1:
          lcd.setCursor(0,1);
          lcd.print("enter minutes:");
          lcd.setCursor(0,2);
          lcd.print("> ");
          lcd.print(minute);
          break;
        case 2:
          lcd.setCursor(0,1);
          lcd.print("enter second: ");
          lcd.setCursor(0,2);
          lcd.print("> ");
          lcd.print(secon);
          break;
      }
      break;

    case 3:
      if(millis() - waiting <= wait_time){
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("----WAIT A SECON----");
        pos1 = 0;
        pos2 = 0;
      } else{
        lcd.setCursor(0,0);
        (TIMING != 0) ? lcd.print("   ....RUNNING....  ") : lcd.print("    ....STOP....    ");
        lcd.setCursor(0, 1);
        lcd.print("Time: ");
        lcd.print((int)floor(TIMING/60));
        lcd.print(" min, ");
        (TIMING < 0) ? lcd.print(0) : lcd.print(TIMING%60);
        lcd.print(" sec");
        lcd.setCursor(0,3);
        (TIMING != 0) ? lcd.print("press * to cancel") : lcd.print("> press D to back");
        clock();
      }
      break;
  }
      
}