#include <Servo.h>

//----- H-Bridge Pins --------
#define IN1 2
#define IN2 4
#define ENA 3 

#define IN3 7
#define IN4 8
#define ENB 6

//--------- Servo ---------
#define servopin 5

//--------ultrasonic------
#define trigPin 10
#define echoPin A0
float distance,time ;
bool stopFlag=false;

//------leds------------
#define backled 13
#define reverseLED 12
#define frontLED 11
bool isOn=false;

//-------buzzer------
#define buzz 9
bool horn=false;
//-------Variables-------
char read = '0';
char lastcommand;
char direction='0';
bool alert = false;  
int speed=255; 
int steerPosition = 90;     
Servo steer;

//-----timing-----------
unsigned long alertTime=millis();
bool mask=false;
unsigned long revTime=millis();
bool mask2=false;
unsigned long ledTime=millis();
bool mask3=false;
//----------------------------setup---------------------------------
void setup() {
  Serial.begin(9600);

//------ultrasonic--------
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);

//----------DC motors------
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
//  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
 // pinMode(ENB, OUTPUT);

//-------light system------
  pinMode(backled, OUTPUT);
  pinMode(frontLED, OUTPUT);
  pinMode(reverseLED, OUTPUT);

//------buzzer(horn)-------
pinMode(buzz, OUTPUT);

//----servo(steering)--------
  steer.attach(servopin);
  steer.write(steerPosition);


}

void loop() {

  //-----ultrasonic--------
  digitalWrite(trigPin,LOW);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  
  time= pulseIn(echoPin,HIGH);
  distance= 0.0343*(time/2);
  
  if(distance<=35){stopFlag=true;}
  else{stopFlag=false;}
  //-----------------------

  if (Serial.available()) {
    read = Serial.read();
    if (read != '0' && read != 'o' && read != 'U' && read != 'D') {
      lastcommand = read;
    }
  }

  switch (read) {
    case 'F':  
      if(!stopFlag){
        forward();
        digitalWrite(backled,LOW); 
        digitalWrite(buzz,LOW);
      } else {
        stop();
        digitalWrite(backled,HIGH);
        buzzAlert();
      }
      break;

    case 'B': 
      digitalWrite(backled,LOW); 
      backward(); 
      buzzRev();  
      ledRev(); 
      break;

    case 'L': 
      steerPosition = constrain(steerPosition + 5, 0, 180); 
      steer.write(steerPosition);  
      break;

    case 'R': 
      steerPosition = constrain(steerPosition - 5, 0, 180); 
      steer.write(steerPosition);  
      break;

    case 'N':  
      if (isOn) {
        digitalWrite(frontLED,LOW); 
        isOn=false;
      } else {
        digitalWrite(frontLED,HIGH); 
        isOn=true;
      } 
      break;

    case 'H': 
      digitalWrite(buzz,HIGH); 
      horn=true; 
      break;

    case 'o': 
      if(horn){
        digitalWrite(buzz,LOW); 
        horn=false;
      } 
      break;

    case 'U': 
      if (speed < 250) speed += 10;
      Serial.print("Speed increased to: ");
      Serial.println(speed);
      break;

    case 'D': 
      if (speed > 10 ) speed -= 10;
      Serial.print("Speed decreased to: ");
      Serial.println(speed);
      break;

    case '0':
      stop(); 
      steerPosition =90 ; 
      steer.write(steerPosition);  
      digitalWrite(backled,HIGH); 
      digitalWrite(buzz,LOW); 
      digitalWrite(reverseLED,LOW); 
      lastcommand='0';
      break;

    default:  
      stop(); 
      digitalWrite(buzz, LOW); 
      digitalWrite(reverseLED, LOW); 
      horn = false; 
      digitalWrite(backled,HIGH); 
      break;
  }

  // --- KEEP DRIVING at updated speed ---
  if (lastcommand == 'F' && !stopFlag) {
    forward();
  } else if (lastcommand == 'B') {
    backward();
  }

}


//-------functions------------

void forward() {
  analogWrite(IN1, 0);
  analogWrite(IN2, speed);
  analogWrite(IN3, 0);
  analogWrite(IN4, speed);
  direction='F';
  //analogWrite(ENA, 0);
  //analogWrite(ENB, 0);
}

void backward() {
  analogWrite(IN1, speed);
  analogWrite(IN2, 0);
  analogWrite(IN3, speed);
  analogWrite(IN4, 0);
  direction='B';
  //analogWrite(ENA, 0);
  //analogWrite(ENB, 0);
}

void stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  //analogWrite(ENA, 0);
 // analogWrite(ENB, 0);
}

void buzzRev(){

  if(millis()-revTime >= 500 && !mask2){
    digitalWrite(buzz,HIGH);
    revTime=millis();
    mask2=true;
  }
  if(millis()-revTime >= 500 && mask2){
    digitalWrite(buzz,LOW);
    revTime=millis();
    mask2=false;
  }
}

void buzzAlert(){

  if(millis()-alertTime >= 300 && !mask){
    digitalWrite(buzz,HIGH);
    alertTime=millis();
    mask=true;
  }
  if(millis()-alertTime >= 300 && mask){
    digitalWrite(buzz,LOW);
    alertTime=millis();
    mask=false;
  }
}

void ledRev(){

  if(millis()-ledTime >= 1000 && !mask3){
    digitalWrite(reverseLED,HIGH);
    ledTime=millis();
    mask3=true;
  }
  if(millis()-ledTime >= 1000 && mask3){
    digitalWrite(reverseLED,LOW);
    ledTime=millis();
    mask3=false;
  }
}