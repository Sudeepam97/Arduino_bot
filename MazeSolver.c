/*The following code is for a Line follower/Maze Solver Robot, based on
  Arduino UNO and LDR Sensors. The code is written in such a way that 
  the bot will follow a white line made on a black surface.
  
  In its first run through the maze, i.e trial run, the bot will store 
  all the correct paths to be taken at a 'T like' condition (A point 
  where the bot has more than one direction to go to) and then use these 
  conditions in the second run, or the final run, to solve the maze 
  in a relatively shorter amount of time. */

#include <EEPROM.h> 
//opens access to the EEPROM of Arduino that will be used to store correct paths

int w, x, y, z, es, avgx, avgy, avgz, avgw, avges;
// w is the front sensor, x is the left sensor, y the central sensor, and z is the right sensor
// es is used to diffrentiate between the end point and the leftstraightright condition.

int maxx= 0, minx= 1023, maxw= 0, minw= 1023, maxy= 0, miny= 1023, maxz= 0, minz= 1023, mines= 1023, maxes= 0; 
//max has been initialized with the min output reading possible of LDR and min with the max output reading possible of the LDR

int l1 = 11, l2 = 10, r1 = 9, r2 = 3;
//l1(positive for left motor) and l2(negative for left motor), similarly for right motors

int rundone = 4;
//For the LED that will blink at the end of the run

int memory[250]; int mcounter = -1;
//memory[] is the memory array, mcounter is to track array index, runnumber helps diffrenciate between trial and final run

int runnumip = 7;   //switch connected to digital pin 7, press this if its the first run. Do not press it if it is the second run.
int val = 0;        //val is a digital variable that will store HIGH or LOW according to the input at pin 7  
int runnumber = 1;  //variable to store the runnumber.
int startcalib = 8; //LED that indicates the start of calibration

// Digital Pins still available are 2, 12, 13
// PWM Pins still available are 5, 6

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

  pinMode(A4, INPUT); //For the value of es (endsensor)
  
  pinMode(runnumip, INPUT);     //sets the digital pin 7 as input
  
  val = digitalRead(runnumip);  // read the digital input pin

  if (val == HIGH)
    {
      runnumber = 1;
      cleareeprom(); //sets the values of all eeprom elements to zero. Takes approx 3.38 seconds.
    }

  else if (val == LOW) 
    {
      runnumber = 2;
    }

  Serial.print("runnumber is: ");
  Serial.print(runnumber);

  digitalWrite(startcalib, HIGH); //Get ready for calibration
  delay(1000);
  digitalWrite(startcalib, LOW);  //Start calibration
  delay(1000);

  callib();

  digitalWrite(startcalib, HIGH);
  delay(5000);
  digitalWrite(startcalib, LOW);
}

void loop()
{
  x = analogRead(A0);
  y = analogRead(A1);
  z = analogRead(A2);
  w = analogRead(A3);

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

  else if (w == 1 && x == 0 && y == 0 && z == 1 )
    left();

  else if (w == 1 && x == 1 && y == 0 && z == 0)
    right();

  else if (w == 1 && x == 0 && y == 1 && z == 1 || w == 1 && x == 1 && y == 0 && z == 1)
    softleft();
  
  else if (w == 1 && x == 1 && y == 1 && z == 0)
    softright();

  else if (w == 1 && x == 1 && y == 1 && z == 1)
    {
      mcounter++;
      turn();
    }
      
  else if (w == 1 && x == 0 && y == 0 && z == 0)
    {
      mcounter++;
      LeftRight();
    }

  else if (w == 0 && x == 1 && y == 0 && z == 0)
    {
      mcounter++;
      StraightRight();
    }

  else if (w == 0 && x == 0 && y == 0 && z == 1)
    {
      mcounter++;
      LeftStraight();
    }

  else if (w == 0 && x == 0 && y == 0 && z == 0)
    {
      int chend = checkend(); //1 if its the end. 0 if not.

      if (chend == 0)
      {
        mcounter++;
        LeftStraightRight();
      }
      else if (chend == 1)
      {
        endpoint();
      }
    }

  else
  {
    Straight();
  }
  

}


