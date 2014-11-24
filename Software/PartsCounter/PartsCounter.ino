#include <PID_v1.h>

//ButtonConstants
#define debounceDelay 90
#define selectC 0x02
#define upC 0x10
#define downC 0x04
#define backC 0x01
#define pauseC 0x08
#define dispensePin 40
#define redPin 38
#define greenPin 39

//MotorConstants
#define motorPin 46
#define motorInA 50
#define motorInB 53
#define Kp 1
#define Ki 0
#define Kd 0
#define minPWM 32
#define sTime 20
#define numOfSamples 200
#define SPT 934
#define blink_time 750

typedef enum STATE_LCD
  {
      WELCOME,
      SELECT_PART,
      SELECT_AMOUNT,
      CONFIRM,
      LOADING,
      COUNTING,
      COMPLETE,
      PAUSE,
      ERROR_SENSOR,
      ERROR_MOTOR,
      ERROR_USER
  } state_lcd;
  
typedef enum STATE_MOTOR
  {
      ON,
      OFF,
  } state_motor;
  
typedef enum STATE_SENSOR
  {
      ACTIVE,
      STANDBY
  } state_sensor;

typedef enum STATE_LED
{
  ENABLED,
  DISABLED,
  BLINK,
} state_led;

typedef struct LCD_Controller
  {
     short amount;
     short part;
     boolean confirmation;
     state_lcd state; 
  } LCD_Controller;

typedef struct Buttons
  {
     byte select;
     long selectDB;
     boolean selectF;
     byte up;
     long upDB;
     boolean upF;
     byte down;
     long downDB;
     boolean downF;
     byte back;
     long backDB;
     boolean backF;
     byte pause;
     long pauseDB;
     boolean pauseF;
     byte dispense;
     long dispenseDB;
     boolean dispenseF;
  } Buttons;
  
typedef struct Motor_Controller
  {
     state_motor state;
     double peak;
     double pwm;
     double current;
     double set_point;
     int sample_num;
  } Motor_Controller; 
 
 typedef struct SENSOR_CONTROLLER
{
	boolean topHi;
	boolean btmHi;
	short top;
	short btm;
	short tempCount;
	short partCount;
//	long lastpartT;
//	long dpartT;
        state_sensor state;
} sensor_controller; 

typedef struct INDICATOR_CONTROLLER
{
     boolean red;
     long red_time;
     state_led red_state;
     boolean green;
     long green_time;
     state_led green_state;
} indicator_controller;

LCD_Controller LCD = {1,1,false,WELCOME};
Buttons BTNS = {0,0,false,0,0,false,0,0,false,0,0,false,0,0,false,0,0,false};
Motor_Controller motor = {OFF,0,0,0,SPT,0};
sensor_controller sensors = {false,false,0,0,0,0,STANDBY};
indicator_controller leds = {false,0,ENABLED,true,0,DISABLED};
PID PIDcontroller = PID(&motor.peak,&motor.pwm,&motor.set_point,Kp,Ki,Kd,DIRECT);
String part_name[] = {"50mm Cap Screw","4mm Hex Nut","25mm Cap Screw","10mm Hex Nut","Wall Anchor","Dry-wall Screw"};
state_lcd previous_state;

void setup()
  {
    Serial.begin(9600);
    setup_BTNS();
    setup_motor();
    setup_sensors();
    lcd_setup();
    update_LCD();
  }

void loop()
  {
    process_input();
    get_count();
    control_motor();
    control_leds();
  }

