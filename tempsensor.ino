#define inPin A3
#define HISTORY_LENGTH 50
#define POLLING_INTERVAL 10  // ms
#define MAX_READING 5000
#define redPin 4
#define greenPin 5
#define bluePin 6

void setup() {
  Serial.begin(9600);
  pinMode(inPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void color(bool r, bool g, bool b) {
  digitalWrite(redPin, r);
  digitalWrite(greenPin, g);
  digitalWrite(bluePin, b);
}

void red() {color(HIGH, LOW, LOW);}
void green() {color(LOW, HIGH, LOW);}
void blue() {color(LOW, LOW, HIGH);}

void tabPrint(String left, float content) {
  Serial.print(left);
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
  float temp = 0.103*avg-45.79;

  float normN = 0;
  for (int i = 0; i < HISTORY_LENGTH; i++) {
    normN += (millivolts[i] - avg) * (millivolts[i] - avg);
  }
  float stdev = sqrt(normN / HISTORY_LENGTH);
  
  if (temp < 18) {
    blue();
  }
  if (18 < temp && temp < 20) {
    green();
  }
  if (temp > 20) {
    color(HIGH, LOW, HIGH);
  }

  tabPrint("min mV= ", minimum);
  tabPrint("max mV= ", maximum);
  tabPrint("avg mV= ", avg);
  tabPrint("stdev mV= ", stdev);
  tabPrint("temp *C= ", temp);
  Serial.println();
  
  delay(100);
}
