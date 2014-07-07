#include <MIDI.h>

int valvePins[4] = {11,12,13,14};

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

int (*mappingfunc)(byte) = octaveToEffect;

void handleNoteOn(byte channel, byte pitch, byte velocity) {

  int effect = mappingfunc(pitch);
  if (effect >= 0) {
    Serial.print("Effect On: ");
    Serial.println(effect, DEC);

    int duration = map(velocity, 0, 127, 0, 1000);
    Valve::valves[effect].timeout = millis() + duration;
    Valve::valves[effect].on();
  }
  else {
    Serial.print("NoteOn: ");
    Serial.println(pitch, DEC);
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  Valve::valves[mappingfunc(pitch)].off();
}

void setup()
{
  for (int i=0;i<4;i++) {
    Valve::valves[i].pin = valvePins[i];
    pinMode(Valve::valves[i].pin, OUTPUT);
  }

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);

  MIDI.begin(MIDI_CHANNEL_OMNI);

  Serial.begin(9600);
  Serial.println("Hello MIDI effect");
}

void loop()
{
  MIDI.read();
  Valve::checkTimeouts();
}
