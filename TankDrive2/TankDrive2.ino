/*
 * Battle Bots 2021 - Azrael
 * 
 * WARNING:
 *   - Do not upload code with bot on the ground.
 *   - When uploading (on MKR WiFi 1010), may need to disconnect reciever pins 
 *   
 *   11.17.21 - 23:56 (T)
 */



/** Weapon **/
#define safetyPin 0
#define lighterPin 1


/** Reciever/Transmitter **/
// Reciever Pins
int pins[] = {6, 7, 8, 9, 10, 11}; 

// Read-in Values
int receive[] = {0, 0, 0, 0, 0, 0};


/** Reciever/Transmitter Offsets **/
float xmin    = -24;
float xmax    = 227;
float xcenter = (xmax - xmin)/2+xmin;
float ymin    = 1;
float ymax    = 212;
float ycenter = (ymax - ymin)/2+ymin;

#define rawtolerance 2

float x = 0;
float y = 0;

float tolerance = .03;


/** Motor Driver Pins **/
// Pins
#define IN1  13
#define IN2  12
#define ENA 2

#define IN3  4
#define IN4  5
#define ENB 3

// Speeds
float speedA = 0;
float speedB = 0;



void setup() {
  Serial.begin(9600);

  // Weapon Pins
  pinMode(safetyPin, OUTPUT);
  pinMode(lighterPin, OUTPUT);

  // Motor Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Receiver Pins
  for(int i = 0; i < 6; i++){
    pinMode(pins[i], INPUT);
  }
}

  float r = 0;
  float angle = 0;
  float a_norm = 0;
  float speed_norm = 0;

void loop() {

  /** Read Transmitter **/
  for(int i = 0; i < 6; i++){
    receive[i] = map(pulseIn(pins[i], HIGH), 1100, 1900, 0, 255);
    Serial.print("C");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.print(receive[i]);
    Serial.print('\t');
  }

  /** Update MOSFET States (Lighter) **/
  // TODO: implement safety refresh
  // Safety Read and Set
  if(receive[0] > 200){
    digitalWrite(safetyPin, HIGH);
    Serial.print("SAFEY OFF");
    Serial.print('\n');
  }
  else{
    digitalWrite(safetyPin, LOW);
  }

  // Lighter Read and Set
  if (receive[2] > 130){
    digitalWrite(lighterPin, HIGH);
    Serial.print("LIGHTER ON");
    Serial.print('\n');
  }
  else{
    digitalWrite(lighterPin, LOW);
  }


  /* MOTOR CONTROL */
  // normalize to 0 to 1
  x = (receive[5] - xmin)/(xmax - xmin);
  y = (receive[4] - ymin)/(ymax - ymin);

  // normalize to -1 to 1 ==> drive direction based on +/-
  x = x * 2 - 1;
  y = y * 2 - 1;

  // calculate radius [0-1]
  r = sqrt(pow(x, float(2)) + pow(y, float(2)));
  // truncate r
  r = r > 1 ? 1 : r;

  // angle [0 ]
  angle = atan(y/x) * 180/3.14159265;

  a_norm = (angle+90/180)/90;
  // add A tolerances

  speed_norm = r*abs(255);

  Serial.print('\t');
  Serial.print(" X: ");
  Serial.print(x);
  Serial.print('\t');
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print('\t');
  Serial.print('\t');
  
  Serial.print(" R: ");
  Serial.print(r);
  Serial.print('\t');
  Serial.print(" A: ");
  Serial.print(angle);
  Serial.print('\t');
  Serial.print(" A: ");
  Serial.print(a_norm);
  
  
  Serial.print('\n');

  // straight
  if (((y > tolerance) or (y < - tolerance)) and ((x < tolerance) and (x > -tolerance))){
    speedA = speed_norm;
    speedB = speed_norm; 
  }
  // right turn
  else if (x > tolerance){
    speedA = speed_norm;
    speedB = speed_norm * (1 - a_norm); 
  }
  // left turn
  else if(x < -tolerance){
    speedA = speed_norm * (1 - a_norm);
    speedB = speed_norm; 
  }


  // forward
  if (y > tolerance){
     digitalWrite(IN1, HIGH);
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
  }
  // backwards
  else if (y < - tolerance){
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  // off
  else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

//  // Off
//  if ((((receive[4] < ycenter + tolerance) and (receive[4] > ycenter - tolerance)) and // Y center
//      ((receive[5] < xcenter + tolerance) and (receive[5] > xcenter - tolerance))) or  // X center
//      (receive[4] == -350 or receive[5] == -350)){
//    digitalWrite(IN1, LOW);
//    digitalWrite(IN2, LOW);
//    digitalWrite(IN3, LOW);
//    digitalWrite(IN4, LOW);
//    speedA = 0;
//    speedB = 0;
//     }
//  // Full forward
//  else if (receive[4] > ycenter + tolerance and ((receive[5] < xcenter + tolerance) and (receive[5] > xcenter - tolerance))){
//     digitalWrite(IN1, HIGH);
//     digitalWrite(IN2, LOW);
//     digitalWrite(IN3, HIGH);
//     digitalWrite(IN4, LOW);
//     
//     speedA = float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255;
//     speedB = float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255;
//  }
//  // Full backward
//  else if (receive[4] < ycenter - tolerance and ((receive[5] < xcenter + tolerance) and (receive[5] > xcenter - tolerance))){
//     digitalWrite(IN1, LOW);
//     digitalWrite(IN2, HIGH);
//     digitalWrite(IN3, LOW);
//     digitalWrite(IN4, HIGH);
//     
//     speedA = float(float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*-255);
//     speedB = float(float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*-255);
//  }
//  else { 
//    digitalWrite(IN3, LOW);
//    digitalWrite(IN4, LOW);
//    speedA = 0;
//    speedB = 0;
//  }
//  analogWrite(ENA, speedA);
//  analogWrite(ENB, speedB);
//  
  
//  Serial.print(" A: ");
//  Serial.print(speedA);
//  Serial.print('\t');
//  Serial.print(" B: ");
//  Serial.print(speedB);
//  Serial.print('\n');


  delay(250);
}
