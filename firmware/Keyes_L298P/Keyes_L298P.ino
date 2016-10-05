#include <SoftwareSerial.h>

// Pin definition
#define MOTOR_A_PWM       10    // For right wheel spped
#define MOTOR_A_DIRECTION 12    // For right wheel run direction
#define MOTOR_B_PWM       11    // For left wheel speed
#define MOTOR_B_DIRECTION 13    // For left wheel run direction

// Serial definition
#define WHEEL_HEAD_CHAR   'W'
#define BUFFER_LENGTH     8
#define UNIT_LENGTH       3
#define BAUDRATE          38400

// Control limit
#define MAX_SPEED   510
#define MIN_SPEED   0
#define STOP_SPEED  255

//#define DEBUG

void Decode(char* recvBuf, int* leftSpeedPtr, int* rightSpeedPtr);
void CtrlMotor(int leftSpeed, int rightSpeed);
void RecvString(char* recvBuf);

void setup()
{
  Serial.begin(BAUDRATE);
  
  pinMode(MOTOR_A_DIRECTION, OUTPUT);
  pinMode(MOTOR_B_DIRECTION, OUTPUT);

  // Stop Motor A
  digitalWrite(MOTOR_A_DIRECTION, HIGH);
  analogWrite(MOTOR_A_PWM, 0);
  
  // Stop Motor B
  digitalWrite(MOTOR_B_DIRECTION, HIGH);
  analogWrite(MOTOR_B_PWM, 0);
}

void loop()
{
  int i;
  int counter;
  int leftSpeed = 0;
  int rightSpeed = 0;
  char tmpRead;
  char recvBuf[BUFFER_LENGTH] = {0};

  // Serial receiving
  RecvString(recvBuf);
  Serial.print(recvBuf);

  // Decode
  Decode(recvBuf, &leftSpeed, &rightSpeed);

  #ifdef DEBUG
  Serial.print("\n");
  Serial.print("SAL: ");
  Serial.print(leftSpeed);
  Serial.print(", SAR: ");
  Serial.print(rightSpeed);
  Serial.print("\n");
  #endif

  // Controlling
  CtrlMotor(leftSpeed, rightSpeed);
}

void Decode(char* recvBuf, int* leftSpeedPtr, int* rightSpeedPtr)
{ 
  int i;
  int leftSpeed;
  int rightSpeed;
  
  // Decoding
  leftSpeed = 0;
  for(i = 0; i < UNIT_LENGTH; i++)
  {
    leftSpeed = leftSpeed * 10 + (recvBuf[i + 1] - '0');
  }

  rightSpeed = 0;
  for(i = 0; i < UNIT_LENGTH; i++)
  {
    rightSpeed = rightSpeed * 10 + (recvBuf[i + UNIT_LENGTH + 1] - '0');
  }

  *leftSpeedPtr = leftSpeed;
  *rightSpeedPtr = rightSpeed;
}

void RecvString(char* recvBuf)
{
  int counter;
  char tmpRead;
  
  // Serial receiving
  counter = 0;
  while(1)
  {
    if(Serial.available())
    {
      tmpRead = Serial.read();
    }
    else
    {
      continue;
    }

    if(counter == 0 && tmpRead != WHEEL_HEAD_CHAR)
    {
      continue;
    }
    else
    {
      recvBuf[counter++] = tmpRead;
    }

    if(counter == BUFFER_LENGTH - 1)
    {
      break;
    }
  }
}

void CtrlMotor(int leftSpeed, int rightSpeed)
{
  if(leftSpeed >= 0 && leftSpeed <= 510 && rightSpeed >= 0 && rightSpeed <= 510)
  {
    // Control left motor
    if(leftSpeed >= STOP_SPEED)
    {
      digitalWrite(MOTOR_B_DIRECTION, HIGH);
      analogWrite(MOTOR_B_PWM, leftSpeed - STOP_SPEED);
    }
    else
    {
      digitalWrite(MOTOR_B_DIRECTION, LOW);
      analogWrite(MOTOR_B_PWM, STOP_SPEED - leftSpeed);
    }

    // Control right motor
    if(rightSpeed >= STOP_SPEED)
    {
      digitalWrite(MOTOR_A_DIRECTION, HIGH);
      analogWrite(MOTOR_A_PWM, rightSpeed - STOP_SPEED);
    }
    else
    {
      digitalWrite(MOTOR_A_DIRECTION, LOW);
      analogWrite(MOTOR_A_PWM, STOP_SPEED - rightSpeed);
    }
  }
}

