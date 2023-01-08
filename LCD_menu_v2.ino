#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27,16,2);

// set button pin
#define button_start 14
#define button_cancel 27
#define button_up 26
#define button_down 25

// set relay pin
#define relay_1 12
#define relay_2 33
#define relay_3 32

// state relay
bool state_relay_1 = LOW;
bool state_relay_2 = LOW;
bool state_relay_3 = LOW;

// variable timer
int timerWashOn = 0; // second
int timerWashOff = 0; // second
int timerSpinOn = 0; // minute
int timerSpinOff = 0; // minute

// setup timer for relay
unsigned long rememberTime_1 = 0;
const long onDuration_relay_1 = timerWashOn * 1000;
const long offDuration_relay_1 = timerWashOff * 1000;

unsigned long rememberTime_2 = 0;
const long onDuration_relay_2 = timerWashOn * 1000;
const long offDuration_relay_2 = timerWashOff * 1000;

unsigned long rememberTime_3 = 0;
const long onDuration_relay_3 = timerSpinOn * 60 * 1000;
const long offDuration_relay_3 = timerSpinOff * 60 * 1000;

// variable accumulation of timer and cycles
int accumulationWashTimer = 0;
int accumulationSpinCycles = 0;

// variable on off
String indikatorOnOff = "OFF";

// bytes to access in EEPROM, int -> 4 bytes
#define EEPROM_SIZE 4

/*
  Address EEPROM:
  1. timerWashOn -> address 0
  2. timerWashOff => address 1
  3. timerSpinOn -> address 2
  4. timerSpinOff -> address 3
  5. accumulationWashTimer -> address 4
  6. accumulationSpinCycles -> address 5
*/

void welcome(){
  lcd.setCursor(5,0);
  lcd.print("WELCOME");
  lcd.setCursor(0,1);
  lcd.print("[S] [C] [UP] [D]");
}

//void washing(){
//  if(state_relay_1 == LOW){
//    
//  }
//}

void menu(){
// ======================= MENU UTAMA ===========================
start:
  if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    goto setting_1;
  } else if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    goto EXIT;
  } else if(digitalRead(button_start) == HIGH && indikatorOnOff == "OFF"){
    while(digitalRead(button_start) == HIGH){}
    indikatorOnOff = "ON ";
  } else if(digitalRead(button_start) == HIGH && indikatorOnOff == "ON "){
    while(digitalRead(button_start) == HIGH){}
    indikatorOnOff = "OFF";
  }

  lcd.setCursor(0,0);
  lcd.print(">START       " + indikatorOnOff);
  lcd.setCursor(0,1);
  lcd.print(" SETTING");
  
  goto start;

// ---------------------------------------------------
setting_1:
  if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    lcd.clear();
    goto setting_2;
  } else if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    lcd.clear();
    goto start;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    lcd.clear();
    goto timer_w;
  } else if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    goto EXIT;
  }

  lcd.setCursor(0,0);
  lcd.print(" START       " + indikatorOnOff);
  lcd.setCursor(0,1);
  lcd.print(">SETTING");

  goto setting_1;
// ---------------------------------------------------
setting_2:
  if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    lcd.clear();
    goto status_1;
  } else if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    lcd.clear();
    goto setting_1;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    lcd.clear();
    goto timer_w;
  } else if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    goto EXIT;
  }

  lcd.setCursor(0,0);
  lcd.print(">SETTING");
  lcd.setCursor(0,1);
  lcd.print(" STATUS");

  goto setting_2;
// ---------------------------------------------------
status_1:
 if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    lcd.clear();
    goto status_2;
  } else if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    lcd.clear();
    goto setting_2;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    lcd.clear();
    goto status;
  } else if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    goto EXIT;
  }

  lcd.setCursor(0,0);
  lcd.print(" SETTING");
  lcd.setCursor(0,1);
  lcd.print(">STATUS");
  goto status_1;
