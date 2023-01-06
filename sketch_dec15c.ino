#define IN1 19
#define IN2 18
#define IN3 5


void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  xTaskCreate(
    Wash,    // Function that should be called
    "Wash",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );
  xTaskCreate(
    Spin,    // Function that should be called
    "Spin",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );
}

void loop() {
 
}

void Wash(void * parameter){
  for(;;){ // infinite loop
    digitalWrite(IN1, HIGH);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    digitalWrite(IN1, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(IN2, HIGH);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    digitalWrite(IN2, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Spin(void * parameter){
  for(;;){ // infinite loop
    digitalWrite(IN3, HIGH);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    digitalWrite(IN3, LOW);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}
