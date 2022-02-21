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
#define xmin -27
#define xmax 230
#define xcenter 102
#define ymin 0
#define ymax 212
#define ycenter 101

#define tolerance 2


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
//  // forward
//  if (receive[5] > xcenter + tolerance){
//     digitalWrite(IN1, HIGH);
//     digitalWrite(IN2, LOW);
//     xspeed = float((receive[5]-(xcenter + tolerance)))/float((xmax - (xcenter + tolerance)))*255;
//  }
//  // X backwards
//  else if (receive[5] < xcenter - tolerance and receive[5] != -350){
//     digitalWrite(IN1, LOW);
//     digitalWrite(IN2, HIGH);
//     xspeed = float((xcenter - tolerance) - receive[5])/float((xcenter - tolerance - xmin)) * 255;
//  }
//  // X off
//  // (receive[5] < xcenter + tolerance) and (receive[5] > xcenter - tolerance)
//  else { 
//    digitalWrite(IN1, LOW);
//    digitalWrite(IN2, LOW);
//    xspeed = 0;
//  }
//  analogWrite(ENA, xspeed);
//  Serial.print("\tX: ");
//  Serial.print(xspeed);
  

  // Off
  if ((((receive[4] < ycenter + tolerance) and (receive[4] > ycenter - tolerance)) and // Y center
      ((receive[5] < xcenter + tolerance) and (receive[5] > xcenter - tolerance))) or  // X center
      (receive[4] == -350 or receive[5] == -350)){
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    speedA = 0;
    speedB = 0;
     }
  // Full forward
  else if (receive[4] > ycenter + tolerance and ((receive[5] < xcenter + tolerance) and (receive[5] > xcenter - tolerance))){
     digitalWrite(IN1, HIGH);
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
     
     speedA = float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255;
     speedB = float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255;
  }
  // Full backward
  else if (receive[4] < ycenter - tolerance and ((receive[5] < xcenter + tolerance) and (receive[5] > xcenter - tolerance))){
     digitalWrite(IN1, LOW);
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
     
     speedA = float(float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*-255);
     speedB = float(float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*-255);
  }
  // Forward Right Turn
  else if ((receive[4] > ycenter - tolerance) and (receive[5] > xcenter + tolerance)){
     digitalWrite(IN1, HIGH);
     digitalWrite(IN2, LOW);
     speedA = float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255;

     // Before Right Mid-point (R motor goes forward slower)
     if (receive[5] > ((xmax - xcenter)/2) + xcenter){
       digitalWrite(IN3, HIGH);
       digitalWrite(IN4, LOW);
       speedB = float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255
              - float((receive[5]-(xcenter + tolerance)))/float((xmax - (xcenter + tolerance)))*255;
     }
     // After Right Mid-point (R motor goes backward)
     else{
       digitalWrite(IN3, LOW);
       digitalWrite(IN4, HIGH);

       speedB = 2 * float((receive[5]-(xcenter + tolerance)))/float((xmax - (xcenter + tolerance)))*255
                -   float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255;
     }
     
  }
  // Backward Right(?) Turn
  else if ((receive[4] < ycenter - tolerance) and (receive[5] > xcenter + tolerance)){
     digitalWrite(IN1, LOW);
     digitalWrite(IN2, HIGH);
     speedA = float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255;

     // Before Right Mid-point (R motor goes forward slower)
     if (receive[5] > ((xmax - xcenter)/2) + xcenter){
       digitalWrite(IN3, LOW);
       digitalWrite(IN4, HIGH);
       speedB = float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255
              - float((xcenter - tolerance) - receive[5])/float((xcenter - tolerance - xmin)) * 255;
     }
     // After Right Mid-point (R motor goes backward)
     else{
       digitalWrite(IN3, HIGH);
       digitalWrite(IN4, LOW);

       speedB = 2* float((xcenter - tolerance) - receive[5])/float((xcenter - tolerance - xmin)) * 255
              - float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255;
     }
     
  }
  // Forward Left Turn
  else if ((receive[4] < ycenter - tolerance) and (receive[5] < xcenter - tolerance)){
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
     speedB = float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255;

     // Before Right Mid-point (R motor goes forward slower)
     if (receive[5] > ((xmax - xcenter)/2)){
       digitalWrite(IN3, HIGH);
       digitalWrite(IN4, LOW);
       speedA = float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255
              - float((xcenter - tolerance) - receive[5])/float((xcenter - tolerance - xmin)) * 255;
     }
     // After Right Mid-point (R motor goes backward)
     else{
       digitalWrite(IN3, LOW);
       digitalWrite(IN4, HIGH);

       speedB = 2* float((xcenter - tolerance) - receive[5])/float((xcenter - tolerance - xmin)) * 255
              - float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255;
     }
     
  }

//  // Turning: X backwards
//  else if (receive[5] > xcenter + tolerance){
//     digitalWrite(IN1, HIGH);
//     digitalWrite(IN2, LOW);
//     xspeed = float((receive[5]-(xcenter + tolerance)))/float((xmax - (xcenter + tolerance)))*255;
//  }
//  // X backwards
//  else if (receive[5] < xcenter - tolerance and receive[5] != -350){
//     digitalWrite(IN1, LOW);
//     digitalWrite(IN2, HIGH);
//     xspeed = float((xcenter - tolerance) - receive[5])/float((xcenter - tolerance - xmin)) * 255;
//  }
  // Y Off
  else { 
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    speedA = 0;
    speedB = 0;
  }
  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
  
  
  Serial.print(" A: ");
  Serial.print(speedA);
  Serial.print('\t');
  Serial.print(" B: ");
  Serial.print(speedB);
  Serial.print('\n');


  delay(250);
}
