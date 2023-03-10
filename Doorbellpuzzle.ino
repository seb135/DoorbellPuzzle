#define DEBUG

const char* deviceID = "arduinoBellen";
const char* nextPuzzle = "ToDevice/arduinoNone";

const byte numInputs = 8;

const byte inputPins[numInputs] = {2, 3, 4, 5, 6, 7, 8, 9};

const byte numSteps = 8;

const byte steps[numSteps] = {0, 1, 2, 3, 4, 5, 6, 7};
const int lockPin = A0;
const int resetButton = A1;
 
bool lastInputState[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
int currentStep = 0;

//Debounce switch to prevent misreads
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {

  // Initialise the input pins that have switches attached
  for(int i=0; i< numInputs; i++){
    pinMode(inputPins[i], INPUT_PULLUP);
  }
  
  pinMode(lockPin, OUTPUT);
  digitalWrite(lockPin, LOW);

  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println(F("Serial communication started"));
  #endif
  EthernetSetup();
}

void loop() {

  if ( (millis() - lastDebounceTime) > debounceDelay) {
  
    for(int i=0; i<numInputs; i++){
      int currentInputState = digitalRead(inputPins[i]);

      if(currentInputState != lastInputState[i]) {
        lastDebounceTime = millis();    
      }

      if(currentInputState == LOW && lastInputState[i] == HIGH) {
        
        if(steps[currentStep] == i) {
          currentStep++;
          
          #ifdef DEBUG
            Serial.print(F("Correct input! Onto step #"));
            Serial.println(currentStep);
          #endif
        }
      
        else {
          currentStep = 0;
          Serial.println(F("Incorrect input! Back to the beginning!"));
        }
      }
      
      lastInputState[i] = currentInputState;
    }
  }

  if(currentStep == numSteps){
    currentStep = 0;
    onSolve();

    int resetState = digitalRead(resetButton);
    if(resetState == HIGH){
      onReset();
    }
  }

  mqttLoop();
}

void onReset(){
  publish("Bellen RESET");
  digitalWrite(lockPin, LOW);
  currentStep = 0;
}

void onSolve(){

  #ifdef DEBUG
  
    Serial.println(F("Puzzle Solved!"));
  #endif 

   publish("Bellen SOLVED");

  digitalWrite(lockPin, HIGH);
}
