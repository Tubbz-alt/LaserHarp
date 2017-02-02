/*

   File:     MP3_Shield_RealtimeMIDI.ino
   Author:   Michael Leung
             Adapted from Nickolas Tezak and Matthias Neeracher

   This code is in the public domain, with the exception of the contents of sVS1053b_Realtime_MIDI_Plugin.

   The code is based on Nathan Seidle's Sparkfun Electronics example code for the Sparkfun
   MP3 Player and Music Instrument shields and and VS1053 breakout board.

   http://www.sparkfun.com/Code/MIDI_Example.pde
   http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Dev/Arduino/Shields/VS_Shield_Example.zip

        Spark Fun Electronics 2011
        Nathan Seidle

        This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

   THEORY OF OPERATIONS

   The VS1053b has two ways of playing MIDI: One method is that you simply send a Standard MIDI level 0 file through
   SPI, and the chip will play it. This works exactly the same way as MP3 mode and will not be discussed further here.
   The other method is that the VS1053b has a "Real Time MIDI mode", in which it will instantly execute MIDI commands
   sent to it through either the UART or SPI.

   Real Time MIDI mode can be enabled with two different methods, controlled by USE_GPIO_INIT
    (1) Setting GPIO1 to HIGH (which is hardwired in the Sparkfun Music Instrument shield, and can be done through
        pin 4 in the MP3 Player Shield)
    (0) Sending a small software patch through SPI.

   MIDI data can be sent with two different methods as well, controlled by USE_SPI_MIDI
    (0) Through a (software) serial connection on pin 3, at 31250 baud
    (1) Through SPI, at an arbitrary data rate. For SPI, each byte of MIDI data needs to be prefixed by a 0 byte
        (The V1053b data sheet erroneously states that the padding should be a 0xFF byte).

   Both initialization methods and both transmission methods can be selected through the #defines below. Out of the box,
   it probably makes most sense to enable real time MIDI through pin 4, and send serial data through pin 3, but if you
   want to cut the traces for pin 3 and 4 and use those pins for another purpose, the alternative methods may come in
   handy.
*/


#define USE_GPIO_INIT  0
#define USE_SPI_MIDI   1

#define THRESHSAM 50 // Used to determine threshold for strumming. Increase if light source has high variability.

#define NUMNOTES 8 // Define number of notes.

#define VERBOSE_DEBUG true // Use true to output to screen (over serial) for debugging

#define USE_PATCH_INIT  !USE_GPIO_INIT
#define USE_SERIAL_MIDI !USE_SPI_INIT
#define USE_SPI         (USE_SPI_MIDI||USE_PATCH_INIT)

#if USE_SPI
#include <SPI.h>
#endif
#if USE_SERIAL_MIDI
#include <SoftwareSerial.h>
SoftwareSerial midiSerial(2, 3); // Soft TX on 3, RX not used (2 is an input anyway, for VS_DREQ)
#endif


// NOTE: We're using this MP3-MIDI Converter available at http://www.gravitech.us/mp3pladforar.html
// The breakout board contains the VS1053B chip. (Abbreviated VS for the remainder of code)
// Pinout for MP3 Converter:
// D2: MP3-DREQ
// D3: MIDI-RX (optional via jumper)
// D4: Micro-SD chip select
// D6: MP3 chip select
// D7: MP3-DCS
// D8: MP3-RST
// D11: SPI MOSI
// D12: SPI MISO
// D13: SPI SCK

#if USE_SPI
#define VS_XCS    6 // Control Chip Select Pin (for accessing SPI Control/Status registers)
#define VS_XDCS   7 // Data Chip Select / BSYNC Pin
#define VS_DREQ   2 // Data Request Pin: Player asks for more data
#endif
#if USE_GPIO_INIT
#define VS_GPIO1  4  // Mode selection (0 = file / 1 = real time MIDI)
#endif

#define VS_RESET  8 //Reset is active low

