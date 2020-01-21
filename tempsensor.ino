#define inPin A3
#define HISTORY_LENGTH 50
#define POLLING_INTERVAL 10  // ms
#define MAX_READING 5000
#define redPin 4
#define greenPin 5
#define bluePin 6
#define plotterMode true

void setup() {
  Serial.begin(9600);
  pinMode(inPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void tabPrint(String left, float content) {
  Serial.print(content);
  Serial.print("\t");
}

void loop() {
  float total = 0;
  float minimum = MAX_READING;
  float maximum = 0;
  float millivolts[HISTORY_LENGTH];
  float raw, mv;
  for (int i = 0; i < HISTORY_LENGTH; i++) {
    raw = analogRead(inPin);
    mv = raw*5000/1024;
    millivolts[i] = mv;
    total += mv;
    minimum = min(minimum, mv);
    maximum = max(maximum, mv);
    delay(POLLING_INTERVAL);
  }

  float avg = total / HISTORY_LENGTH;
  float temp = 0.100192*avg-45.1788;
  
  doColor(temp);

  float normN = 0;
  for (int i = 0; i < HISTORY_LENGTH; i++) {
    normN += (millivolts[i] - avg) * (millivolts[i] - avg);
  }

  // high by 1 at low temps, low by 1 at high temps
  float stdev = sqrt(normN / HISTORY_LENGTH);
  
  doColor(temp);

  if (plotterMode) {
    Serial.print(avg);
    Serial.print("\t");
    Serial.print(temp);
    Serial.print("\t");
    Serial.println();
  } else {
    tabPrint("min mV= ", minimum);
    tabPrint("max mV= ", maximum);
    tabPrint("stdev mV= ", stdev);
    tabPrint("avg mV= ", avg);
    tabPrint("temp *C= ", temp);
  }
  Serial.println();
  
  delay(100);
}

void red(bool status) {
  digitalWrite(redPin, status);
}

void green(bool status) {
  digitalWrite(greenPin, status);
}

void blue(bool status) {
  digitalWrite(bluePin, status);
}

bool in(float x, float a, float b) {
  return a < x && x < b;
}

void doColor(float temp) {
  green(in(temp, 5, 35));
  red(in(temp, 25, 40));
  blue(in(temp, 50, 60));
}
