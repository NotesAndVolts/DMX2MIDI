/******************************
   DMX2MIDI - v1.0
   for Teensy LC (www.pjrc.com)
   by Notes and Volts
   www.notesandvolts.com
 ******************************/

/******************************
   ** Upload Settings **
   Board: "Teensy LC"
   USB Type: "MIDI"
   CPU Speed: "48 Mhz"
 ******************************/

#include <TeensyDMX.h>
namespace teensydmx = ::qindesign::teensydmx;

//---SETTINGS----------------------------------------------------------
#define NUM_FIXTURES 3 // How many DMX channels? Set up fixtures below.
//---------------------------------------------------------------------

#define LED 16 // Pin for external LED
const bool DEBUG = true; // Set 'true' for serial debugging or 'false' to turn off

//**DO NOT TOUCH**
#define CC 0
#define NOTE 1
//****************

// DMX Receiver Serial Port
teensydmx::Receiver dmxRx{Serial1};

struct FIXTURE {
  int dmxChannel;
  byte midiChannel;
  byte midiValue;
  byte type;
  byte minimum = 0;
  byte maximum = 127;
  byte last = 0;
};

// Define Fixtures HERE: (DMX Channel(1-512), Midi Channel(1-16), Midi CC/Note number(0-127), Midi Type (CC or NOTE), Midi Minimum Value(optional), Midi Maximum Value(optional)
FIXTURE FIXTURES[NUM_FIXTURES] = {
  {15, 1, 85, CC},           //DMX:1  MIDI-Channel:1  MIDI-CC:85
  {16, 1, 86, CC, 63, 70},   //DMX:2  MIDI-Channel:1  MIDI-CC:86  Min:63  Max:70
  {17, 1, 20, NOTE},         //DMX:3  MIDI-Channel:1  MIDI-NOTE:20
};
// Make sure you update NUM_FIXTURES (top of program) to match number of fixtures defined.

void setup() {
  if (DEBUG) {
    Serial.begin(115200);
    while (!Serial && millis() < 4000) {
    }
    Serial.println("----DMX2MIDI-----");
    Serial.println("NotesAndVolts.com");
    Serial.println("-----------------");
    Serial.println();
    for (int i = 0; i < NUM_FIXTURES; i++) {
      Serial.printf("dmxChannel %d\n", FIXTURES[i].dmxChannel);
      Serial.printf("midiChannel %d\n", FIXTURES[i].midiChannel);
      Serial.printf("midiValue %d\n", FIXTURES[i].midiValue);
      Serial.printf("type %d\n", FIXTURES[i].type);
      Serial.printf("minimum %d\n", FIXTURES[i].minimum);
      Serial.printf("maximum %d\n", FIXTURES[i].maximum);
      //Serial.printf("last %d\n", FIXTURES[i].last);
      Serial.println("-----------------");
    }
  }
  // Activity LEDs set up
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWriteFast(LED_BUILTIN, HIGH);
  digitalWriteFast(LED, HIGH);
  // Start the DMX Receiver
  dmxRx.begin();
}

void loop() {
  usbMIDI.read();
  DMX2MIDI();
  blinkLed();
}

void DMX2MIDI() {
  static byte i = 0;
  byte value = dmxRx.get(FIXTURES[i].dmxChannel);

  if (value != FIXTURES[i].last) {
    if (DEBUG) Serial.printf("DMX Channel %d: %d\n", FIXTURES[i].dmxChannel, value);
    FIXTURES[i].last = value;
    value = value >> 1;

    switch (FIXTURES[i].type) {
      
      // MIDI CC
      case 0:
        if ((FIXTURES[i].minimum != 0) && (FIXTURES[i].maximum == 127)) {
          if (value < FIXTURES[i].minimum) value = FIXTURES[i].minimum;
        }
        else if ((FIXTURES[i].minimum == 0) && (FIXTURES[i].maximum != 127)) {
          if (value > FIXTURES[i].maximum) value = FIXTURES[i].maximum;
        }
        else if ((FIXTURES[i].minimum != 0) && (FIXTURES[i].maximum != 127)) {
          value = map(value, 0, 127, FIXTURES[i].minimum, FIXTURES[i].maximum);
        }

        usbMIDI.sendControlChange(FIXTURES[i].midiValue, value, FIXTURES[i].midiChannel);
        if (DEBUG) {
          Serial.printf("MIDI CC: %d VALUE: %d CHANNEL: %d\n", FIXTURES[i].midiValue, value, FIXTURES[i].midiChannel);
          Serial.println();
        }
        break;
        
      //MIDI Note
      case 1:
        if (value > 63) {
          usbMIDI.sendNoteOn(FIXTURES[i].midiValue, 127, FIXTURES[i].midiChannel);
          if (DEBUG) {
            Serial.printf("MIDI NOTE ON: %d VALUE: %d CHANNEL: %d\n", FIXTURES[i].midiValue, value, FIXTURES[i].midiChannel);
            Serial.println();
          }
        }
        else {
          usbMIDI.sendNoteOff(FIXTURES[i].midiValue, 0, FIXTURES[i].midiChannel);
          if (DEBUG) {
            Serial.printf("MIDI NOTE OFF: %d VALUE: %d CHANNEL: %d\n", FIXTURES[i].midiValue, value, FIXTURES[i].midiChannel);
            Serial.println();
          }
        }
        break;
    }

    digitalWriteFast(LED_BUILTIN, LOW);
    digitalWriteFast(LED, LOW);
  }

  i++;
  if (i >= NUM_FIXTURES) i = 0;
}

void blinkLed() {
  static unsigned long previousMillis = 0;
  const int ledInterval = 75;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= ledInterval) {
    previousMillis = currentMillis;
    digitalWriteFast(LED_BUILTIN, HIGH);
    digitalWriteFast(LED, HIGH);
  }
}
