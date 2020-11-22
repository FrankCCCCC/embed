int button = 1;
int xAxis = A0,     yAxis = A1;
void setup() {
  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP); //return LOW when down
}
void loop() {
  int xVal = analogRead(xAxis);
  int yVal = analogRead(yAxis);
  int isPress = digitalRead(button);
  Serial.print("X="); Serial.print(xVal); Serial.print("\n");
  Serial.print("Y="); Serial.print(yVal); Serial.print("\n");
  if(isPress == 0) Serial.print("Button is pressed.\n");
  else Serial.print("Button is not pressed.\n");
  delay(200);
}
  
