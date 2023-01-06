
#define button_start 14
#define button_cancel 27
#define button_up 26
#define button_down 25

int button_state1 = 0;
int button_state2 = 0;
int button_state3 = 0;
int button_state4 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(button_start, INPUT);
  pinMode(button_cancel, INPUT);
  pinMode(button_up, INPUT);
  pinMode(button_down, INPUT);

}

void loop() {
  button_state1 = digitalRead(button_start);
  Serial.print("State1: ");
  Serial.println(button_state1);

  button_state2 = digitalRead(button_cancel);
  Serial.print("State2: ");
  Serial.println(button_state2);

  button_state3 = digitalRead(button_up);
  Serial.print("State3: ");
  Serial.println(button_state3);

  button_state4 = digitalRead(button_down);
  Serial.print("State4: ");
  Serial.println(button_state4);

}
