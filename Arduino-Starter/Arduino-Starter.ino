// Replace 12345 with the correct team number and then uncomment the line below.
#define TEAM_NUMBER 12345


#ifndef TEAM_NUMBER
#error "Define your team number with `#define TEAM_NUMBER 12345` at the top of the file."
#endif


void setup() {
  Serial.begin(115200);

  /*float rx = axisRX();
  float ly = axisLY();
  drive(ly + rx, ly - rx);*/
}

void loop() {
  float rx = axisRX();
  float ly = axisLY();

  setMotor1(ly + rx);
  setMotor2(ly - rx);
  setMotor3(rx - ly);
  setMotor4(rx + ly);

  setServo1((axisRY() + 1.0) * 90.0);
  setServo2((axisRY() + 1.0) * 90.0);
  setServo3((axisRY() + 1.0) * 90.0);
  setServo4((axisRY() + 1.0) * 90.0);

  Serial.print(getUltrasonic());
  Serial.print(" ");
  int sensors[6];
  getLineSensors(sensors);
  for (int i = 0; i < 6; ++i) {
    Serial.print(sensors[i]);
    Serial.print(" ");
  }
  Serial.println();


  int max_index = 0;
  int max_sensor = -10000;
  for (int i = 0; i < 6; ++i) {
    if (max_sensor < sensors[i]) {
      max_sensor = sensors[i];
      max_index = i;
    }
  }

  // 0 -> Needs to turn left
  // 5 -> Needs to turn right

  //drive(1.0 + (max_index - 2.5) * 0.15, 1.0 + (max_index - 0.25) * -0.15);

  //drive(0.0, 0.0);

  /*Serial.print(buttonA());
  Serial.print(" ");
  Serial.print(buttonB());
  Serial.print(" ");
  Serial.print(buttonX());
  Serial.print(" ");
  Serial.print(buttonY());
  Serial.print(" ");
  Serial.print(buttonLB());
  Serial.print(" ");
  Serial.print(buttonRB());
  Serial.print(" ");
  Serial.print(buttonBack());
  Serial.print(" ");
  Serial.print(buttonStart());
  Serial.print(" ");
  Serial.print(buttonL3());
  Serial.print(" ");
  Serial.print(buttonR3());
  Serial.print(" ");
  Serial.print(axisLX());
  Serial.print(" ");
  Serial.print(axisLY());
  Serial.print(" ");
  Serial.print(axisRX());
  Serial.print(" ");
  Serial.print(axisRY());
  Serial.print(" ");
  Serial.print(buttonLT());
  Serial.print(" ");
  Serial.print(buttonRT());
  Serial.print(" ");
  Serial.println(dpad());*/
  delay(20);
}
