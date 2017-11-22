#include <SoftwareSerial.h>

// Pin definition
#define IN1 5
#define IN2 6
#define IN3 11
#define IN4 12

// Serial definition
#define WHEEL_HEAD_CHAR   'W'
#define BUFFER_LENGTH     8
#define BAUDRATE          9600

// Control limit
#define MAX_SPEED   510
#define MIN_SPEED   0
#define STOP_SPEED  255

//#define DEBUG

int bufIndex = 0;
char recvBuf[BUFFER_LENGTH] = {0};
int recvCompelete = 0;

int in1_val = 0;
int in2_val = 0;
int in3_val = 0;
int in4_val = 0;

void ctrl_wheel()
{
  analogWrite(IN1, in1_val);
  analogWrite(IN2, in2_val);
  analogWrite(IN3, in3_val);
  analogWrite(IN4, in4_val);
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
      in1_val = 0;
      in2_val = 0;
    }
    else if(sal > 255)
    {
      in1_val = sal - 255;
      in2_val = 0;
    }
    else
    {
      in1_val = 0;
      in2_val = 255 - sal;
    }

    if(sar == 255)
    {
      in3_val = 0;
      in4_val = 0;
    }
    else if(sar > 255)
    {
      in3_val = sar - 255;
      in4_val = 0;
    }
    else
    {
      in3_val = 0;
      in4_val = 255 - sar;
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
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Setup serial
  Serial.begin(BAUDRATE);
  while(!Serial);

  // Initial control
  ctrl_wheel();
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

	// Echo
    Serial.println(recvBuf);
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