// import instrument definitions
#include "vs1053midi.h"


#define INSTRUMENT instr_orchestral_harp
//#define INSTRUMENT instr_marimba
//#define INSTRUMENT instr_guitar_harmonics
//#define INSTRUMENT instr_harpsichord
//#define INSTRUMENT instr_acoustic_grand_piano

unsigned short offset = 60;
unsigned short on_vel = 100;
unsigned short off_vel = 10;
unsigned short volume = 127; // Max volume is 127

const unsigned short notes[NUMNOTES] = {0, 2, 4, 5, 7, 9, 11, 12};

int counter = 0;
#define THRESHMIN 15

// Data used by MIDI
boolean chord_plucked[NUMNOTES] = {false, false, false, false, false, false, false, false};
boolean status_changed[NUMNOTES] = {false, false, false, false, false, false, false, false};

// Structure for holding sensor data
struct lightSensor {
  int curSample;
  int prevSample;
  int pinNum;
  int thresh;
};

// Array of Structure, for convenient indexing of sensors
lightSensor sensorArray[8];
int curSampleTemp;

// Determines the threshold for each sensor
void setupThresh() {
  // Buffer for storing a sequence of sensor measurements. It will be used to set threshold for each sensor at setup.
  //delay(10); // Increase this  (1000 = 1 second) if lasers turn on at the same time as arduino to give them time to stabilize.
  float threshMeas[THRESHSAM];
  float totalVal, mean, var, std;
  for (int note = 0; note < NUMNOTES; note++) {

    totalVal = 0;
    for (int nn = 0; nn < THRESHSAM; nn++) {
      threshMeas[nn] = (float) analogRead(sensorArray[note].pinNum);
      totalVal += threshMeas[nn];
      delay(1);
    }
    mean = totalVal / THRESHSAM;
    var = 0;
    for (int r = 0; r < THRESHSAM; r++) {
      var += pow(threshMeas[r] - mean, 2);
    }
    var = var / THRESHSAM;
    std = sqrt(var);

    sensorArray[note].thresh = max((int) (6 * std), THRESHMIN);

#if VERBOSE_DEBUG
    Serial.print(mean);
    Serial.print(" ");
    Serial.print(var);
    Serial.print(" ");
    Serial.print(std);
    Serial.print(" DONE \n");
#endif
  }
}


// Iterate through all sensors to determine if any of the sensors have been triggered.
// It uses a very rudimentary scheme for detecting laser blocking, by comparing it to the last measurement.
// NOTE: This scheme could be broken if someone was to very slowly move their hand over the detector.
//
// The value of chord_plucked[kk] should always reflect the current sensor readout, true for blocked laser
// the value of status_changed should only be updated to true if the current sensor readout and the last sensor are different,
// i.e., when the value of chord_plucked[kk] is actually being changed.
void readSensors() {
  for (int note = 0; note < NUMNOTES; note++) {
    curSampleTemp = analogRead(sensorArray[note].pinNum);
    if (curSampleTemp > sensorArray[note].prevSample + sensorArray[note].thresh) {
      chord_plucked[note] = true;
      status_changed[note] = true;
    }
    else if (curSampleTemp < sensorArray[note].prevSample - sensorArray[note].thresh) {
      chord_plucked[note] = false;
      status_changed[note] = true;
    }
    sensorArray[note].prevSample = sensorArray[note].curSample;
    sensorArray[note].curSample = curSampleTemp;
  }
  delay(10);
}


// Set the instrument of the first channel.
// We are currently using only one MIDI channel.
// We could easily change this.
void setupMidi() {
  int kk = 0;
  //  for(kk = 0; kk < 8; kk=kk+2){
  talkMIDI(0xB0 | kk, 0x00, 0x79); // Default bank GM1
  talkMIDI(0xB0 | kk, 0x07, volume); //0xB0 is channel message, set channel volume to near max (127)
  talkMIDI(0xC0 | kk, INSTRUMENT, 0); //Set instrument number. 0xC0 is a 1 data byte command
  //  }
}


