
#include <SerialCommand.h>
SerialCommand sCmd;

const int SPEED_OUT = 5;
const int TURN_OUT = 3;
const int SPEED_OUT2 = 6;
const int TURN_OUT2 = 9;

const int SPEEDUP = 8;//YELLOW
const int SPEEDDOWN = 7;//ORANGE

const int POWER_SWITCH = 4;
const int POWER_RELAY = 2;

const int POWER_LED = A2;

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
  digitalWrite(SPEEDUP, HIGH);
  pinMode(SPEEDDOWN, OUTPUT);
  digitalWrite(SPEEDDOWN, HIGH);
  
  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("TURN",   turnCMD);          // sets turn
  sCmd.addCommand("SPEED",  speedCMD);         // Sets sped
  sCmd.addCommand("SPEEDLEVEL",  speedLevel);         // Sets sped
  sCmd.addCommand("POWER",  powerToggle);        // Toggles power
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  Serial.println("Ready");
}

void loop() {
  if(!digitalRead(POWER_SWITCH))
  {
    Serial.println("Power Switch");
    powerToggle();
  }
  sCmd.readSerial();     // We don't do much, just process serial commands
  
  speedOut = map(speedVar, 0, 1023, 50, 210);
  analogWrite(SPEED_OUT, speedOut);
  analogWrite(SPEED_OUT2, speedOut);
  turnOut = map(turnVar, 0, 1023, 50, 210);
  analogWrite(TURN_OUT, turnOut);
  analogWrite(TURN_OUT2, turnOut);
  
  Serial.println(analogRead(POWER_LED));
  
  
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

void powerToggle()
{
  digitalWrite(POWER_RELAY, LOW);
  analogWrite(SPEED_OUT, 128);
  analogWrite(TURN_OUT, 128);
  analogWrite(SPEED_OUT2, 128);
  analogWrite(TURN_OUT2, 128);

  delay(100);
  digitalWrite(POWER_RELAY, HIGH);
  delay(900);
  Serial.println("Power Toggled");
  lastCommand = 0;
}

void speedLevel(){
  char *arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    if(atoi(arg) == 1){
      digitalWrite(SPEEDUP, LOW);
      delay(100);
      digitalWrite(SPEEDUP, HIGH);
      Serial.println("Speed up");
    }else if(atoi(arg) == -1){
      digitalWrite(SPEEDDOWN, LOW);
      delay(100);
      digitalWrite(SPEEDDOWN, HIGH);
      Serial.println("Speed Down");
    }else{
      Serial.println("Invalid parameter");
    }
  }
  lastCommand = 0;
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}
