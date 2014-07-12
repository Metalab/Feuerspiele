#include <MIDI.h>
#include "Adafruit_WS2801.h"
#include "SPI.h"

int valvePins[4] = {11,12,13,14};
int igniterPin = 10;
long lastEffect = 0;
long igniterSafetyTimeout = 10000; // 10 sec

// LED strip - Arduino
//uint8_t dataPin  = 2;
//uint8_t clockPin = 3;

// LED strip - Teensy 2.0
uint8_t dataPin  = 1;
uint8_t clockPin = 2;

#define NUMPIXELS 16
Adafruit_WS2801 strip = Adafruit_WS2801(NUMPIXELS, dataPin, clockPin);
// Optional: leave off pin numbers to use hardware SPI


struct Valve {
  int pin;
  bool state;
  long timeout;

  Valve() : pin(-1), state(false), timeout(0) { }
  void on() { digitalWrite(pin, 1); }
  void off() { digitalWrite(pin, 0); }

  static void checkTimeouts() {
    long m = millis();
    for (int i=0;i<4;i++) {
      if (valves[i].timeout <= m) valves[i].off();
    }

    if (m - lastEffect > igniterSafetyTimeout) digitalWrite(igniterPin, 0);
  }

  static Valve valves[4];
};

Valve Valve::valves[4];

int octaveToEffect(byte note) {
  int effect = -1;
  if (note < 48) effect = 0;
  else if (note < 60) effect = 1;
  else if (note < 72) effect = 2;
  else effect = 3;
  return effect;
}

int noteToEffect(byte note) {
  int effect = -1;
  switch (note) {
  case 60:
    effect = 0;
    break;
  case 62:
    effect = 1;
    break;
  case 64:
    effect = 2;
    break;
  case 65:
    effect = 3;
    break;
  default:
    break;
  }
  return effect;
}

int (*effectMappingfunc)(byte) = octaveToEffect;


void hue2rgb(float h, uint32_t &rgb)
{
  int val = ((int)(h * 6 * 255)) % 255;
  int i = h * 6;
  if (i == 6) i = 5;
  uint8_t q = 255 - val;
  
  switch (i) {
  case 0: rgb = 0xff0000 | val << 8; break;
  case 1: rgb = 0x00ff00 | q << 16; break;
  case 2: rgb = 0x00ff00 | val; break;
  case 3: rgb = 0x0000ff | q << 8; break;
  case 4: rgb = 0x0000ff | val << 16; break;
  case 5: rgb = 0xff0000 | q ; break;
  }
}


void handleNoteOn(byte channel, byte pitch, byte velocity) {

  int effect = effectMappingfunc(pitch);
  if (effect >= 0) {
    Serial.print("Effect On: ");
    Serial.println(effect, DEC);
    digitalWrite(igniterPin, 1);
    lastEffect = millis();

    int duration = map(velocity, 0, 127, 0, 1000);
    Valve::valves[effect].timeout = millis() + duration;
    Valve::valves[effect].on();
  }
  else {
    Serial.print("NoteOn: ");
    Serial.println(pitch, DEC);
  }

  // 36..96 - 60 notes
  float hue = 1.0f * (pitch - 36) / 60;
  uint32_t rgb;
  hue2rgb(hue, rgb);

  for (uint8_t i=0;i<NUMPIXELS-1;i++) {
    strip.setPixelColor(i+1, strip.getPixelColor(i));
  }
  strip.setPixelColor(0, rgb);
  strip.show();
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  Valve::valves[effectMappingfunc(pitch)].off();
}

void setup()
{
  for (int i=0;i<4;i++) {
    Valve::valves[i].pin = valvePins[i];
    pinMode(Valve::valves[i].pin, OUTPUT);
  }
  pinMode(igniterPin, OUTPUT);
  digitalWrite(igniterPin, 0);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);

  MIDI.begin(MIDI_CHANNEL_OMNI);
#if defined(__AVR_ATmega32U4__) && defined(CORE_TEENSY)
 pinMode(CORE_RXD1_PIN, INPUT_PULLUP);
#endif
 
  strip.begin();
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0x00ff00);
  }
  strip.show();

  Serial.begin(9600);
  Serial.println("Hello MIDI effect (with LEDs)");
}

void loop()
{
  MIDI.read();
  Valve::checkTimeouts();
  

  // Manual mode
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c >= '1' && c <= '4') {
      int effect = c - '1';
      Valve::valves[effect].timeout = millis() + 300;
      Valve::valves[effect].on();
      digitalWrite(igniterPin, 1);
      lastEffect = millis();
    }
  }
}
