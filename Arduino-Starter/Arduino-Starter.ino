#define TEAM_NUMBER 1

void setup() {
  Serial.begin(115200);
}

void loop() {
  float rx = axisRX();
  float ly = axisLY();
  drive(ly + rx, ly - rx);
  
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
  Serial.print(axisLT());
  Serial.print(" ");
  Serial.print(axisRT());
  Serial.print(" ");
  Serial.println(dpad());*/
  delay(20);
}