void update_LCD()
  {
    switch(LCD.state)
      {
          case WELCOME:
                clear_lcd();
                writeLCD(0x84);
                writeLCD("Welcome!");
                nextLine();
                for(int i = 0;i<16; i++)
                  {
                    writeLCD("*");
                    delay(100);
                  }
                 LCD_next_screen();
                 update_LCD();
            break;      
          case SELECT_PART:
                clear_lcd();
                writeLCD("Select Part:");
                nextLine();
                printLCD(part_name[LCD.part]);
            break;
          case SELECT_AMOUNT:
                clear_lcd();
                writeLCD("Select Amount:");
                nextLine();
                printLCD(String(LCD.amount));
            break;
          case CONFIRM:
            clear_lcd();
            writeLCD("Ready to begin?");
            nextLine();
            if(LCD.confirmation == false)
              {
                writeLCD("X No         Yes");
              }
            else
              {
                writeLCD(" No        X Yes");
              }
            break;
//          case LOADING:
//            clear_lcd();
//            writeLCD("Loading...");
//            break;
          case COUNTING:
            clear_lcd();
            writeLCD("Counting...");
            nextLine();
            printLCD("Parts: " + String(sensors.partCount));
            break;
          case COMPLETE:
            clear_lcd();
            writeLCD(" *  Ready to  *");
            nextLine();
            writeLCD(" *  Dispense! *");
            break;
          case PAUSE:
            clear_lcd();
            writeLCD("Count paused.");
            nextLine();
            printLCD("Parts: " + String(sensors.partCount));
            break;
          case ERROR_SENSOR:
            clear_lcd();
            setLCDCursor(5,1);
            writeLCD("ERROR!");
            nextLine();
            writeLCD("Sensor failure.");
            break;
          case ERROR_MOTOR:
            clear_lcd();
            setLCDCursor(5,1);
            writeLCD("ERROR!");
            nextLine();
            writeLCD("Motor failure.");
            break;
          case ERROR_USER:
            clear_lcd();
            setLCDCursor(5,1);
            writeLCD("ERROR!");
            nextLine();
            writeLCD("Dispenser open.");
            break;
      }
  }

void LCD_next_screen()
  {
    switch(LCD.state)
      {
          case WELCOME:
            LCD.state = SELECT_PART;
            break;      
          case SELECT_PART:
            LCD.state = SELECT_AMOUNT;
            break;
          case SELECT_AMOUNT:
            LCD.state = CONFIRM;
            break;
          case CONFIRM:
            sensors.state = ACTIVE;
            LCD.state = COUNTING;
            break;
//          case LOADING:
//            LCD.state = COUNTING;
//            break;
          case COUNTING:
            LCD.state = COMPLETE;
            break;
          case PAUSE:
            sensors.state = ACTIVE;
            LCD.state = COUNTING;
            break;  
          case COMPLETE:
            sensors.state = ACTIVE;
            LCD.state = COUNTING;
            break;
          case ERROR_SENSOR:
            sensors.state = STANDBY;
            LCD.state = SELECT_PART;
            break;
          case ERROR_MOTOR:
            sensors.state = STANDBY;
            LCD.state = SELECT_PART;
            break;
          case ERROR_USER:
            sensors.state = STANDBY;
            LCD.state = SELECT_PART;
            break;
      }
  }
  
  void LCD_previous_screen()
  {
    switch(LCD.state)
      {
          case WELCOME:
            break;      
          case SELECT_PART:
            break;
          case SELECT_AMOUNT:
            LCD.state = SELECT_PART;
            break;
          case CONFIRM:
            LCD.state = SELECT_AMOUNT;
            break;
          case LOADING:
            break;
          case COUNTING:
            break;
          case COMPLETE:
            break;
          case ERROR_SENSOR:
            break;
          case ERROR_MOTOR:
            break;
          case ERROR_USER:
            break;
      }
  }

