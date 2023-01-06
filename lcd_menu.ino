#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

// variable button
#define button_start 14
#define button_cancel 27
#define button_up 26
#define button_down 25

int button_state_start = 0;
int button_state_cancel = 0;
int button_state_up = 0;
int button_state_down = 0;

int menu = 1;

void welcome(){
  lcd.setCursor(4,0);
  lcd.print("PANASONIC");
  lcd.setCursor(5,1);
  lcd.print("WELCOME");
  delay(3000);
}

void update_start_menu(){
  switch(menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(">START");
      lcd.setCursor(0,1);
      lcd.print(" SETTING"); 
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" START");
      lcd.setCursor(0,1);
      lcd.print(">SETTING"); 
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(">SETTING");
      lcd.setCursor(0,1);
      lcd.print(" STATUS"); 
      break;
     case 4:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" SETTING");
      lcd.setCursor(0,1);
      lcd.print(">STATUS"); 
      break;
     case 5:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(">STATUS");
      lcd.setCursor(0,1);
      lcd.print(" TARGET"); 
      break;
     case 6:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" STATUS");
      lcd.setCursor(0,1);
      lcd.print(">TARGET"); 
      break;
     case 7:
      menu = 6;
      break;
  }
}

void start_button_operation(){
    button_state_down = digitalRead(button_down);
    button_state_up = digitalRead(button_up);
    
  if(!button_state_down){
    menu++;
    update_start_menu();
    Serial.print("state_down: ");
    Serial.println(button_state_down);
   
  } else if(!button_state_up){
    menu--;
    update_start_menu();
    Serial.print("state_up: ");
    Serial.println(button_state_up);
  }
}

void IRAM_ATTR isr() {
  button_state_up = 1;
  button_state_down = 1;
}

void setup() {
  Serial.begin(115200);

  // initialize input
  pinMode(button_start, INPUT_PULLUP);
  pinMode(button_cancel, INPUT_PULLUP);
  pinMode(button_up, INPUT_PULLUP);
  pinMode(button_down, INPUT_PULLUP);

  attachInterrupt(button_up, isr, FALLING);
  attachInterrupt(button_down, isr, FALLING);
  
  // Setup LCD
  lcd.init();         
  lcd.init();
  lcd.backlight();

  // Welcome message
  welcome();
  update_start_menu();
}

void loop() {
  start_button_operation();

}
