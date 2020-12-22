int buzzer = 9;
void setup() {
   pinMode(buzzer, OUTPUT);
}
void loop() {
   for(int i=0; i<10; i++) {
	if(i%2 == 0) tone(buzzer,698);   
      else tone(buzzer, 523);  
      delay(500);
   }
   noTone(buzzer); // turn off the buzzer
   delay(2000);
}
