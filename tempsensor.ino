#include <QueueList.h>

#define inPin A3
#define POLLING_INTERVAL 100  // ms
#define redPin 4
#define greenPin 5
#define bluePin 6
#define plotterMode true
#define MEASURE_HIST_LENGTH 50

void setup() {
  Serial.begin(9600);
  pinMode(inPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void tabPrint(String left, float content) {
  // ignore funky data from first second
  if (millis() < 1000) {
    return;
  }
  if (!plotterMode) {
    Serial.print(left);
  }
  Serial.print(content);
  Serial.print("\t");
}

// https://www.kalmanfilter.net/kalman1d.html
// current variables are initial estimates
float x;
float v = 0;
// initial x estimate, can be imprecise
float x_pred = 750;
float v_pred = 0;
// variance of initial estimate
float estimate_cov = 25000;
float last_time = 0;
float beta = 0;
float addKalmanMeasurement(float this_time, float measurement) {
  float dt = this_time - last_time;
  // avoid dt=0 on first update; no dt should be < POLLING_INTERVAL/10 in normal operation
  if (dt < POLLING_INTERVAL / 10) {
      dt = POLLING_INTERVAL;
  }
  last_time = this_time;
  // CALIB
  // assume same variance for each measurement
  float measurement_cov = 197;
  // 3. Kalman Gain
  float kalman_gain = estimate_cov / (estimate_cov + measurement_cov);
  // 1. State Update
  x = (1-kalman_gain) * x_pred + kalman_gain * measurement;
  v = (1-beta) * v_pred + beta * (measurement - x_pred) / dt;
  // 2. State Extrapolation
  x_pred = x + dt * v;
  v_pred = 0;
  // 4. Covariance Update
  float cov = (1-kalman_gain) * estimate_cov;
  // 5. Covariance Extrapolation
  // process noise
  float q = 0.1;
  estimate_cov = cov + q;

  return x;
}

float naive_total = 0;
QueueList<float> measure_hist;
void loop() {
  float t = millis();
  float raw = analogRead(inPin);
  float raw_mv = raw*5000/1024;

  tabPrint("raw dV= ", raw_mv/100);

  float kalman_mv = addKalmanMeasurement(t, raw_mv);
  naive_total += raw_mv;
  measure_hist.push(raw_mv);
  if (measure_hist.count() > MEASURE_HIST_LENGTH) {
    naive_total -= measure_hist.peek();
    measure_hist.pop();
  }
  tabPrint("naive dV= ", naive_total / measure_hist.count() / 100);

  float temp = 0.1029*kalman_mv-45.18;

  doColor(temp);

  tabPrint("avg dV= ", kalman_mv/100);
  tabPrint("temp *C= ", temp);
  // tabPrint("dV pred= ", x_pred/100);
  tabPrint("var (mV^2)= ", estimate_cov);
  Serial.println();

  delay(POLLING_INTERVAL);
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

void rgb(bool r, bool g, bool b) {
  red(r);
  green(g);
  blue(b);
}

bool isIn(float x, float a, float b) {
  return a < x && x < b;
}

struct Pair {
  float x;
  float y;

  Pair(float x, float y) {
    this->x = x;
    this->y = y;
  }

  friend float operator^(float a, Pair b) {
    return isIn(a, b.x, b.y);
  }
};

#define in ^ Pair

void doColor(float temp) {
  red(temp in (5,20));
  green(temp in (25, 40));
  blue(temp in (45, 90));
}
