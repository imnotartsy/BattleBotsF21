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
float xspeed = 0;
float yspeed = 0;



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
  // X forward
  if (receive[5] > xcenter + tolerance){
     digitalWrite(IN1, HIGH);
     digitalWrite(IN2, LOW);
     xspeed = float((receive[5]-(xcenter + tolerance)))/float((xmax - (xcenter + tolerance)))*255;
  }
  // X backwards
  else if (receive[5] < xcenter - tolerance and receive[5] != -350){
     digitalWrite(IN1, LOW);
     digitalWrite(IN2, HIGH);
     xspeed = float((xcenter - tolerance) - receive[5])/float((xcenter - tolerance - xmin)) * 255;
  }
  // X off
  // (receive[5] < xcenter + tolerance) and (receive[5] > xcenter - tolerance)
  else { 
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    xspeed = 0;
  }
  analogWrite(ENA, xspeed);
  Serial.print("\tX: ");
  Serial.print(xspeed);
  

  // Y forward
  if (receive[4] > ycenter + tolerance){
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
     yspeed = float((receive[4]-(ycenter + tolerance)))/float((ymax - (ycenter + tolerance)))*255;
  }
  // Y backwards
  else if (receive[4] < ycenter - tolerance and receive[4] != -350){
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
     yspeed = float((ycenter - tolerance) - receive[4])/float((ycenter - tolerance - ymin)) * 255;
  }
  // Y Off
  else { 
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    yspeed = 0;
  }
  analogWrite(ENB, yspeed);
  Serial.print(" Y: ");
  Serial.print(yspeed);
  Serial.print('\n');


  delay(20);
}
