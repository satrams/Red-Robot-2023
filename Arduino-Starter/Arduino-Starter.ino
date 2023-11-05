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

bool autonEnabled = false;
bool lineFollow = false;
const float forward = 0.34;
const unsigned long autonTime = 20000; // 20 SEconds for autonomous
unsigned long startTime = 0;
const unsigned long lineFollowTime = 19000; // optional timing for how long it takes to get to the end of the line
const float wallDistance = 20; // 50 cm
const float turnSpeed = 0.75;
const float topServoVal = 60;
const float bottomServoVal = 180;
const unsigned long failsafeTime = 11000; // A time to go to the next phase if still in lineFollow
bool deploy = false;
const unsigned long deployTime = 800;
bool undeploy = false;
const unsigned long undeployTime = 500;
bool moveBack = false;
const float moveBackSpeed = .7;
const unsigned long moveBackTime = 2000;

void cancel_auton() {
    PID_reset();
    lineFollow = false;
    moveBack = false;
    deploy = false;
    RR_setMotor1(0); // This is just for safety
    RR_setMotor2(0);
    Serial.println("ahhhhh");
    autonEnabled = false;
}

void start_auton() {
  PID_reset();
  lineFollow = true;
  deploy = false;
  moveBack = false;
  autonEnabled = true;
  startTime = millis();
  Serial.println("yipeee");
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

  // Serial.println("-----------");
  // Serial.println(btnA);
  // Serial.println(btnB);
  // Serial.println(btnX);
  // Serial.println(btnY);

  if (autonEnabled) {
    if (millis() - startTime >= autonTime) {
      cancel_auton();
    }
    
    //we might also want a timing thing here but it's not that important
    float ultrasonic = get_average_ultrasonic();
    if (millis() - startTime >= lineFollowTime) {
      lineFollow = false; // Change mode to set servo position
      deploy = true;
    }

    if (millis() - startTime >= lineFollowTime + deployTime) {
      lineFollow = false;
      deploy = false;
      undeploy = true;
    }

    if (millis() - startTime >= lineFollowTime + deployTime + undeployTime) {
      lineFollow = false;
      deploy = false;
      undeploy = false;
      moveBack = true;
    }

    if (millis() - startTime >= lineFollowTime + deployTime  + undeployTime + moveBackTime) {
      lineFollow = false;
      deploy = false;
      moveBack = false;
    }
    
    if (lineFollow) {
        float turnVal = PID_control(get_current_value());
        // Serial.println(turnVal);
        // Serial.println();
        motor1 = forward - turnVal;
        motor2 = forward + turnVal;
    }
    else if (deploy) {
      //Set the position of the servo
      RR_setServo3(topServoVal);
    }
    else if (undeploy) {
      RR_setServo3(bottomServoVal);
    }
    else if (moveBack) {
      motor1 = -moveBackSpeed;
      motor2 = -moveBackSpeed;
    }

    if (btnX) {
      Serial.println("stopped");
      cancel_auton();
    }
  }
  else {
    // Teleop Controls
    motor1 = sign(leftY) * square(leftY) +
     (sign(rightX) * square(rightX) * turnSpeed);
    motor2 = sign(leftY) * square(leftY) -
     (sign(rightX) * square(rightX) * turnSpeed);

    // read the ultrasonic sensors
    Serial.print("Ultrasonic=");
    Serial.println(RR_getUltrasonic());
    Serial.print(" ;; ");

    
    if (btnY) {
      Serial.println("started");
      start_auton();
    }

    if (btnA) {
      // Set servo down
      RR_setServo3(topServoVal);
    }
    else if (btnB) {
      // Set servo up      
      RR_setServo3(bottomServoVal);
    }
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

  //Serial.println(motor1);

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

  float sum = 0;

  for (int i = 0; i < 6; i++) {
    sum += norms[i];
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

  return output/sum; // Should be .5 if it's even
}

const int T = 2; // Fixed Timestep
const float kp = 0.27;
const float kd = 0.04;
unsigned long last_time = 0;
float last_error = 0;
const float setpoint = 0;

void PID_reset() {
  last_time = 0;
  last_error = 0;
}

// Just returns some control output
float PID_control(float currentValue) {
  
  unsigned long current_time = millis();

  float delta = current_time - last_time;

  float error = setpoint - currentValue; // positive when to the left, negative when to the right
  float delta_error = error - last_error; // negative decreasing, positive increasing
  
  float output_value = kp*error + (kd/delta)*(delta_error);

  //Serial.println(output_value);

  last_error = error;
  last_time = current_time;

  return output_value;
}

float val1 = -1;
float val2 = -1;
float val3 = -1;
float prevAvg = -1;
const float diffMax = 20;

// Returns the average value of the last couple frames

float get_average_ultrasonic() {
  float current = RR_getUltrasonic();

  int num = 1;
  if (abs(current - prevAvg) > diffMax) {
    return -1;
  }

  float output = current;

  // Only add the numbers to the average if they exist
  if (val3 != -1) {
    output += val3;
    num++;
  }
  if (val2 != -1) {
    output += val2;
    num++;
  }
  if (val1 != -1) {
    output += val1;
    num++;
  }

  // Rotate variables back
  val1 = val2;
  val2 = val3;
  val3 = current;

  //Divide by the average
  output /= num;

  prevAvg = output;
  return output;
}

float sign(float val) {
  return  (val > 0) - (val < 0);
}

float square(float val) {
  return val * val;
}

// vim: tabstop=2 shiftwidth=2 expandtab
