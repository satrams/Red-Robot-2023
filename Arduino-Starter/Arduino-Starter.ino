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

bool line_follow = false;
const float forward = 0.5;

void loop() {
  // Read the four joystick axes
  // These will be in the range [-1.0, 1.0]
  float rightX = RR_axisRX();
  float rightY = RR_axisRY();
  float leftX  = RR_axisLX();
  float leftY  = RR_axisLY();

  // Arcade-drive scheme
  // Left Y-axis = throttle
  // Right X-axis = steering
  RR_setMotor1(leftY + rightX);
  RR_setMotor2(leftY - rightX);

  // Get the button states
  bool btnA = RR_buttonA();
  bool btnB = RR_buttonB();
  bool btnX = RR_buttonX();
  bool btnY = RR_buttonY();
  bool btnRB = RR_buttonRB();
  bool btnLB = RR_buttonLB();

  if (btnA) {
    PIDReset();
    line_follow = true;
  }
  else if (btnB) {
    PIDReset();
    line_follow = false;
  }

  if (line_follow) {
    float turnVal = PIDControl(get_current_value);
    RR_setMotor1(forward + turnVal);
    RR_setMotor2(forward - turnVal);
  }


  // // Control motor3 port (unused on base robot) using A/B buttons
  // if (btnA) {
  //   RR_setMotor3(1.0);
  // }
  // else if (btnB) {
  //   RR_setMotor3(-1.0);
  // }
  // else {
  //   RR_setMotor3(0.0);
  // }

  // // Control motor4 port (unused on base robot) using X/Y buttons
  // if (btnX) {
  //   RR_setMotor4(1.0);
  // }
  // else if (btnY) {
  //   RR_setMotor4(-1.0);
  // }
  // else {
  //   RR_setMotor4(0.0);
  // }

  // // Control servo 1 using the dpad
  // // 6 = left, 2 = right, 0 = up, 4 = down, 8 = center
  // // (note that you will also see the value 0 if the controller
  // //  is disconnected)
  // if (RR_dpad() == 6) { // left

  //   // we can't move a servo less than 0 degrees
  //   if (temp > 0) temp -= 10;
  // }
  // else if (RR_dpad() == 2) { // right

  //   // we can't move a servo past 180 degrees
  //   // for continuous rotation, try using a DC motor
  //   if (temp < 180) temp += 10;
  // }
  // RR_setServo1(temp);

  // // Control servo 2 using the shoulder buttons
  // // This example moves the servo to fixed points
  // // You can change the angles based on your mechanism
  // // (this is great for a mechanism that only has 2 states,
  // //  such as a grabber or hook)
  // if (btnRB) {
  //   RR_setServo2(180);
  // }
  // else if (btnLB) {
  //   RR_setServo2(0);
  // }

  // we also have RR_setServo3 and RR_setServo4 available


  // read the ultrasonic sensors

  // Serial.print("Ultrasonic=");
  // Serial.print(RR_getUltrasonic());
  // Serial.print(" ;; ");
  
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
}

float sensor_maxes[6] = {
  6214,
  6061,
  4427,
  3531,
  4610,
  7025
}

float map_normalize(float val, float min, float max) {
  return (val - min) / (max - min);
}

float get_current_value() {
  int sensors[6];

  RR_getLineSensors(sensors);

  int norms[6];

  for (int i = 0; i < 6; i++) {
    norms[i] = constrain(map_normalize(sensors[i], sensor_mins[i], sensor_maxes[i], 0, 1);
  }

  int output = 0;

  for (int i = 0; i < 6; i++) {
    output += (i-2) * norms[i];
  }

  return output; // Should be .5 if it's even
}

const int T = 2; // Fixed Timestep
const float kp = 1;
const float kd = .1;
unsigned long last_time = 0;
float last_error = 0;
const float setpoint = 0.5;

void PIDReset() {
  last_time = 0;
  last_error = 0;
}

float PIDControl(float currentValue) {
  
  unsigned long current_time = millis();

  float delta = current_time - last_time;

  float error = setpoint - currentValue;
  float delta_error = error - last_error;
  
  float output_value = kp*error + (kd/delta)*(delta_error);

  last_error = error;
  last_time = current_time;

  return output_value;
}


// vim: tabstop=2 shiftwidth=2 expandtab