// ---------------------------------------------------
status_2:
  if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    lcd.clear();
    goto target;
  } else if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    lcd.clear();
    goto status_1;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    lcd.clear();
    goto status;
  } else if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    goto EXIT;
  }

  lcd.setCursor(0,0);
  lcd.print(">STATUS");
  lcd.setCursor(0,1);
  lcd.print(" TARGET");

  goto status_2;
// ---------------------------------------------------
target:
  if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    lcd.clear();
    goto status_2;
  } else if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    goto EXIT;
  }

  lcd.setCursor(0,0);
  lcd.print(" STATUS");
  lcd.setCursor(0,1);
  lcd.print(">TARGET");
  goto target;

  // ===================== SUB MENU SETTING ===========================
timer_w:
  if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    lcd.clear();
    goto timer_s;
  } else if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    lcd.clear();
    goto start;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    lcd.clear();
    goto set_timer_w_on;
  }

  lcd.setCursor(0,0);
  lcd.print(">TIMER WASH");
  lcd.setCursor(0,1);
  lcd.print(" TIMER SPIN");

  goto timer_w;

timer_s:
  if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    lcd.clear();
    goto timer_w;
  } else if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    lcd.clear();
    goto start;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    lcd.clear();
    goto set_timer_s_on;
  }

  lcd.setCursor(0,0);
  lcd.print(" TIMER WASH");
  lcd.setCursor(0,1);
  lcd.print(">TIMER SPIN");

  goto timer_s;

// ===================== SUB MENU TIMER WASH ===========================
set_timer_w_on:
  if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    lcd.clear();
    goto set_timer_w_off;
  } else if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    lcd.clear();
    goto timer_w;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    lcd.clear();
    goto W_ON_TIMER;
  }

  lcd.setCursor(0,0);
  lcd.print(">TIMER ON : " + String(timerWashOn) + "dtk");
  lcd.setCursor(0,1);
  lcd.print(" TIMER OFF: " + String(timerWashOff) + "dtk");

  goto set_timer_w_on;

set_timer_w_off:
  if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    lcd.clear();
    goto timer_w;
  } else if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    lcd.clear();
    goto set_timer_w_on;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    lcd.clear();
    goto W_OFF_TIMER;
  }

  lcd.setCursor(0,0);
  lcd.print(" TIMER ON : " + String(timerWashOn) + "dtk");
  lcd.setCursor(0,1);
  lcd.print(">TIMER OFF: " + String(timerWashOff) + "dtk");
  goto set_timer_w_off;

W_ON_TIMER:
  if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    timerWashOn--;
  } else if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    timerWashOn++;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    Serial.print("Timer Wash On: ");
    Serial.println(timerWashOn);
    EEPROM.write(0, timerWashOn); // 0 adalah address eeprom
    EEPROM.commit();
    lcd.clear();
    goto set_timer_w_on;
  }
  
  lcd.setCursor(0,0);
  lcd.print(">TIMER ON : " + String(timerWashOn) + "dtk");
  lcd.setCursor(0,1);
  lcd.print(" TIMER OFF: " + String(timerWashOff) + "dtk");
  goto W_ON_TIMER;

W_OFF_TIMER:
  if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    timerWashOff--;
  } else if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    timerWashOff++;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    Serial.print("Timer Wash Off: ");
    Serial.println(timerWashOff);
    EEPROM.write(1, timerWashOff); // 1 adalah address eeprom
    EEPROM.commit();
    lcd.clear();
    goto set_timer_w_off;
  }

  lcd.setCursor(0,0);
  lcd.print(" TIMER ON : " + String(timerWashOn) + "dtk");
  lcd.setCursor(0,1);
  lcd.print(">TIMER OFF: " + String(timerWashOff) + "dtk");
  
  goto W_OFF_TIMER;

