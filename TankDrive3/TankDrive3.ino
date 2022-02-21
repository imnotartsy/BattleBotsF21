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
#define batteryPin 14


/** Reciever/Transmitter **/
// Reciever Pins
int pins[] = {6, 7, 8, 9, 10, 11}; 

// Read-in Values
int receive[] = {0, 0, 0, 0, 0, 0};


/** Reciever/Transmitter Offsets **/
float xmin    = -24;
float xmax    = 227;
float xcenter = (xmax - xmin)/2+xmin;
float ymin    = 3;
float ymax    = 208;
float ycenter = (ymax - ymin)/2+ymin;

#define rawtolerance 2

float x = 0;
float y = 0;

float tolerance = .06;


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
  pinMode(batteryPin, OUTPUT);

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

  if(receive[1] > 60){
    digitalWrite(batteryPin, HIGH);
    Serial.print("M3 ON");
    Serial.print('\n');
  }
  else{
    digitalWrite(batteryPin, LOW);
  }

  /* MOTOR CONTROL */
  // normalize to 0 to 1
  x = (receive[5] - xmin)/(xmax - xmin);
  y = (receive[4] - ymin)/(ymax - ymin);

  // normalize to -1 to 1 ==> drive direction based on +/-
  x = x * 2 - 1;
  y = y * 2 - 1;

  Serial.print('\t');
  Serial.print(" X: ");
  Serial.print(x);
  Serial.print('\t');
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print('\t');

//  // calculate radius [0-1]
//  r = sqrt(pow(x, float(2)) + pow(y, float(2)));
//  // truncate r
//  r = r > 1 ? 1 : r;
//
//  // angle [0 ]
//  angle = atan(y/x) * 180/3.14159265;
//
//  a_norm = (angle+90/180)/90;
//  // add A tolerances
//
//  speed_norm = r*abs(255);

    speed_norm = abs(y);

  
//  Serial.print('\t');
//  Serial.print('\n');
  
//  Serial.print(" R: ");
//  Serial.print(r);
//  Serial.print('\t');
//  Serial.print(" A: ");
//  Serial.print(angle);
//  Serial.print('\t');
//  Serial.print(" A: ");
//  Serial.print(a_norm);
  
 

  // straight
  if ((y > tolerance) and ((x < tolerance) and (x > -tolerance))){
    Serial.print("Q0\t");
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    speedA = y*255;
    speedB = y*255; 
  }
  // forward right turn
  else if ((x > tolerance) and (y > tolerance)){
    Serial.print("Q1\t");
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    speedA = abs(y)*255;
    speedB = abs(abs(y)*255 - 2*speedA*x); 
    if (y*255 - 2*speedA*x > 0){
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    }
    else{
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH); 
    }
    
  }
  // left turn
  else if ((x < - tolerance) and (y > tolerance)){
    Serial.print("Q2.0\t");
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    speedB = y*255; 
    speedA = abs(abs(y)*255 - 2*speedB*abs(x));
    if (abs(y)*255 - 2*speedB*abs(x) > 0){
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
    }
    else{
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH); 
    }
  }
  
  
  
  
  // backwards
  else if ((y < - tolerance) and ((x < tolerance) and (x > -tolerance))){
    Serial.print("Q-0\t");
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    speedA = abs(y)*255;
    speedB = abs(y)*255; 
  }
  // backwards right
  else if ((y < - tolerance) and (x > tolerance)){
    Serial.print("Q3\t");
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    speedA = abs(y)*255;
    speedB = abs(abs(y)*255 - 2*speedA*x); 
    if (abs(y)*255 - 2*speedA*abs(x) > 0){
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    }
    else{
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW); 
    }
    
  }
  // backwards left
  else if ((x < - tolerance) and (y < -tolerance)){
    Serial.print("Q4\t");
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    speedB = abs(y)*255; 
    speedA = abs(abs(y)*255 - 2*speedB*abs(x));
    if (abs(y)*255 - 2*speedB*abs(x) > 0){
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
    }
    else{
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW); 
    }
    
  }
  else if (receive[5] == -350 or receive[4] == -350){
    Serial.print("Q-OFF-\t");
    speedA = 0;
    speedB = 0;
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); 
    digitalWrite(IN4, LOW);
    
  }
  else{
    Serial.print("Q-F-\t");
    speedA = 0;
    speedB = 0;
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  
  if (speedA > 255){
    speedA = 255;
  }
  if (speedB > 255){
    speedB = 255;
  }

  Serial.print(" A: ");
  Serial.print(speedA);
  Serial.print('\t');
  Serial.print(" B: ");
  Serial.print(speedB);
  Serial.print('\n');

  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
  

  delay(250);
}
