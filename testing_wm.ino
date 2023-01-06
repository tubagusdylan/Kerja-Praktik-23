#include <LiquidCrystal_I2C.h>
int lcdColumns = 16;
int lcdRows = 2;
bool end=true;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 
TaskHandle_t TaskHandle_1;
TaskHandle_t TaskHandle_2; 
unsigned long start, current,count=0;
int hour, second, minute,h,i,j,k;
#define IN1 19
#define IN2 18
#define IN3 5
String print, print2;

void setup() {
  Serial.begin(115200);
  lcd.init();                      
  lcd.backlight();
  lcd.cursor();
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
}

void loop() {
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
    delay(300);
  }
}
void Wash(void * parameter){
  for(;;){ 
    digitalWrite(IN1, LOW);
    vTaskDelay((5000) / portTICK_PERIOD_MS);
    digitalWrite(IN1, HIGH);
    vTaskDelay((1000) / portTICK_PERIOD_MS);
    digitalWrite(IN2, LOW);
    vTaskDelay((5000) / portTICK_PERIOD_MS);
    digitalWrite(IN2, HIGH);
    vTaskDelay((1000) / portTICK_PERIOD_MS);
  }
}

void Spin(void * parameter){
  for(;;){
    digitalWrite(IN3, LOW);
    vTaskDelay((8000) / portTICK_PERIOD_MS);
    digitalWrite(IN3, HIGH);
    vTaskDelay((2000) / portTICK_PERIOD_MS);
    count++;
  }
}
