#define motorPin 46
#define motorInA 50
#define motorInB 53

double percent=50; //can go from 0-255

void setup()
{
  pinMode(motorPin,OUTPUT);
  pinMode(motorInA,OUTPUT);
  pinMode(motorInB,OUTPUT);
  Serial.begin(9600);
  analogWrite(motorPin,percent);
  digitalWrite(motorInB,LOW);
  digitalWrite(motorInA,HIGH);
}

void loop()
{
  Serial.println(analogRead(15));
  delay(200);
}
