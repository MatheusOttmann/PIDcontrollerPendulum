const int PWMA = 6; // pwa signal to motor driver
const int AIN1 = 5;
const int AIN2 = 4; // "power and ground" to the motor effectively

#define Encoder_output_A 2 
#define Encoder_output_B 3 // encoder pins used to keep track of motor rotations

double Count_pulses = 0;
double angle = 0;
int correction = 0;
double errorA = 0;
int errorP = 0;
int cumErrorA = 0;
int cumErrorP = 0;
int last_errorA = 0;
int last_erroP = 0;
double kpA = 100;
double kiA = 0.2;
double kpP = 0.2;
double kiP = 0;
int PID = 0;
int PID_A = 0;
int PID_P = 0;
int dir = 0;
unsigned long currentTime, elapsedTime, previousTime;
    
void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT); // outputs to send signal to driver

Serial.begin(9600); // activates the serial communication
pinMode(Encoder_output_A,INPUT); // sets the Encoder_output_A pin as the input
pinMode(Encoder_output_B,INPUT); // sets the Encoder_output_B pin as the input
attachInterrupt(digitalPinToInterrupt(Encoder_output_A),DC_Motor_Encoder,RISING); // runs DC_Motor_Encoder when A sees an interrupt
}

void loop() {
  find_angle();
  errorA = 90-angle;
  errorP = Count_pulses;
  calcPID();
  if(PID > 0)
  {
    analogWrite(PWMA,PID);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    dir = 0;
  }
  if(PID < 0)
  {
    analogWrite(PWMA, PID *-1);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    dir = 1;
  }
  //Serial.println(errorA);
  Serial.println(Count_pulses);

return;
}

void find_angle(){ //this function reads the analog pin of potentiometer, which returns 0-1024
double sensorValue = analogRead(A0);
angle = sensorValue * 360.0/1024; 
angle = angle+210.8;
if (angle>360){angle=angle-360;} //converts 0-1024 to 0-360 and set straight up as 90
}

void DC_Motor_Encoder(){ //keeps track of motor position
  //int b = digitalRead(Encoder_output_B);
  //Serial.println(b);
  if(dir == 0){
    Count_pulses++;
  }
  else{
    Count_pulses = Count_pulses-1;
  }
  
}

void calcPID()
{
  currentTime = millis();
  elapsedTime = currentTime - previousTime;
  
  cumErrorA += errorA * elapsedTime;
  cumErrorP += errorP * elapsedTime;
  PID_P = (kpP*(errorP)) + (kiP*(cumErrorP));  
  PID_A = (kpA*(errorA)) + (kiA*(cumErrorA));
  PID = PID_P +PID_A;
  previousTime = currentTime;

  PID = constrain(PID, -255, 255);
}
