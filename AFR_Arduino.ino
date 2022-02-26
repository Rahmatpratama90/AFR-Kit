#include <Servo.h>

Servo myservo;
#define potentio A5 //A5
#define servo 10
#define o2 A0 //A0
#define OPENED 870 //was 1050 now is 870
#define CLOSED 1670 // was 1950 now is 1670
#define HOLD_MIN 1080 //
#define HOLD_MAX 1220
int angle,potval = 0;
int openValue = 0;
int closeValue = 0;
int actualValue,targetValue = 0;
int inertia = 16; // change this parameter to exepriment. Higher the value, slower the servo rotation speed

void setup()
{
  Serial.begin(115200);
  pinMode(potentio,INPUT);
  myservo.attach(10);

}

void loop()
{
  while(millis() < 7200000 ) // waits while 32 seconds exceed (start of the 32 second is at the power on of arduino
  {
    potval = analogRead(potentio);
    angle = map(potval,0,1024,600,2400);
    myservo.writeMicroseconds(angle); // sets the servo position according to the scaled value
    actualValue = angle;
    delay(15); //for servo to come that point
    Serial.print("POT CONTROL :");
    Serial.println(angle);
  }

  pd(); //function for controlling valve using o2 sensor.
}

// operative range of motor applied to valve is 870uSec(OPEN) to 1670usec(CLOSED)
// o2 significative range is 2.34V - 3.12V (38% - 47% of the valve)

int readO2Sensor()
{
  int o2value = analogRead(o2);
  return o2value;
}

int giveMicros(int percent)
{
  int milly = map(percent,0,100,OPENED,CLOSED); //IN THIS LINE TRY TO SWAP THE POSITION OF OPENED AND CLOSED. Do some test with this mod.
  return milly;
}

int activeRange(int readings)
{
  int volts = map(readings,477,640,38,47); // 38 and 47 are the % of opening of the valve.
  // can be adjusted to different values
  return volts;
}

void pd()
{
  int oxigen = readO2Sensor();
  if ((oxigen > 477) && (oxigen < 640)) //if the read o2 value is between the range, actuate the valve)
  {
    targetValue = giveMicros(activeRange(oxigen));
    if (actualValue > targetValue)
    {
      actualValue = actualValue - 1;
      delay(inertia/2);
    }
    else
    {
      actualValue = actualValue + 1;
      delay(inertia/2);
    }
    
  }
  if (oxigen < 477) // if the read O2 value is going below the low treshold, open the valve
  {
    actualValue--;
    if (actualValue < OPENED) actualValue = OPENED;
    delay(inertia);
    
  }
  if (oxigen > 640) // if the read O2 value is going over the high treshold, close the valve
  {
    // closing Valve
    actualValue++;
    if (actualValue > CLOSED) actualValue = CLOSED;
    delay(inertia);
  }
  
  myservo.writeMicroseconds(actualValue);
  
}
