#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Nyanya"
#define WIFI_PASSWORD "terserahlu15"
#define API_KEY "AIzaSyCzZRoHPEese_ockCLPuficWzRgLpu-4Jk"
#define DATABASE_URL "https://temperatur-27863-default-rtdb.asia-southeast1.firebasedatabase.app/" 
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
int x=0,y=0,z=0;
int h1=0,h2=0,m1=0,m2=0,s1=0,s2=0,pos;
unsigned long start, current,count=0;
int hour, second, minute;
String print, print2;
#define ledu 13
#define ledd 12
#define ledr 14
#define ledl 15
#define lede 27
#define IN1 19
#define IN2 18
#define IN3 5
bool enter;
int timer,Wmin_on,Wsec_on,Wmin_off,Wsec_off,Smin_on,Ssec_on,Smin_off, Ssec_off,Wjam_tot,Wmin_tot,Wsec_tot;
int W_tot_delay,W_on_delay,W_off_delay,S_on_delay,S_off_delay,S_tot_delay;
int i,j,k,h;
bool end=true,delete_wash=false,delete_spin=false;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 
TaskHandle_t TaskHandle_1;
TaskHandle_t TaskHandle_2; 
TaskHandle_t TaskHandle_3;

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  lcd.init();                      
  lcd.backlight();
  lcd.cursor();
  Serial.begin(115200);
  pinMode(ledu, INPUT);
  pinMode(ledd, INPUT);
  pinMode(ledr, INPUT);
  pinMode(ledl, INPUT);
  pinMode(lede, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
}

void loop(){
 Initial();
 xTaskCreate(
    Wash,    // Function that should be called
    "Wash",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    &TaskHandle_1             // Task handle
  );
  xTaskCreate(
    Spin,    // Function that should be called
    "Spin",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    &TaskHandle_2             // Task handle
  );
  start=millis();
  lcd.noCursor();
  while(end){
    current=millis();
    current=current-start;
    current=current/1000;
    if(current>3599){
      hour=current/3600;
    }if(current>59){
      minute=(current-(hour*3600))/60;
    }
    second=(current-(hour*3600)-(minute*60));
    print="Wash:"+String(hour)+":"+String(minute)+":"+String(second);
    print2="Spin:"+String(count)+" Cycle"                       ;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(print);
    lcd.setCursor(0,1);
    lcd.print(print2);
    Serial.println(print);
    if(digitalRead(lede)){
      vTaskDelete(TaskHandle_1);
      Firebase.RTDB.setString(&fbdo, "Wash", print);
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, HIGH);
    }else if(digitalRead(ledd)){
      vTaskDelete(TaskHandle_2);
      Firebase.RTDB.setString(&fbdo, "Spin", print2);
      digitalWrite(IN3, HIGH);
    }
    delay(300);
  }  
}

void Wash(void * parameter){
  for(;;){ 
    digitalWrite(IN1, LOW);
    vTaskDelay((W_on_delay*1000) / portTICK_PERIOD_MS);
    digitalWrite(IN1, HIGH);
    vTaskDelay((W_off_delay*1000) / portTICK_PERIOD_MS);
    digitalWrite(IN2, LOW);
    vTaskDelay((W_on_delay*1000) / portTICK_PERIOD_MS);
    digitalWrite(IN2, HIGH);
    vTaskDelay((W_off_delay*1000) / portTICK_PERIOD_MS);
  }
}

void Spin(void * parameter){
  for(;;){
    digitalWrite(IN3, LOW);
    vTaskDelay((S_on_delay*1000) / portTICK_PERIOD_MS);
    digitalWrite(IN3, HIGH);
    vTaskDelay((S_off_delay*1000) / portTICK_PERIOD_MS);
    count++;
  }
}

