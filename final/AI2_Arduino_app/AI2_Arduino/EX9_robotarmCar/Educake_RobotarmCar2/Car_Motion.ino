
void CarStop()
{
  Serial.println("Car Stop");
  
  //Right Motor
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  //Left Motor
  digitalWrite(7,LOW);
  digitalWrite(8,LOW);  
}
void CarForward()
{
  Serial.println("Car Forward");
  
  //Right Motor
  digitalWrite(4,HIGH);
  digitalWrite(5,LOW);
  //Left Motor
  digitalWrite(7,HIGH);
  digitalWrite(8,LOW);
}

void CarBack()
{
  Serial.println("Car Back");
  
  //Right Motor
  digitalWrite(4,LOW);
  digitalWrite(5,HIGH);
  //Left Motor
  digitalWrite(7,LOW);
  digitalWrite(8,HIGH);  
}

void CarLeft()
{
  Serial.println("Car Left");
  
  //Right Motor
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  //Left Motor
  digitalWrite(7,HIGH);
  digitalWrite(8,LOW);  
}

void CarRight()
{
  Serial.println("Car Right");
  
  //Right Motor
  digitalWrite(4,HIGH);
  digitalWrite(5,LOW);
  //Left Motor
  digitalWrite(7,LOW);
  digitalWrite(8,LOW);  
}

