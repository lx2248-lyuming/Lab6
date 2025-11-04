unsigned long lastStepTime = 0;
int stepLength = 250;
int currentStep = 0;
int totalSteps = 4;
int buttonPinsArray[4] = { 33, 34, 35, 36 };
int totalButtonPins = 4;
int ledPinsArray[4] = { 9, 10, 11, 12 };
int totalLeds = 4;
bool buttonStateArray[4] = { HIGH, HIGH, HIGH, HIGH };
bool lastButtonStateArray[4] = { LOW, LOW, LOW, LOW };

int brightnessPotPin = A14;
int brightnessPotValue = 0;
int ledBrightness = 0;
int speedPot = A13;

int channelButtonPin = 28;
int channelDisplayed = 0;
bool channellastButtonState = LOW;
bool channelButtonState = HIGH;
int channelLedsArray[3] = { 41, 40, 39 };
int totalChannelLeds = 3;
bool buttonOn[3][4] = {
  { HIGH, LOW, HIGH, LOW },
  { LOW, HIGH, LOW, HIGH },
  { HIGH, HIGH, HIGH, HIGH },
};

int midiNotes[3] = { 36, 38, 42 };

int button = 13;

int led = 14;



void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPinsArray[i], INPUT);
    pinMode(ledPinsArray[i], OUTPUT);
  }
  pinMode(channelButtonPin, INPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(channelLedsArray[i], OUTPUT);
  }
}

void checkChannelLeds() {
  for (int i = 0; i < 3; i++) {
    if (i == channelDisplayed) {
      digitalWrite(channelLedsArray[i], HIGH);
    } else {
      digitalWrite(channelLedsArray[i], LOW);
    }
  }
}

void checkChannelButton() {
  channelButtonState = digitalRead(channelButtonPin);
  // Serial.println(channelButtonState);
  // delay(50);
  if (channellastButtonState == LOW and channelButtonState == HIGH) {
    channelDisplayed = channelDisplayed + 1;
    if (channelDisplayed > 2) {
      channelDisplayed = 0;
    }
    Serial.println(channelDisplayed);
    delay(5);
  } else if (channellastButtonState == HIGH and channelButtonState == LOW) {
    delay(5);
  }
  channellastButtonState = channelButtonState;
}

void checkButtons() {
  for (int i = 0; i < 4; i++) {
    buttonStateArray[i] = digitalRead(buttonPinsArray[i]);
    if (lastButtonStateArray[i] == LOW and buttonStateArray[i] == HIGH) {
      buttonOn[channelDisplayed][i] = !buttonOn[channelDisplayed][i];
    }
    lastButtonStateArray[i] = buttonStateArray[i];
  }
}

void checkLeds() {
  for (int i = 0; i < 4; i++) {
    if (buttonOn[channelDisplayed][i]) {
      brightnessPotValue = analogRead(brightnessPotPin);
      ledBrightness = map(brightnessPotValue, 0, 1023, 0, 255);
      analogWrite(ledPinsArray[i], ledBrightness);
    } else {
      analogWrite(ledPinsArray[i], 0);
    }
    analogWrite(ledPinsArray[currentStep], 255);
  }
}

void sequence() {
  if (millis() >= lastStepTime + stepLength) {
    lastStepTime = millis();
    currentStep = currentStep + 1;
    if (currentStep >= totalSteps) {
      currentStep = 0;
    }


    for (int i = 0; i < 3; i++) {
      if (buttonOn[i][currentStep]) {
        usbMIDI.sendNoteOff(midiNotes[i], 0, 1);
        usbMIDI.sendNoteOn(midiNotes[i], 127, 1);
      }
    }
  }
  stepLength = analogRead(speedPot);
}


void loop() {
  sequence();
  checkButtons();
  checkLeds();
  checkChannelButton();
  checkChannelLeds();
}