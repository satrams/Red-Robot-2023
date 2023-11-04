// Replace 12345 with the correct team number and then uncomment the line below.
#define TEAM_NUMBER 9

#ifndef TEAM_NUMBER
#error "Define your team number with `#define TEAM_NUMBER 12345` at the top of the file."
#elif TEAM_NUMBER < 1 || 20 < TEAM_NUMBER
#error "Team number must be within 1 and 20"
#endif


void setup() {
  Serial.begin(115200);
}

int temp = 0;

bool autonEnabled = false;
bool lineFollow = false;
const float forward = 0.35;
const unsigned long autonTime = 20000; // 20 SEconds for autonomous
unsigned long startTime = 0;
const unsigned long lineFollowTime = 9200;
const float wallDistance = 50; // 50 cm

void cancel_auton() {
    PID_reset();
    lineFollow = false;
    RR_setMotor1(0); // This is just for safety
  RR_setMotor2(0);
}

void start_auton() {
  PID_reset();
  lineFollow = true;
  startTime = millis();
}

void loop() {
  float motor1 = 0;
  float motor2 = 0;

  // Read the four joystick axes
  // These will be in the range [-1.0, 1.0]
  float rightX = RR_axisRX();
  float rightY = RR_axisRY();
  float leftX  = RR_axisLX();
  float leftY  = RR_axisLY();

  // Arcade-drive scheme
  // Left Y-axis = throttle
  // Right X-axis = steering
  // RR_setMotor1(leftY + rightX);
  // RR_setMotor2(leftY - rightX);

  // Get the button states
  bool btnA = RR_buttonA();
  bool btnB = RR_buttonB();
  bool btnX = RR_buttonX();
  bool btnY = RR_buttonY();
  bool btnRB = RR_buttonRB();
  bool btnLB = RR_buttonLB();

  if (btnA) {
    start_auton();
  }
  else if (btnB) {
    cancel_auton();
  }

  if (autonEnabled) {
    if (millis() - startTime < autonTime) {
      cancel_auton();
    }
    
    if (get_average_ultrasonic() < wallDistance) {
      lineFollow = false; // Change mode to set servo position
    }
    
    if (lineFollow) {
        float turnVal = PID_control(get_current_value());
        // Serial.println(turnVal);
        Serial.println();
        motor1 = forward - turnVal;
        motor2 = forward + turnVal;
    }
    else {
      //Set the position of the servo
    }
  }
  else {
    // Teleop Controls
    motor1 = leftY + rightX;
    motor2 = leftY - rightX;
  }

  // int sensors[6];

  // Serial.print("Line sensors=");
  // RR_getLineSensors(sensors);
  // for (int i = 0; i < 6; ++i) {
  //   Serial.print(sensors[i]);
  //   Serial.print(" ");
  // }
  // Serial.print(btnA ? 1 : 0);
  // Serial.print(btnB ? 1 : 0);
  // Serial.print(btnX ? 1 : 0);
  // Serial.print(btnY ? 1 : 0);
  // Serial.println();

  RR_setMotor1(motor1);
  RR_setMotor2(motor2);

  // This is important - it sleeps for 0.02 seconds (= 50 times / second)
  // Running the code too fast will overwhelm the microcontroller and peripherals

  delay(20);
}

// PID Control
// General loop:
// Fixed timestep T
// Current time, last time, delta_time (current time - last time)
// Current error, last error, delta_error (error - last error)
// Pvalue = Kp * error
// Dvalue = Kd * delta_error/T
// output = Pvalue + Dvalue
// last error = current error
// last time = current time
// return output

float sensor_mins[6] = {
  1425,
  1321,
  993,
  770,
  1007,
  1453
};

float sensor_maxes[6] = {
  6214,
  6061,
  4427,
  3531,
  4610,
  7025
};

float map_normalize(float val, float min, float max) {
  return (val - min) / (max - min);
}

float get_current_value() {
  int sensors[6];

  RR_getLineSensors(sensors);

  float norms[6];

  float weights[6] = {
    -2.5,
    -1.5,
    -0.5,
    0.5,
    1.5,
    2.5
  };

  for (int i = 0; i < 6; i++) {
    norms[i] = constrain(map_normalize(sensors[i], sensor_mins[i], sensor_maxes[i]), 0, 1);
  }

  float output = 0;
  
  for (int i = 0; i < 6; i++) {
    output += weights[i] * norms[i];
  }
  // output = output/6;
  // Serial.print("Norms: ");
  // for (int i = 0; i < 6; i++) {
  //   Serial.print(norms[i]);
  //   Serial.print(" ");
  // }
  // Serial.println();
  // Serial.print("Output value: ");
  // Serial.println(output);

  return output; // Should be .5 if it's even
}

const int T = 2; // Fixed Timestep
const float kp = 0.125;
const float kd = -.01;
unsigned long last_time = 0;
float last_error = 0;
const float setpoint = 0;

void PID_reset() {
  last_time = 0;
  last_error = 0;
}

float PID_control(float currentValue) {
  
  unsigned long current_time = millis();

  float delta = current_time - last_time;

  float error = setpoint - currentValue;
  float delta_error = error - last_error;
  
  float output_value = kp*error + (kd/delta)*(delta_error);

  //Serial.println(output_value);

  last_error = error;
  last_time = current_time;

  return output_value;
}

float val1 = 0;
float val2 = 0;
float val3 = 0;

float get_average_ultrasonic() {
  float current = RR_getUltrasonic();
  float output = (current + val1 + val2 + val3)/4;

  val1 = val2;
  val2 = val3;
  val3 = current;

  return output;
}

// vim: tabstop=2 shiftwidth=2 expandtab