void process_input()
  {
    byte buttonState = PINA;
    switch(LCD.state)
      {     
          case SELECT_PART:
            if(upBTN(buttonState))
              {
                if(LCD.part != 5)
                  {
                    LCD.part++;
                  }
                else
                  {
                    LCD.part = 0;
                  }
                update_LCD();
              }
            if(downBTN(buttonState))
              {
                if(LCD.part != 0)
                  {
                    LCD.part--;
                  }
                else
                  {
                    LCD.part = 5;
                  }
                update_LCD();
              }
            if(selectBTN(buttonState))
              {
                LCD_next_screen();
                update_LCD();
              }
            break;
          case SELECT_AMOUNT:
            if(upBTN(buttonState))
              {
                if(LCD.amount != 6)
                  {
                    LCD.amount++;
                  }
                else
                  {
                    LCD.amount = 1;
                  }
                update_LCD();
              }
            if(downBTN(buttonState))
              {
                if(LCD.amount != 1)
                  {
                    LCD.amount--;
                  }
                else
                  {
                    LCD.amount = 6;
                  }
                update_LCD();
              }
            if(selectBTN(buttonState))
              {
                LCD_next_screen();
                update_LCD();
              }
            if(backBTN(buttonState))
                {
                    LCD_previous_screen();
                    update_LCD();
                }
            break;
          case CONFIRM:
            if(upBTN(buttonState))
              {
                LCD.confirmation = !LCD.confirmation;
                update_LCD();
              }
            if(downBTN(buttonState))
              {
                LCD.confirmation = !LCD.confirmation;
                update_LCD();
              }
            if(selectBTN(buttonState))
              {
                if(LCD.confirmation)
                  {
                    leds.red_state = DISABLED;
                    leds.green_state = BLINK;
                    LCD_next_screen();
                    update_LCD();
                    motor.state = ON;
                    switch_motor();
                  }
              }
              if(backBTN(buttonState))
                {
                    LCD_previous_screen();
                    update_LCD();
                }
            break;
          case COUNTING:
            if(pauseBTN(buttonState))
              {
                leds.red_state = ENABLED;
                leds.green_state = DISABLED;
                leds.green = true;
                motor.state = OFF;
                switch_motor();
                LCD.state = PAUSE;
                update_LCD();
              }
            break;
          case PAUSE:
            if(pauseBTN(buttonState))
              {
                leds.red_state = DISABLED;
                leds.green_state = BLINK;
                motor.state = ON;
                switch_motor();
                LCD.state = COUNTING;
                update_LCD();
              }
          case COMPLETE:
            if(dispenseBTN(digitalRead(dispensePin)))
            {
                delay(1000);
                leds.red_state = DISABLED;
                leds.green_state = BLINK;
                sensors.partCount = 0;
                sensors.state = ACTIVE;
                motor.state = ON;
                switch_motor();
                LCD_next_screen();
                update_LCD();
            }
            break;
          case ERROR_SENSOR:
  
            break;
          case ERROR_MOTOR:
  
            break;
          case ERROR_USER:
  
            break;
      }
  }
  
void control_leds()
{
 if(leds.red_state == ON && leds.red == false)
    {
      digitalWrite(redPin,HIGH);
      leds.red = true;
    } 
 if(leds.red_state == OFF && leds.red == true)
    {
      digitalWrite(redPin,LOW);
      leds.red = false;
    } 
 if(leds.green_state == ON && leds.green == false)
    {
      digitalWrite(greenPin,HIGH);
      leds.green = true;
    } 
 if(leds.green_state == OFF && leds.green == true)
    {
      digitalWrite(greenPin,LOW);
      leds.green = false;
    }
 if(leds.green_state == BLINK)
  {
    if(millis()-leds.green_time > blink_time)
    {
      digitalWrite(greenPin,leds.green);
      leds.green = !leds.green;
      leds.green_time = millis();
    }
  }
  if(leds.red_state == BLINK)
  {
    if(millis() - leds.red_time > blink_time)
    {
      digitalWrite(redPin,!leds.red);
      leds.red = !leds.red;
      leds.red_time = millis();
    }
  }
}
void setup_BTNS()
  {
      DDRA = 0x00;
      pinMode(dispensePin,INPUT);
      pinMode(greenPin,OUTPUT);
      pinMode(redPin,OUTPUT);
  }

