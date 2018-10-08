/*The following code is for a Line follower/Maze Solver Robot, based on 
  Arduino UNO and LDR Sensors. The code is written in such a way that 
  the bot will follow a white line made on a black surface.*/

#include <EEPROM.h> 
//opens access to the EEPROM of Arduino that will be used to store correct paths

int w, wp, x, xp, y, yp, z, zp, es, avgx, avgy, avgz, avgw, avges;
// w is the front sensor, x is the left sensor, y the central sensor, and z is the right sensor
// wp,xp etc store feedback values.

int maxx= 0, minx= 1023, maxw= 0, minw= 1023, maxy= 0, miny= 1023, maxz= 0, minz= 1023, mines= 1023, maxes= 0; 
// max has been initialized with the min output reading possible of LDR and min with the max output reading possible of the LDR

int l1 = 11, l2 = 10, r1 = 9, r2 = 3;
// l1(positive for left motor) and l2(negative for left motor), similarly for right motors

int runnumber = 1;  //variable to store the runnumber.

void setup() 
{
  Serial.begin(9600);

  pinMode(l1, OUTPUT); //output at pin 11. Connect to pin 2 of l293d
  pinMode(l2, OUTPUT); //output at pin 10. Connect to pin 7 of l293d
  pinMode(r1, OUTPUT); //output at pin 9. Connect to pin 15 of l293d
  pinMode(r2, OUTPUT); //output at pin 3. Connect to pin 10 of l293d

  pinMode(A0, INPUT); //For the values of Sensor x
  pinMode(A1, INPUT); //For the values of Sensor y
  pinMode(A2, INPUT); //For the values of Sensor z
  pinMode(A3, INPUT); //For the values of Sensor w

  pinMode(A4, INPUT); //For the value of endsensor

  Serial.print("Run number is: ");
  Serial.print(runnumber);
  Serial.println("");
  
  callib();
  
  wp = avgw;
  xp = avgx;
  yp = avgy;
  zp = avgz;
}

void loop()
{
  x = 0.15*xp + 0.85*analogRead(A0);
  xp = x;
  y = 0.15*yp + 0.85*analogRead(A1);
  yp = y;
  z = 0.15*zp + 0.85*analogRead(A2);
  zp = z;
  w = 0.15*wp + 0.85*analogRead(A3);
  wp = w;
  
  Serial.print(" w is: ");
  Serial.print(w);
  Serial.print(" x is: ");
  Serial.print(x);
  Serial.print(" y is: ");
  Serial.print(y);
  Serial.print(" z is: ");
  Serial.print(z);
  Serial.println("");

  /*Sensor value less than average corosponds to a White surface and sensor 
    value greater than average corosponds to Black surface. We choose '0' 
    to represent a White surface and '1' to represent a Black surface. 
    If the bot has to run on a black line made on a white surface, just 
    change the 'less than' conditions to 'greater than' conditions*/

  if (x < avgx)
    x = 0;
  else
    x = 1;
  if (y < avgy)
    y = 0;
  else
    y = 1;
  if (z < avgz)
    z = 0;
  else
    z = 1;
  if (w < avgw)
    w = 0;
  else
    w = 1;

  if (w == 0 && x == 1 && y == 0 && z == 1)
    Straight();

  else if (w == 1 && x == 0 && y == 0 && z == 1)
    left();

  else if (w == 1 && x == 1 && y == 0 && z == 0)
    right();
  
  else if (w == 1 && x == 0 && y == 1 && z == 1 || w == 1 && x == 1 && y == 0 && z == 1)
    softleft();
  
  else if (w == 1 && x == 1 && y == 1 && z == 0)
    softright();

  else if (w == 1 && x == 1 && y == 1 && z == 1)
    {
      turn();
    }

  else if (w == 1 && x == 0 && y == 0 && z == 0)
    {
      LeftRight();
    }

  else if (w == 0 && x == 1 && y == 0 && z == 0)
    {
      StraightRight();
    }

  else if (w == 0 && x == 0 && y == 0 && z == 1)
    {
      LeftStraight();
    }

  else if (w == 0 && x == 0 && y == 0 && z == 0)
    {
      LeftStraightRight();
    }

  else
  {
    Straight();
  }
  

}

void callib() //Calibration takes approximately 12 seconds
{
  for (int i = 0; i < 8000; i++)
  { 
    x  = analogRead(A0); 
    y  = analogRead(A1);
    z  = analogRead(A2); 
    w  = analogRead(A3);
    es = analogRead(A4);
    
    if (x > maxx)
        maxx = x;
    if (x < minx)
      minx = x;
    
    if (y > maxy)
      maxy = y;
    if (y < miny)
      miny = y;
    
    if (z > maxz)
      maxz = z;
    if (z < minz)
      minz = z;
    
    if (w > maxw)
      maxw = w;
    if (w < minw)
      minw = w;
    
    if (es > maxes)
      maxes = es;
    if (es < mines)
      mines = es;

    delay(1);
  }

  avgx  = (maxx  +  minx) / 2;
  avgy  = (maxy  +  miny) / 2;
  avgz  = (maxz  +  minz) / 2;
  avgw  = (maxw  +  minw) / 2;
  avges = (maxes + mines) / 2;

  Serial.print(" w average is:  ");
  Serial.print(avgw); 
  Serial.print(" x average is:  ");
  Serial.print(avgx);
  Serial.print(" y average is:  ");
  Serial.print(avgy);
  Serial.print(" z average is:  ");
  Serial.print(avgz);
  Serial.print(" es average is: ");
  Serial.print(avges);
  Serial.println("");

}


/*A call to analogWrite() is on a scale of 0 - 255, such that analogWrite(255)
  requests a 100% duty cycle (always on), and analogWrite(127) is a 50% duty 
  cycle (on half the time) */

void Straight() 
{  
  analogWrite(l1, 160);
  analogWrite(l2, 0);
  analogWrite(r1, 160);
  analogWrite(r2, 0);
}

void left()
{
  analogWrite(l1, 0);
  analogWrite(l2, 110);
  analogWrite(r1, 110);
  analogWrite(r2, 0);
  delay(100);
}

void right()
{
  analogWrite(r1, 0);
  analogWrite(r2, 110);
  analogWrite(l1, 110);
  analogWrite(l2, 0);
  delay(100);
}

void turn()
{ 
  analogWrite(l1,0);
  analogWrite(l2,110);
  analogWrite(r1,110);
  analogWrite(r2,0);
  delay(200);
}

void softleft()
{
  analogWrite(l1, 0);
  analogWrite(l2, 60);
  analogWrite(r1, 60);
  analogWrite(r2, 0);
  delay(20);
}

void softright()
{
  analogWrite(r1, 0);
  analogWrite(r2, 60);
  analogWrite(l1, 60);
  analogWrite(l2, 0);
  delay(20);
}

void LeftRight()
{
  left();
}

void LeftStraight()
{ 
  left();
}

void LeftStraightRight()
{
  left();
}

void StraightRight()
{ 
  Straight();
}
