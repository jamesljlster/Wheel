#include <SoftwareSerial.h>

// Pin definition
#define PWMA  11
#define PWMB  10

#define AIN1  9
#define AIN2  8
#define STBY  7
#define BIN1  6
#define BIN2  5

// Serial definition
#define WHEEL_HEAD_CHAR   'W'
#define BUFFER_LENGTH     8
#define BAUDRATE          38400

// Control limit
#define MAX_SPEED   510
#define MIN_SPEED   0
#define STOP_SPEED  255

#define RESP  "WOK"

//#define DEBUG

int bufIndex = 0;
char recvBuf[BUFFER_LENGTH] = {0};
int recvCompelete = 0;

int standby = HIGH;

int motorL_speed = 0;
int motorL_in1 = 0;
int motorL_in2 = 0;

int motorR_speed = 0;
int motorR_in1 = 0;
int motorR_in2 = 0;

void ctrl_wheel()
{
  digitalWrite(STBY, standby);

  analogWrite(PWMA, motorR_speed);
  digitalWrite(AIN1, motorR_in1);
  digitalWrite(AIN2, motorR_in2);
  
  analogWrite(PWMB, motorL_speed);
  digitalWrite(BIN1, motorL_in1);
  digitalWrite(BIN2, motorL_in2);
}

void update_delta()
{
  int sal = 0;
  int sar = 0;

  sal = (recvBuf[1] - '0') * 100 + (recvBuf[2] - '0') * 10 + (recvBuf[3] - '0');
  sar = (recvBuf[4] - '0') * 100 + (recvBuf[5] - '0') * 10 + (recvBuf[6] - '0');

  if(sal >= 0 && sal <= 510 && sar >= 0 && sar <= 510)
  {
    if(sal == 255)
    {
      motorL_speed = 255;
      motorL_in1 = HIGH;
      motorL_in2 = HIGH;
    }
    else
    {
      int speedTmp = sal - 255;
      motorL_speed = abs(speedTmp);
      if(speedTmp > 0)
      {
        motorL_in1 = LOW;
        motorL_in2 = HIGH;
      }
      else
      {
        motorL_in1 = HIGH;
        motorL_in2 = LOW;
      }
    }

    if(sar == 255)
    {
      motorR_speed = 255;
      motorR_in1 = HIGH;
      motorR_in2 = HIGH;
    }
    else
    {
      int speedTmp = sar - 255;
      motorR_speed = abs(speedTmp);
      if(speedTmp > 0)
      {
        motorR_in1 = HIGH;
        motorR_in2 = LOW;
      }
      else
      {
        motorR_in1 = LOW;
        motorR_in2 = HIGH;
      }
    }
    
    #ifdef DEBUG
    Serial.print("SAL = ");
    Serial.println(sal);
    Serial.print("SAR = ");
    Serial.println(sar);
    #endif
  }
}

void setup()
{
  // Initial pin mode
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(STBY, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);

  // Setup serial
  Serial.begin(BAUDRATE);
  while(!Serial);

  // Initial control
  ctrl_wheel();

  // Send startup response
  Serial.print(RESP);
}

void loop()
{
  // If receiving compeleted, update control delta.
  if(recvCompelete)
  {
    // Update control variable
    update_delta();

    // Control wheel
    ctrl_wheel();

    // Reset receive status
    recvCompelete = 0;

    // Send response
    Serial.print(RESP);
  }
}

void serialEvent()
{
  if(recvCompelete == 0)
  {
    while(Serial.available())
    {
      char tmp = Serial.read();
      if(tmp == WHEEL_HEAD_CHAR)
        bufIndex = 0;
      recvBuf[bufIndex++] = tmp;
      if(bufIndex >= BUFFER_LENGTH - 1)
      {
        bufIndex = 0;
        if(recvBuf[0] == WHEEL_HEAD_CHAR)
        {
          recvCompelete = 1;
          break;
        }
      }
    }
  }
}


