#define safetyPin 0
#define lighterPin 1


void setup() {
  pinMode(safetyPin, OUTPUT);
  pinMode(lighterPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  digitalWrite(safetyPin, HIGH);
  delay(1000);
  digitalWrite(lighterPin, HIGH);
  delay(1000);
  digitalWrite(lighterPin, LOW);
  delay(1000);
  digitalWrite(safetyPin, LOW);
  Serial.print("Test.");
  Serial.print('\n');
  delay(3000);
}
