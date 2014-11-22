boolean topHi, btmHi;
short top, btm;
short tempCount, partCount;
long lastpartT,dpartT;

void setup(){
 Serial.begin(9600);
 Serial.println("Welcome!"); 
 DDRC = 0x00;
 topHi=false;
 btmHi=false;
 lastpartT = millis();
 tempCount = 0;
 partCount = 0;
}
void loop(){
  readSensors();
  
  if (top != 0x00 && !topHi)
    {
      topHi=true;
      tempCount ++;
    }
  else 
    {
      if(top == 0x00)
        {
        topHi=false;
        }
    }
  if (btm != 0x00 && !btmHi)
    {
      btmHi=true;
      tempCount ++;
    }
  else 
    {
      if(btm == 0x00)
        {
        btmHi=false;
        }
    }
  
  if (tempCount!=0 && tempCount%2==0)
    {
      partCount ++;
      tempCount = 0;
      long now = millis();
      dpartT = now - lastpartT;
      lastpartT = now;
      Serial.print("Count: ");
      Serial.println(partCount);
      Serial.print("Time: ");
      Serial.println(lastpartT);
      Serial.print("Time Between Parts: ");
      Serial.println(dpartT);
    }
}

void readSensors()
  {
   top = PINC & 0x0F;
   btm = PINC & 0xF0;
  }