boolean selectBTN(byte buttonVals)
  {
    byte btnstate = buttonVals&selectC;
    if (btnstate != BTNS.select)
      {
        BTNS.selectDB = millis();
        BTNS.select = btnstate;
        BTNS.selectF = true;
      }
    
    if ((millis() - BTNS.selectDB) > debounceDelay)
    {
        if(BTNS.selectF && BTNS.select != 0x00)
          {
            BTNS.selectF = false;
            return true;
          }
        else
          {
            return false;
          }
    }
    else
      {
        return false;
      }
  }

boolean upBTN(byte buttonVals)
  {
    byte btnstate = buttonVals&upC;
    
    if (btnstate != BTNS.up)
      {
        BTNS.upDB = millis();
        BTNS.up = btnstate;
        BTNS.upF = true;
      }
    
    if ((millis() - BTNS.upDB) > debounceDelay)
      {
          if(BTNS.upF && BTNS.up != 0x00)
            {
              BTNS.upF = false;
              return true;
            }
          else
            {
              return false;
            }
      }
    else
      {
        return false;
      }
  }

boolean downBTN(byte buttonVals)
  {
    byte btnstate = buttonVals&downC;
    
    if (btnstate != BTNS.down)
      {
        BTNS.downDB = millis();
        BTNS.down = btnstate;
        BTNS.downF = true;
      }
    
    if ((millis() - BTNS.downDB) > debounceDelay)
      {
          if(BTNS.downF && BTNS.down != 0x00)
            {
              BTNS.downF = false;
              return true;
            }
          else
            {
              return false;
            }
      }
    else
      {
        return false;
      }
  }

boolean backBTN(byte buttonVals)
  {
    byte btnstate = buttonVals&backC;
    
    if (btnstate != BTNS.back)
      {
        BTNS.backDB = millis();
        BTNS.back = btnstate;
        BTNS.backF = true;
      }
    
    if ((millis() - BTNS.backDB) > debounceDelay)
      {
          if(BTNS.backF && BTNS.back != 0x00)
            {
              BTNS.backF = false;
              return true;
            }
          else
            {
              return false;
            }
      }
    else
      {
        return false;
      }
  }

boolean pauseBTN(byte buttonVals)
  {
    byte btnstate = buttonVals&pauseC;
    
    if (btnstate != BTNS.pause)
      {
        BTNS.pauseDB = millis();
        BTNS.pause = btnstate;
        BTNS.pauseF = true;
      }
    
    if ((millis() - BTNS.pauseDB) > debounceDelay)
      {
          if(BTNS.pauseF && BTNS.pause != 0x00)
            {
              BTNS.pauseF = false;
              return true;
            }
          else
            {
              return false;
            }
      }
    else
      {
        return false;
      }
  }
  
boolean dispenseBTN(boolean buttonVals)
  {
    boolean btnstate = !buttonVals;
    
    if (btnstate != BTNS.dispense)
      {
        BTNS.dispenseDB = millis();
        BTNS.dispense = btnstate;
        BTNS.dispenseF = true;
      }
    
    if ((millis() - BTNS.dispenseDB) > debounceDelay)
      {
          if(BTNS.dispenseF && BTNS.dispense != 0x00)
            {
              BTNS.dispenseF = false;
              return true;
            }
          else
            {
              return false;
            }
      }
    else
      {
        return false;
      }
  }

//LCD Methods
void lcd_setup()
  {
    Serial1.begin(19200);
    writeLCD(0x16);
    writeLCD(0x11);
  }
void clear_lcd()
  {
    writeLCD(0x0C);
  }
void setLCDCursor(int x, int y)
  {
    writeLCD(128+20*y+x);
  }
void nextLine()
  {
    writeLCD(0x0D);
  }
void printLCD(String text)
  {
    Serial1.print(text);
  }
void writeLCD(char text[])
  {
    Serial1.write(text);
  }
void writeLCD(int text)
  {
    Serial1.write(text);
  }

