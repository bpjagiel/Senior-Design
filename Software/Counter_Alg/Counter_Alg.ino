//Constants
#define debounceDelay 90
#define selectC 8
#define upC 00
#define downC 00
#define backC 00
#define pauseC 00

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
      BRAKEHI,
      BRAKELOW
  } state_motor;
  
typedef enum STATE_SENSOR
  {
      ACTIVE,
      STANDBY
  } state_sensor;

typedef struct LCD_Controller
  {
     short count;
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
  } Buttons;

LCD_Controller LCD = {1,1,1,false,WELCOME};
Buttons BTNS = {0,0,false,0,0,false,0,0,false,0,0,false,0,0,false};
String part_name[] = {"part1","part2","part3","part4"};


void setup()
  {
    DDRA = 0x00;
    Serial.begin(9600);
  }

void loop()
  {
    byte input = PINA;
    if(selectBTN(input))
      {
      Serial.println("SELECT");
      } 
    process_input(); 
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
                    delay(250);
                  }
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
          case LOADING:
            clear_lcd();
            writeLCD("Loading...");
            break;
          case COUNTING:
            clear_lcd();
            writeLCD("Counting...");
            nextLine();
            printLCD("Parts: " + LCD.count);
            break;
          case COMPLETE:
            clear_lcd();
            writeLCD("Ready to Dispense!");
            break;
          case PAUSE:
            clear_lcd();
            writeLCD("Counting paused.");
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
            LCD.state = LOADING;
            break;
          case LOADING:
            LCD.state = COUNTING;
            break;
          case COUNTING:
            LCD.state = COMPLETE;
            break;
          case COMPLETE:
            LCD.state = COUNTING;
            break;
          case ERROR_SENSOR:
            LCD.state = SELECT_PART;
            break;
          case ERROR_MOTOR:
            LCD.state = SELECT_PART;
            break;
          case ERROR_USER:
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
                if(LCD.part != 6)
                  {
                    LCD.part++;
                  }
                else
                  {
                    LCD.part = 1;
                  }
                update_LCD();
              }
            if(downBTN(buttonState))
              {
                if(LCD.part != 1)
                  {
                    LCD.part--;
                  }
                else
                  {
                    LCD.part = 6;
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
                LCD.confirmation != LCD.confirmation;
                update_LCD();
              }
            if(downBTN(buttonState))
              {
                LCD.confirmation != LCD.confirmation;
                update_LCD();
              }
            if(selectBTN(buttonState))
              {
                if(LCD.confirmation)
                  {
                    LCD_next_screen();
                    update_LCD();
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
              //////////////////////////////////////////////////////////////////////////////////////////
              ///////////////////////////fill in code here//////////////////////////////////////////////
              //////////////////////////////////////////////////////////////////////////////////////////
              }
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
    
    if (btnstate != BTNS.back) {
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

//LCD Methods
void lcd_setup()
  {
    Serial1.begin(9600);
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
//int readMotor()
//{
//  
//}