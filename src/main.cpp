#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

// Smooth buttons and potentiometers, see the libraries for examples on how to use them.
#include <Bounce2.h>
#include <ResponsiveAnalogRead.h>

// USB MIDI object
Adafruit_USBD_MIDI usbMidi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMidi, MIDI);

// analog read
const int ANALOG_PIN = 3;
ResponsiveAnalogRead analog(ANALOG_PIN, true);

// value last
int lastMappedValue = 0;

void handle_midi_cc(byte channel, byte note, byte velocity)
{
  Serial.printf("Got cc! %d %d %d \n", channel, note, velocity);
  digitalWrite(LED_BUILTIN, 1);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("MIDI pedal powering up");

  TinyUSBDevice.setManufacturerDescriptor("il ponz");
  TinyUSBDevice.setProductDescriptor("il ponz esp32s2 midi pedal");

  usbMidi.begin();
  MIDI.begin();

  // Turn midi thru off
  MIDI.turnThruOff();
  // MIDI.turnThruOn();

  // Set the MIDI note on handling function
  // MIDI.setHandleNoteOn(handle_midi_note_on);
  // MIDI.setHandleControlChange(handle_midi_cc);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);

  analog.setAnalogResolution(8192);

  delay(500);
  digitalWrite(LED_BUILTIN, 0);
  delay(500);
  digitalWrite(LED_BUILTIN, 1);
  delay(500);
  digitalWrite(LED_BUILTIN, 0);

  Serial.println("MIDI pedal setup ok");
}

// auto pastLEDState = true;
void loop()
{
  MIDI.read();
  
  // update the ResponsiveAnalogRead object every loop
  analog.update();
  int rawValue = analog.getRawValue();

  // Map the analog value to a 0-127 range for cc
  int mappedValue = map(rawValue, 0, 8192, 0, 127);
  int analogValue = analog.getValue();

  // if the responsive value has change, print out 'changed'
  if (analog.hasChanged() && mappedValue != lastMappedValue)
  {
    Serial.printf("raw %d \t analog %d \t mapped %d \n", rawValue, analogValue, mappedValue);
    // send midi cc
    MIDI.sendControlChange(11, mappedValue, 1);
    lastMappedValue = mappedValue;
    digitalWrite(LED_BUILTIN, 1);
    delay(10);
  }

  digitalWrite(LED_BUILTIN, 0);
}
