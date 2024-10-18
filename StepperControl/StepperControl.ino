int stepPin = 4;
int dirPin = 4;

int currentPos = 0;
int neutralPos = 0;
int floatPos = 200;
int sinkPos = -200;

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  rotateToPos(400);
  Serial.println(currentPos);
  Serial.print("1 neutral; 2 float; 3 sink: ");
  int i = Serial.read();
  if(i == 1){
    rotateToPos(neutralPos);
  }else if(i == 2){
    rotateToPos(floatPos);
  }else if(i == 3){
    rotateToPos(sinkPos);
  }
}

void rotateToPos(int pos){
  int coeff = pos<currentPos ? -1 : 1;

  int steps = currentPos + (coeff * pos);

  currentPos = pos;

  if(steps < 0){
    digitalWrite(dirPin,HIGH);
  }else{
    digitalWrite(dirPin,LOW);
  }

  for(int x = 0; x < steps; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
  }
  delay(1000);
}
