#define inPin A3
#define POLLING_INTERVAL 100  // ms
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
  v = v_pred + kalman_gain * (measurement - x_pred) / dt;
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


void loop() {
  float t = millis();
  float raw = analogRead(inPin);
  float raw_mv = raw*5000/1024;

  tabPrint("time= ", t);
  tabPrint("raw mV= ", raw_mv);

  float kalman_mv = addKalmanMeasurement(t, raw_mv);

  float temp = 0.100192*kalman_mv-45.1788;

  doColor(temp);

  tabPrint("avg mV= ", kalman_mv);
  tabPrint("temp *C= ", temp);
  tabPrint("mV pred= ", x_pred);
  tabPrint("var= ", estimate_cov);
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

bool in(float x, float a, float b) {
  return a < x && x < b;
}

void doColor(float temp) {
  green(in(temp, 5, 35));
  red(in(temp, 25, 40));
  blue(in(temp, 50, 60));
}