void playChords() {
  // play sounds
  for (int note = 0; note < NUMNOTES; note++) {
    if (status_changed[note]) {
      if (chord_plucked[note]) {
        noteOn(0, notes[note] + offset, on_vel);
#if VERBOSE_DEBUG
        Serial.print("Playing Note:");
        Serial.println(note, DEC);
#endif
      } else {
        noteOff(0, notes[note] + offset, off_vel);
#if VERBOSE_DEBUG
        Serial.print("Stopping Note:");
        Serial.println(note, DEC);
#endif
      }
      status_changed[note] = false;
    }
  }
}


#if USE_PATCH_INIT
//Write to VS10xx register
//SCI: Data transfers are always 16bit. When a new SCI operation comes in
//DREQ goes low. We then have to wait for DREQ to go high again.
//XCS should be low for the full duration of operation.
void VSWriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte) {
  while (!digitalRead(VS_DREQ)) ; //Wait for DREQ to go high indicating IC is available
  digitalWrite(VS_XCS, LOW); //Select control

  //SCI consists of instruction byte, address byte, and 16-bit data word.
  SPI.transfer(0x02); //Write instruction
  SPI.transfer(addressbyte);
  SPI.transfer(highbyte);
  SPI.transfer(lowbyte);
  while (!digitalRead(VS_DREQ)) ; //Wait for DREQ to go high indicating command is complete
  digitalWrite(VS_XCS, HIGH); //Deselect Control
}


// Plugin to put VS10XX into realtime MIDI mode
// Originally from http://www.vlsi.fi/fileadmin/software/VS10XX/vs1053b-rtmidistart.zip
// Permission to reproduce here granted by VLSI solution.
//
const unsigned short sVS1053b_Realtime_MIDI_Plugin[28] = { /* Compressed plugin */
  0x0007, 0x0001, 0x8050, 0x0006, 0x0014, 0x0030, 0x0715, 0xb080, /*    0 */
  0x3400, 0x0007, 0x9255, 0x3d00, 0x0024, 0x0030, 0x0295, 0x6890, /*    8 */
  0x3400, 0x0030, 0x0495, 0x3d00, 0x0024, 0x2908, 0x4d40, 0x0030, /*   10 */
  0x0200, 0x000a, 0x0001, 0x0050,
};


void VSLoadUserCode(void) {
  int i = 0;
  while (i < sizeof(sVS1053b_Realtime_MIDI_Plugin) / sizeof(sVS1053b_Realtime_MIDI_Plugin[0])) {
    unsigned short addr, n, val;
    addr = sVS1053b_Realtime_MIDI_Plugin[i++];
    n = sVS1053b_Realtime_MIDI_Plugin[i++];
    while (n--) {
      val = sVS1053b_Realtime_MIDI_Plugin[i++];
      VSWriteRegister(addr, val >> 8, val & 0xFF);
    }
  }
}
#endif


