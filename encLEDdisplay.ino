#include <digitalWriteFast.h>
#define CLK 8
#define SERIN 9
#define LATCH 10
#define ENABLE 11
#define encoderPhaseA 2
#define encoderPhaseB 3
#define encoderInterruptA 0

volatile int encoderTicks = 0;
long distance = 0;
volatile int mills = 0;
volatile int meters = 0;
int decimal = 0;
int third = 0;
int second = 0;
int first = 0;

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
}; // digits array

void setup() {

	pinMode(CLK, OUTPUT);
	pinMode(SERIN, OUTPUT);
	pinMode(LATCH, OUTPUT);
	pinMode(ENABLE, OUTPUT);
	pinMode(encoderPhaseA, INPUT_PULLUP); //input pin with internal pull-up resistor
	pinMode(encoderPhaseB, INPUT_PULLUP); //input pin with internal pull-up resistor
	
	attachInterrupt(encoderInterruptA, encoderISR, FALLING);
	shiftOut(SERIN, CLK, LSBFIRST, dig[0]);
	shiftOut(SERIN, CLK, LSBFIRST, dig[0] + 0x1);
	shiftOut(SERIN, CLK, LSBFIRST, dig[0]);
	shiftOut(SERIN, CLK, LSBFIRST, dig[0]);
}

void loop() {
	
	distance = (meters + mills * 0.001) * 10;

	splitDigits(abs(distance));
	displayDigits();
}

void encoderISR(){
	if (digitalReadFast(encoderPhaseB)){
		encoderTicks++;
	} else {
		encoderTicks--;
	}
	if (encoderTicks == 4){
		mills += 3;
		encoderTicks = 0;
	}else if (encoderTicks == -4){
		mills -= 3;
		encoderTicks = 0;
	}
	if(mills >= 1000){
		meters++;
		mills = 0;
	}
}

void splitDigits(int distance){

	decimal = distance % 10;
	shiftOut(SERIN, CLK, LSBFIRST, dig[decimal]);   
	third = (distance / 10) % 10;
	shiftOut(SERIN, CLK, LSBFIRST, dig[third] + 0x1);
	second = (distance  / 100) % 10;
	shiftOut(SERIN, CLK, LSBFIRST, dig[second]);
	first = (distance / 1000) % 10;
	shiftOut(SERIN, CLK, LSBFIRST, dig[first]);

}

void displayDigits(){

	digitalWrite(LATCH, HIGH);
	digitalWrite(LATCH, LOW);	
	
}

