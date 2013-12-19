
#include <SerialCommand.h>
SerialCommand sCmd;

const int SPEED_OUT = 5;
const int TURN_OUT = 3;
const int SPEED_OUT2 = 6;
const int TURN_OUT2 = 9;

const int SPEEDUP = 8;//YELLOW
const int SPEEDUP_SWITCH = 11;
const int SPEEDDOWN = 7;//ORANGE
const int SPEEDDOWN_SWITCH = 10;

const int POWER_SWITCH = 4;
const int POWER_RELAY = 2;

const int POWER_LED = A7;//1023 means off, 600 something is on

int speedOut = 0;
int turnOut = 0;

int speedVar = 512;
int turnVar = 512;
int lastCommand = 0;

void setup() {

  Serial.begin(9600);
  
  pinMode(POWER_SWITCH, INPUT_PULLUP);
  pinMode(POWER_RELAY, OUTPUT);
  digitalWrite(POWER_RELAY, HIGH);
  pinMode(SPEEDUP, OUTPUT);
  pinMode(SPEEDUP_SWITCH, INPUT_PULLUP);
  digitalWrite(SPEEDUP, HIGH);
  pinMode(SPEEDDOWN, OUTPUT);
  pinMode(SPEEDDOWN_SWITCH, INPUT_PULLUP);
  digitalWrite(SPEEDDOWN, HIGH);
  
  
  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("TURN",   turnCMD);          // sets turn
  sCmd.addCommand("SPEED",  speedCMD);         // Sets sped
  sCmd.addCommand("SPEEDLEVEL",  speedLevel);         // Sets sped
  sCmd.addCommand("POWER",  power);        // Toggles power
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  Serial.println("Ready");
}

void loop() {
  if(!digitalRead(POWER_SWITCH))
  {
    powerToggle();
  }
  if(!digitalRead(SPEEDUP_SWITCH)){
    speedUp();
  }
  if(!digitalRead(SPEEDDOWN_SWITCH)){
    speedDown();
  }
  sCmd.readSerial();
  
  speedOut = map(speedVar, 0, 1023, 50, 210);
  analogWrite(SPEED_OUT, speedOut);
  analogWrite(SPEED_OUT2, speedOut);
  turnOut = map(turnVar, 0, 1023, 50, 210);
  analogWrite(TURN_OUT, turnOut);
  analogWrite(TURN_OUT2, turnOut);
  
  delay(2);
  if(lastCommand > 3000){//~3 seconds
    turnVar = 512;
    speedVar = 512;
    lastCommand = 0;
  }else{
    lastCommand++;
  }
}

void speedCMD(){
  char *arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    speedVar = atoi(arg);
  }
  else {
    speedVar = 512;
  }
  lastCommand = 0;
}

void turnCMD(){
  char *arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    turnVar = atoi(arg);
  }
  else {
    turnVar = 512;
  }
  lastCommand = 0;
}
void power(){
  char *arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    if(atoi(arg) == 1){
      powerOn();
    }else if(atoi(arg) == 0){
      powerOff();
    }
  }
}

void powerToggle(){
  digitalWrite(POWER_RELAY, LOW);
  analogWrite(SPEED_OUT, 128);
  analogWrite(TURN_OUT, 128);
  analogWrite(SPEED_OUT2, 128);
  analogWrite(TURN_OUT2, 128);

  delay(100);
  digitalWrite(POWER_RELAY, HIGH);
  delay(900);
  lastCommand = 0;
}
void powerOn(){
  if(analogRead(POWER_LED) > 1000){
    powerToggle();
    Serial.println("Power On");
  }
}

void powerOff(){
  if(analogRead(POWER_LED) < 1000){
    powerToggle();
    Serial.println("Power Off");
  }
}

void speedLevel(){
  char *arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    if(atoi(arg) == 1){
      speedUp();
    }else if(atoi(arg) == -1){
      speedDown();
    }else{
      Serial.println("Invalid parameter");
      lastCommand = 0;
    }
  }
}

void speedUp(){
  digitalWrite(SPEEDUP, LOW);
  delay(100);
  digitalWrite(SPEEDUP, HIGH);
  Serial.println("Speed up");
  lastCommand = 0;
}

void speedDown(){
  digitalWrite(SPEEDDOWN, LOW);
  delay(100);
  digitalWrite(SPEEDDOWN, HIGH);
  Serial.println("Speed Down");
  lastCommand = 0;
}
//This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}