// ===================== SUB MENU TIMER SPIN =======================
set_timer_s_on:
  if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    lcd.clear();
    goto set_timer_s_off;
  } else if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    lcd.clear();
    goto timer_s;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    lcd.clear();
    goto S_ON_TIMER;
  }

  lcd.setCursor(0,0);
  lcd.print(">TIMER ON : " + String(timerSpinOn) + "mnt");
  lcd.setCursor(0,1);
  lcd.print(" TIMER OFF: " + String(timerSpinOff) + "mnt");

  goto set_timer_s_on;

set_timer_s_off:
  if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    lcd.clear();
    goto set_timer_s_on;
  } else if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    lcd.clear();
    goto timer_s;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    lcd.clear();
    goto S_OFF_TIMER;
  }

  lcd.setCursor(0,0);
  lcd.print(" TIMER ON : " + String(timerSpinOn) + "mnt");
  lcd.setCursor(0,1);
  lcd.print(">TIMER OFF: " + String(timerSpinOff) + "mnt");

  goto set_timer_s_off;

S_ON_TIMER:
  if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    timerSpinOn--;
  } else if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    timerSpinOn++;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    Serial.print("Timer Spin On: ");
    Serial.println(timerSpinOn);
    EEPROM.write(2, timerSpinOn); // 2 adalah address eeprom
    EEPROM.commit();
    lcd.clear();
    goto set_timer_s_on;
  }
  
  lcd.setCursor(0,0);
  lcd.print(">TIMER ON : " + String(timerSpinOn) + "mnt");
  lcd.setCursor(0,1);
  lcd.print(" TIMER OFF: " + String(timerSpinOff) + "mnt");
  goto S_ON_TIMER;

S_OFF_TIMER:
  if(digitalRead(button_down) == HIGH){
    while(digitalRead(button_down) == HIGH){}
    timerSpinOff--;
  } else if(digitalRead(button_up) == HIGH){
    while(digitalRead(button_up) == HIGH){}
    timerSpinOff++;
  } else if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    Serial.print("Timer Spin Off: ");
    Serial.println(timerSpinOff);
    EEPROM.write(3, timerSpinOff); // 3 adalah address eeprom
    EEPROM.commit();
    lcd.clear();
    goto set_timer_s_off;
  }
  
  lcd.setCursor(0,0);
  lcd.print(" TIMER ON : " + String(timerSpinOn) + "mnt");
  lcd.setCursor(0,1);
  lcd.print(">TIMER OFF: " + String(timerSpinOff) + "mnt");
  goto S_OFF_TIMER;

// ===================== SUB MENU STATUS ===========================
status:
  if(digitalRead(button_cancel) == HIGH){
    while(digitalRead(button_cancel) == HIGH){}
    lcd.clear();
    goto start;
  }

  lcd.setCursor(0,0);
  lcd.print("WASH:");
  lcd.setCursor(0,1);
  lcd.print("SPIN:");

  goto status;

 // ================= EXIT KE TAMPILAN AWAL =========================
 EXIT:
    lcd.clear();
    delay(100);

}

void setup() {
  Serial.begin(115200);

  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);

  // read all of variable that save in EEPROM
  timerWashOn = EEPROM.read(0);
  timerWashOff = EEPROM.read(1);
  timerSpinOn = EEPROM.read(2);
  timerSpinOff = EEPROM.read(3);
  accumulationWashTimer = EEPROM.read(4);;
  accumulationSpinCycles = EEPROM.read(5);;
  
  // initialize input
  pinMode(button_start, INPUT);
  pinMode(button_cancel, INPUT);
  pinMode(button_up, INPUT);
  pinMode(button_down, INPUT);
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
  pinMode(relay_3, OUTPUT);

  // confirm the relays turn off
  digitalWrite(relay_1, LOW);
  digitalWrite(relay_2, LOW);
  digitalWrite(relay_3, LOW);

  // Setup LCD
  lcd.init();         
  lcd.init();
  lcd.backlight();
}

void loop() {
  if(digitalRead(button_start) == HIGH){
    while(digitalRead(button_start) == HIGH){}
    lcd.clear();
    menu();
  }
  // Welcome message
  welcome();
}
