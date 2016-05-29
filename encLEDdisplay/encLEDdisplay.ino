#include <digitalWriteFast.h>
#define CLK 8
#define SERIN 9
#define LATCH 10
#define ENABLE 11
#define encoderPhaseA 2
#define encoderPhaseB 3
#define encoderInterruptA 0
//  400ppr encoder with 300mm length wheel so 4 ticks == 3 mm -> 3 / 4 = 0.75
//  dividing by 100 to get decimeters or by 1000 to get meters
#define TICKS_PER_DECIMETER 0.75 / 100

volatile long encoderTicks = 0;
int distance;
int decimal;
int third;
int second;
int first;

//  digits array
byte dig[10] = {0xFC, // 0
                0x60, // 1
                0xDA, // 2
                0xF2, // 3
                0x66, // 4
                0xB6, // 5
                0xBE, // 6
                0xE0,  // 7
                0xFE, // 8
                0xF6 // 9
               };

void setup() {

  pinMode(CLK, OUTPUT);
  pinMode(SERIN, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  pinMode(encoderPhaseA, INPUT_PULLUP); //input pin with internal pull-up resistor
  pinMode(encoderPhaseB, INPUT_PULLUP); //input pin with internal pull-up resistor

  attachInterrupt(encoderInterruptA, encoderISR, FALLING);
  shiftOut(SERIN, CLK, LSBFIRST, dig[0]);
  shiftOut(SERIN, CLK, LSBFIRST, dig[0] + 0x1); //0x1 is dot segment address
  shiftOut(SERIN, CLK, LSBFIRST, dig[0]);
  shiftOut(SERIN, CLK, LSBFIRST, dig[0]);
  
}

void loop() {
  
  distance = encoderTicks * TICKS_PER_DECIMETER;
  displayDigits(abs(distance));
  delay(100);
  
}

void encoderISR() {
  
  if (digitalReadFast(encoderPhaseB)) {
    encoderTicks++;
  } else {
    encoderTicks--;
  }
  
}

void displayDigits(int distance) {

  decimal = distance % 10;
  shiftOut(SERIN, CLK, LSBFIRST, dig[decimal]);
  third = (distance / 10) % 10;
  shiftOut(SERIN, CLK, LSBFIRST, dig[third] + 0x1);
  second = (distance  / 100) % 10;
  shiftOut(SERIN, CLK, LSBFIRST, dig[second]);
  first = (distance / 1000) % 10;
  shiftOut(SERIN, CLK, LSBFIRST, dig[first]);

  digitalWrite(LATCH, HIGH);
  digitalWrite(LATCH, LOW);

}
