#include <SoftwareSerial.h>

// Pin definition
#define A_OPEN_PIN    5
#define A_ENABLE_PIN  4
#define A_SPEED_PIN   3
#define A_BRAKE_PIN   2

#define B_OPEN_PIN    9
#define B_ENABLE_PIN  12
#define B_SPEED_PIN   11
#define B_BRAKE_PIN   10

#define SPEED_PIN_MODE  0x03

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

int motorL_open = 0;
int motorL_enable = 0;
int motorL_speed = 0;
int motorL_brake = 0;
int motorR_open = 0;
int motorR_enable = 0;
int motorR_speed = 0;
int motorR_brake = 0;

void ctrl_wheel()
{
  digitalWrite(A_ENABLE_PIN, motorR_enable);
  digitalWrite(A_OPEN_PIN, motorR_open);
  analogWrite(A_SPEED_PIN, motorR_speed);
  digitalWrite(A_BRAKE_PIN, motorR_brake);

  digitalWrite(B_ENABLE_PIN, motorL_enable);
  digitalWrite(B_OPEN_PIN, motorL_open);
  analogWrite(B_SPEED_PIN, motorL_speed);
  digitalWrite(B_BRAKE_PIN, motorL_brake);
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
      motorL_enable = LOW;
      motorL_open = HIGH;
      motorL_speed = 127;
      motorL_brake = HIGH;
    }
    else
    {
      motorL_enable = HIGH;
      motorL_open = LOW;
      motorL_speed = sal / 2;
      motorL_brake = LOW;
    }

    if(sar == 255)
    {
      motorR_enable = LOW;
      motorR_open = HIGH;
      motorR_speed = 127;
      motorR_brake = HIGH;
    }
    else
    {
      motorR_enable = HIGH;
      motorR_open = LOW;
      motorR_speed = sar / 2;
      motorR_brake = LOW;
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
  // Set speed pin frequency
  TCCR2B = (TCCR2B & 0b11111000) | SPEED_PIN_MODE;

  // Initial pin mode
  pinMode(A_OPEN_PIN, OUTPUT);
  pinMode(A_ENABLE_PIN, OUTPUT);
  pinMode(A_SPEED_PIN, OUTPUT);
  pinMode(A_BRAKE_PIN, OUTPUT);
  pinMode(B_OPEN_PIN, OUTPUT);
  pinMode(B_ENABLE_PIN, OUTPUT);
  pinMode(B_SPEED_PIN, OUTPUT);
  pinMode(B_BRAKE_PIN, OUTPUT);

  digitalWrite(A_OPEN_PIN, LOW);
  digitalWrite(A_ENABLE_PIN, LOW);
  digitalWrite(A_SPEED_PIN, LOW);
  digitalWrite(A_BRAKE_PIN, LOW);
  digitalWrite(B_OPEN_PIN, LOW);
  digitalWrite(B_ENABLE_PIN, LOW);
  digitalWrite(B_SPEED_PIN, LOW);
  digitalWrite(B_BRAKE_PIN, LOW);

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