// Setup code
// This code runs one upon initialization
void setup() {

#if VERBOSE_DEBUG
  Serial.begin(115200); //Use serial for debugging
  Serial.println("\n******\n");
  Serial.println("MP3 Shield for LaserHarp");
  Serial.println("Michael Leung, Stanford Optical Society");
  Serial.println("");
#endif

#if USE_SERIAL_MIDI
  midiSerial.begin(31250);
#endif

  //Initialize VS1053 chip
  pinMode(VS_RESET, OUTPUT);
  digitalWrite(VS_RESET, LOW); //Put VS1053 into hardware reset

#if USE_SPI
  pinMode(VS_DREQ, INPUT);
  pinMode(VS_XCS, OUTPUT);
  pinMode(VS_XDCS, OUTPUT);
  digitalWrite(VS_XCS, HIGH); //Deselect Control
  digitalWrite(VS_XDCS, HIGH); //Deselect Data
  //Setup SPI for VS1053
  pinMode(10, OUTPUT); //Pin 10 must be set as an output for the SPI communication to work
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  //From page 12 of datasheet, max SCI reads are CLKI/7. Input clock is 12.288MHz.
  //Internal clock multiplier is 1.0x after power up.
  //Therefore, max SPI speed is 1.75MHz. We will use 1MHz to be safe.
  SPI.setClockDivider(SPI_CLOCK_DIV16); //Set SPI bus speed to 1MHz (16MHz / 16 = 1MHz)
  SPI.transfer(0xFF); //Throw a dummy byte at the bus
#endif

  delay(1);
  digitalWrite(VS_RESET, HIGH); //Bring up VS1053

#if USE_PATCH_INIT
  VSLoadUserCode();
#else
  pinMode(VS_GPIO1, OUTPUT);
  digitalWrite(VS_GPIO1, HIGH);  // Enable real time MIDI mode
#endif

  sensorArray[0].pinNum = A0;
  sensorArray[1].pinNum = A1;
  sensorArray[2].pinNum = A2;
  sensorArray[3].pinNum = A3;
  sensorArray[4].pinNum = A4;
  sensorArray[5].pinNum = A5;
  sensorArray[6].pinNum = A6;
  sensorArray[7].pinNum = A7;
  setupMidi();
#if VERBOSE_DEBUG
  Serial.println("setupThresh");
#endif
  setupThresh();
#if VERBOSE_DEBUG
  Serial.println("Done!");
  for (int note = 0; note < NUMNOTES; note++) {
    Serial.print("Sensor threshold ");
    Serial.print(note);
    Serial.print(" ");
    Serial.println(sensorArray[note].thresh);
  }
#endif
}


void sendMIDI(byte data) {
#if USE_SPI_MIDI
  SPI.transfer(0);
  SPI.transfer(data);
#else
  midiSerial.write(data);
#endif
}


//Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
void talkMIDI(byte cmd, byte data1, byte data2) {
#if USE_SPI_MIDI
  // Wait for chip to be ready (Unlikely to be an issue with real time MIDI)
  while (!digitalRead(VS_DREQ))
  { } // wait, do nothing
  digitalWrite(VS_XDCS, LOW);
#endif
  sendMIDI(cmd);
  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if ( (cmd & 0xF0) <= 0xB0 || (cmd & 0xF0) >= 0xE0) {
    sendMIDI(data1);
    sendMIDI(data2);
  } else {
    sendMIDI(data1);
  }
#if USE_SPI_MIDI
  digitalWrite(VS_XDCS, HIGH);
#endif
}


// Send a MIDI note-on message.  Like pressing a piano key
// channel ranges from 0-15
void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);
}


// Send a MIDI note-off message.  Like releasing a piano key
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}


void loop() {
  readSensors();
  playChords();
  //debugVerbose();
}


void debugVerbose() {
  for (int note = 0; note < NUMNOTES; note++) {
    Serial.print("sensor ");
    Serial.print(note);
    Serial.print("output: ");
    Serial.print(sensorArray[note].curSample);
    Serial.print(" ");
    Serial.print(sensorArray[note].prevSample - sensorArray[note].curSample);
    //Serial.print(analogRead(sensorArray[0].pinNum));
    Serial.print(" ");
    Serial.println(chord_plucked[note]);
  }

  // testing code
  chord_plucked[0] = true;
  status_changed[0] = true;
  playChords();
  delay(300);

  for (int kk = 0; kk < 7; kk++) {
    chord_plucked[kk] = false;
    status_changed[kk] = true;

    chord_plucked[kk + 1] = true;
    status_changed[kk + 1] = true;

    playChords();
    delay(300);
  }
  chord_plucked[7] = false;
  status_changed[7] = true;
  playChords();
  delay(1000);

  offset++;
  if (offset >= 100) offset = 22;
  // testing code end
}


