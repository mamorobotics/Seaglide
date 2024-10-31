int stepPin = 2;
int dirPin = 3;

int currentPos = 0;
int neutralPos = 0;
int floatPos = 1000;
int sinkPos = -1000;

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(13, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // digitalWrite(dirPin, HIGH);
  // digitalWrite(13, HIGH);

	// // Spin motor slowly
	// for(int x = 0; x < 400; x++)
	// {
	// 	digitalWrite(stepPin, HIGH);
	// 	delayMicroseconds(2000);
	// 	digitalWrite(stepPin, LOW);
	// 	delayMicroseconds(2000);
	// }
	// delay(1000); // Wait a second
	
	// // Set motor direction counterclockwise
	// digitalWrite(dirPin, LOW);

	// // Spin motor quickly
	// for(int x = 0; x < 400; x++)
	// {
	// 	digitalWrite(stepPin, HIGH);
	// 	delayMicroseconds(1000);
	// 	digitalWrite(stepPin, LOW);
	// 	delayMicroseconds(1000);
	// }
	// delay(1000);


  //Serial.println(currentPos);
  //Serial.println("1 neutral; 2 float; 3 sink: ");
  if(Serial.available()){
    char i = Serial.read();
    Serial.println(i);
    if(i == '1'){
      Serial.println("neutral");
      rotateToPos(neutralPos);
    }else if(i == '2'){
      Serial.println("float");
      rotateToPos(floatPos);
    }else if(i == '3'){
      Serial.println("sink");
      rotateToPos(sinkPos);
    }
  }
}

void rotateToPos(int pos){
  if(currentPos == pos){return;}
  int coeff = pos<currentPos ? -1 : 1;

  if(currentPos<0 && coeff==1){
    currentPos *= -1;
  }

  int steps = currentPos + (coeff * pos);

  currentPos = pos;

  if(coeff == -1){
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
  Serial.println(currentPos);
  delay(1000);
}