void Initial(){
  bool done = true;
  bool enter = true;
  bool skip = true;
  int choose;
  while(done){
    lcd.clear();
    lcd.noCursor();
    while(enter){
      lcd.setCursor(0, 0);
      lcd.print("Wash: Tekan <");
      delay(50);
      lcd.setCursor(0,1);
      lcd.print("Spin: Tekan >");
      delay(50);
      if(digitalRead(ledl)){
        choose=1;
        enter=false;
      }else if(digitalRead(ledr)){
        choose=2;
        enter=false;
      }delay(100);
    }
    lcd.clear();
    switch(choose){
      case 1:
      
  DISP_TIMER(1);
  timer=COUNT_TIME();
  W_on_delay=timer;
  Wmin_on=timer/60;
  Wsec_on=timer-(Wmin_on*60);
  Serial.println( W_on_delay);
  DISP_COUNT(Wmin_on, Wsec_on,1);

  DISP_TIMER(2);
  timer=COUNT_TIME();
  W_off_delay=timer;
  Wmin_off=timer/60;
  Wsec_off=timer-(Wmin_off*60);
  Serial.println( W_off_delay);
  DISP_COUNT(Wmin_off, Wsec_off,2);

  if(skip){
  lcd.clear();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print("Setting Spin?");
  delay(50);
  lcd.setCursor(0,1);
  lcd.print("Ya: <  Tidak: >");
  delay(50);
 
        enter=true;
        while(enter){
          if(digitalRead(ledr)){
            done=false;
            enter=false;
          }else if(digitalRead(ledl)){
            choose=2;
            enter=false;
          }delay(50);
        } lcd.clear();skip=false;    
  }
  else{
          done=false;
        }
      break;
      case 2:
  DISP_TIMER(3);
  timer=COUNT_TIME();
  S_on_delay=timer;
  Smin_on=timer/60;
  Ssec_on=timer-(Smin_on*60);
  Serial.println( S_on_delay);
  DISP_COUNT(Smin_on, Ssec_on,3);

  DISP_TIMER(4);
  timer=COUNT_TIME();
  S_off_delay=timer;
  Smin_off=timer/60;
  Ssec_off=timer-(Smin_off*60);
  Serial.println( S_off_delay);
  DISP_COUNT(Smin_off, Ssec_off,4);

  if(skip){
  lcd.clear();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print("Setting Wash?");
  delay(50);
  lcd.setCursor(0,1);
  lcd.print("Ya: <  Tidak: >");
  delay(50);
        enter=true;
        while(enter){
          if(digitalRead(ledr)){
            done=false;
            enter=false;
          }else if(digitalRead(ledl)){
            choose=1;
            enter=false;
          }delay(50);
        }lcd.clear(); skip=false; 
        }
        else{
          done=false;
        }
      break;
    }
  } 
}
void DISP_COUNT(int a, int b, int c){
  lcd.setCursor(0, 0);
  switch(c){
    case 1:
    lcd.print("Timer Wash ON");break;
    case 2:
    lcd.print("Timer Wash OFF");break;
    case 3:
    lcd.print("Timer Spin ON");break;
    case 4:
    lcd.print("Timer Spin OFF");break;    
  }
  delay(250);
  lcd.setCursor(0, 1);
  lcd.print(a);lcd.print(" Menit ");lcd.print(b);lcd.print(" Sekon");
  delay(3000);
  lcd.clear(); 
}

int COUNT_TIME(){
  enter=true;
  m1=0;m2=0;s1=0;s2=0;
  int total;
  pos=11;
  lcd.setCursor(pos,0);
  lcd.cursor();
  while(enter){
    if(digitalRead(ledr)){
      if(pos<15){
      pos+=1;
      lcd.setCursor(pos,0);
      lcd.cursor();
      delay(250);}
    }
    if(digitalRead(ledl)){
      if(pos>11){
      pos-=1;
      lcd.setCursor(pos,0);
      lcd.cursor();
      delay(250);}
    }
    if(digitalRead(ledu)){
      switch(pos){
        case 11:if(m1<9){
          m1++;
          lcd.print(m1);}
          break;
        case 12:if(m2<9){
          m2++;
          lcd.print(m2);}
          break;
        case 14:if(s1<9){
          s1++;
          lcd.print(s1);}
          break;
        case 15:if(s2<9){
          s2++;
          lcd.print(s2);}
          break;
        default:
          break;
      }
      lcd.setCursor(pos,0);
      lcd.cursor();
      delay(250);
    }
    if(digitalRead(ledd)){
      switch(pos){
        case 11:if(m1>0){
        m1--;
        lcd.print(m1);}
        break;
        case 12:if(m2>0){
        m2--;
        lcd.print(m2);}
        break;
        case 14:if(s1>0){
        s1--;
        lcd.print(s1);}
        break;
        case 15:if(s2>0){
        s2--;
        lcd.print(s2);}
        break;
      }
      lcd.setCursor(pos,0);
      lcd.cursor();
      delay(250);
    }
    if(digitalRead(lede)){
      enter=false;
      delay(250);
    }
  }
  total=s2+(s1*10)+(m2*60)+(m1*10*60);
  lcd.clear();
  return total;
}

void DISP_TIMER(int a){
  lcd.setCursor(0, 0);
  lcd.print("Timer");
  delay(50);
  lcd.setCursor(11,0);
  lcd.print("00:00");
  delay(50);
  lcd.setCursor(0, 1);
  switch(a){
    case 1:
      lcd.print("Wash ON");break;
    case 2:
      lcd.print("Wash OFF");break;
    case 3:
      lcd.print("Spin ON");break;
    case 4:
      lcd.print("Spin OFF");break;
  }  
  delay(50);
  lcd.setCursor(11,1);
  lcd.print("mm:ss"); 
}