void cleareeprom()
{
  for (int i = 0 ; i < 1024 ; i++)
  {
    EEPROM.write(i, 0);
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
      mines =es;

    delay(1);
  }

  avgx  = (maxx  +  minx) / 2;
  avgy  = (maxy  +  miny) / 2;
  avgz  = (maxz  +  minz) / 2;
  avgw  = (maxw  +  minw) / 2;
  avges = (maxes + mines) / 2;

  Serial.print(" x average is:  ");
  Serial.print(avgx);
  
  Serial.print(" y average is:  ");
  Serial.print(avgy);
  
  Serial.print(" z average is:  ");
  Serial.print(avgz);
  
  Serial.print(" w average is:  ");
  Serial.print(avgw); 
  
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
  delay(500);
}

void softleft()
{
  analogWrite(l1, 0);
  analogWrite(l2, 50);
  analogWrite(r1, 50);
  analogWrite(r2, 0);
  delay(50);
}

void softright()
{
  analogWrite(r1, 0);
  analogWrite(r2, 50);
  analogWrite(l1, 50);
  analogWrite(l2, 0);
  delay(50);
}


/* We choose the following integer values, similar to the numpad of the keyboard, to make decisions,
   
   8: Means Straight
   4: Means Left
   6: Means Right
   2: Means Back

*/


void turn()
{
  memory[mcounter] == 2; //2 stands for back,
  analogWrite(l2,0);
  analogWrite(r1,0);
  analogWrite(l1,180);
  analogWrite(r2,180);
  delay(1000);
}

void LeftRight()
{
  if (runnumber == 1)
  {
    memory[mcounter] == 4; //4 stands for left
    checkPrevious();
    left();
  }

  else if (runnumber == 2)
  {
    memoryaction();
  }
}

void StraightRight()
{ 
  if (runnumber == 1)
  {
      memory[mcounter] == 8; //8 stands for straight
      checkPrevious();
      Straight();
  }

  else if (runnumber == 2)
  {
    memoryaction();
  }
}

void LeftStraight()
{
  if (runnumber == 1)
  {
    // The action that has to be performed on 'LeftStraight' is the same as 'LeftRight'
      LeftRight();
  }

  else if (runnumber == 2)
  {
    memoryaction();
  }
}

void LeftStraightRight()
{
  if (runnumber == 1)
  {
    // The action that has to be performed on 'LeftStraightRight' is the same as 'LeftRight'
      LeftRight();
  }
  
  else if (runnumber == 2)
  {
    memoryaction();
  }
}

int checkend()
{
    delay(42);

    for(int i=0; i<100; i++)
    es = es + analogRead(A4);

  es = es/100;
  
  if (es < avges)
    es = 0;
    else
    es = 1;
    
    return es;
}

void endpoint()
{
  stop();
  writeToMemory();
  digitalWrite(rundone, HIGH);
}

void stop()
{
  analogWrite(l1, 0);
  analogWrite(l2, 0);
  analogWrite(r1, 0);
  analogWrite(r2, 0);
}

void writeToMemory()
{
  int i;
  for(i = 0; i<mcounter; i++)
  {
    EEPROM.write(i, memory[i]);
  }
}

void checkPrevious()
{
  if (mcounter > 0 && mcounter-1 == 2)
    replacement();
}

void replacement()
{
  int c3 = memory[mcounter];
  int c1 = memory[mcounter-1];
  int c2 = memory[mcounter-2];

  if ( c2 == 4 && c1 == 2 && c3 == 6) //LBR = B
  {
    memory[mcounter-2] = 2;
    cleararray();
  }

  else if ( c2 == 4 && c1 == 2 && c3 == 8) //LBS = R
  {
    memory[mcounter-2] = 6;
    cleararray();
  }

  else if ( c2 == 4 && c1 == 2 && c3 == 4) //LBL = S
  {
    memory[mcounter-2] = 8;
    cleararray();
  }

  else if ( c2 == 8 && c1 == 2 && c3 == 4) //SBL = R
  {
    memory[mcounter-2] = 6;
    cleararray();
  }

  else if ( c2 == 8 && c1 == 2 && c3 == 8) //SBS = B
  {
    memory[mcounter-2] = 2;
    cleararray();
  }

  else if ( c2 == 6 && c1 == 2 && c3 == 4) //RBL = B
    {
    memory[mcounter-2] = 2;
    cleararray();
  }

}

void cleararray()
{
  memory[mcounter] = 0;
  memory[mcounter-1] = 0;
  mcounter = mcounter-2;
}

void memoryaction()
{
  int ch = EEPROM.read(mcounter);

  if (ch == 4)
    left();

  else if (ch == 8)
    Straight();

  else if (ch == 6)
    right();
}