//Motor Control Methods
void setPwmFrequency(int pin, int divisor)
  {
    byte mode;
    if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
      switch(divisor) {
        case 1: mode = 0x01; break;
        case 8: mode = 0x02; break;
        case 64: mode = 0x03; break;
        case 256: mode = 0x04; break;
        case 1024: mode = 0x05; break;
        default: return;
      }
      if(pin == 5 || pin == 6) {
        TCCR0B = TCCR0B & 0b11111000 | mode;
      } else {
        TCCR1B = TCCR1B & 0b11111000 | mode;
      }
    } else if(pin == 3 || pin == 11) {
      switch(divisor) {
        case 1: mode = 0x01; break;
        case 8: mode = 0x02; break;
        case 32: mode = 0x03; break;
        case 64: mode = 0x04; break;
        case 128: mode = 0x05; break;
        case 256: mode = 0x06; break;
        case 1024: mode = 0x7; break;
        default: return;
      }
      TCCR2B = TCCR2B & 0b11111000 | mode;
    }
  }
void control_motor()
  {
    if(motor.state == ON)
    {
      int reading = analogRead(15);
      if(reading > motor.peak)
        {
          motor.peak = reading;
        }
      motor.sample_num++;
      if(motor.sample_num >= numOfSamples)
        {
            motor.sample_num = 0;
            motor.peak = 1024-motor.peak;
            PIDcontroller.Compute();
            Serial.println(motor.pwm);
            analogWrite(motorPin,motor.pwm);
            motor.peak = 0;
        }
    }
  }
void setup_motor()
{
  analogReference(INTERNAL1V1);
  pinMode(motorPin,OUTPUT);
  pinMode(motorInA,OUTPUT);
  pinMode(motorInB,OUTPUT);  
  setPwmFrequency(motorPin,1);
  PIDcontroller.SetMode(AUTOMATIC);
  PIDcontroller.SetOutputLimits(minPWM,255);
  PIDcontroller.SetSampleTime(sTime);
}

void resetMotorVals()
{
  motor.peak = 0;
  motor.pwm = 0;
  motor.sample_num = 0;  
}

void switch_motor()
{
  if(motor.state == ON)
    {
      resetMotorVals();
      PIDcontroller.SetMode(AUTOMATIC);
      digitalWrite(motorInB,LOW);
      digitalWrite(motorInA,HIGH);
    }
  else
    {
      PIDcontroller.SetMode(MANUAL);
      digitalWrite(motorInB,LOW);
      digitalWrite(motorInA,LOW); 
    }
}
  
 //Sensor Module Methods
 void setup_sensors()
{
	 DDRC = 0x00;
}

void readSensors()
  {
   byte sensorsRead = PINC;
   sensors.top = sensorsRead & 0x0F;
   sensors.btm = sensorsRead & 0xF0;
  }
void get_count()
  {
    if(sensors.state == ACTIVE)
    {
      readSensors();
      
      if (sensors.top != 0x00 && !sensors.topHi)
        {
          sensors.topHi=true;
          sensors.tempCount ++;
        }
      else 
        {
          if(sensors.top == 0x00)
            {
            sensors.topHi=false;
            }
        }
      if (sensors.btm != 0x00 && !sensors.btmHi)
        {
          sensors.btmHi=true;
          sensors.tempCount ++;
        }
      else 
        {
          if(sensors.btm == 0x00)
            {
            sensors.btmHi=false;
            }
        }
      
      if (sensors.tempCount!=0 && sensors.tempCount%2==0)
        {
          sensors.partCount ++;
          sensors.tempCount = 0;
//          long now = millis();
//          sensors.dpartT = now - sensors.lastpartT;
//          sensors.lastpartT = now;
          if(sensors.partCount < LCD.amount)
            {
              update_LCD();
            }
          else
            {
              LCD_next_screen();
              leds.green_state = ENABLED;
              leds.red_state = DISABLED;
              update_LCD();
              motor.state = OFF;
              switch_motor();
            }
        }
    }
}

