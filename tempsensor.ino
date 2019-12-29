#define inPin A3
#define HISTORY_LENGTH 50
#define POLLING_DELAY 20  // ms
#define MAX_READING 1024

void setup() {
  Serial.begin(9600);
}

void loop() {
  float total = 0;
  int minimum = MAX_READING;
  int maximum = 0;
  for (int i = 0; i < HISTORY_LENGTH; i++) {
    int raw = analogRead(inPin);
    total += raw;
    minimum = min(minimum, raw);
    maximum = max(maximum, raw);
    delay(POLLING_DELAY);
  }

  float raw = total / HISTORY_LENGTH;
  float mv = raw*5000/1024;
  float temp = 0.103*mv-45.79;

  Serial.print(minimum);
  Serial.print("\t");
  Serial.print(maximum);
  Serial.print("\t");
  Serial.print(raw);
  Serial.print("\t");
  Serial.print(mv);
  Serial.print("\t");
  Serial.print(temp);
  Serial.println();
  
  delay(400);
}
