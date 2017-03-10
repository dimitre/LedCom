

//#include "Timer.h"
// CONFIGURACOES GERAIS
#include <Bounce2.h>
#include <DmxSimple.h>

#define NBUTTONS 16
#define DMX_PIN 11
#define POT_PIN 13
#define SOBEDESCE_PIN 31
#define TODOS_PIN 33
#define NCHANNELS 280

bool buttonStateEsquerda = true;
bool buttonStateDireita = true;

//double dmx[NCHANNELS];

// INTERFACE
//bool botoes[NBUTTONS];
//bool buttonPressed[NBUTTONS];
//int potVal[8];
bool debug = true;
// precisa?
//#include <Bounce2.h>

#include "Classes.h"
#include "Definitions.h"

// Bounce buttons[] = { 
// 	Bounce(15, 8), 
// 	Bounce(16, 8), 
// 	Bounce(17, 8)
// };

// uint8_t pots[] = { 13 };
// uint8_t buttonsIndex[] = { 15, 16, 17 };

uint16_t prevPot;

Bounce debounceEsq;
Bounce debounceDir;

// void setDmx(int c, int v) {
// 	dmx[c] = v;
// 	DmxSimple.write(c, byte(v));
// }

void panic() {
	Serial.println("PANIC!");
	for (int a=0; a<512; a++) {
		DmxSimple.write(a,0);
	}
}

void unlock() {
	Serial.println("---->>> UNLOCKing all fixtures");
	for (auto & f : fixtures) {
		f.setDmx(0, 82);
		f.setDmx(1, 182);
	}
}

//void sobeAll() {
void moveAll() {
	for (auto & f : fixtures) {
		if (f.channel > 0) {
			f.move();
		}
	}
}

// void desceAll() {
// 	Serial.println("Pointer to Function - DESCE ALL");
// 	for (auto & f : fixtures) {
// 		f.move();
// 	}
// }

void paraAll() {
	Serial.println("Pointer to Function - PARA ALL");
	for (auto & f : fixtures) {
		f.para();
	}
}


void setupLedcom() {
	Serial.begin(115200);
	delay(800);



	
	pinMode(SOBEDESCE_PIN, INPUT_PULLUP);

	for (int a = 0; a < NBUTTONS; a++ ) {
		// não é bem assim. 
		pinMode(a, INPUT_PULLUP);
	}

	DmxSimple.usePin(DMX_PIN);
	DmxSimple.maxChannel(NCHANNELS);
	//pinMode(LED_BUILTIN, OUTPUT);
	//digitalWrite(LED_BUILTIN, HIGH);

	debounceEsq.attach(33, 8);
	debounceDir.attach(22, 8);


	for (auto & f : fixtures) {
		pinMode(f.port, INPUT_PULLUP);
	}
	Serial.println("SETUP LEDCOM");


	for (auto & f : fixtures) {
 		f.mover = &moveAll;
		f.parar = &paraAll;
		f.panic = &panic;
		f.ready = true;
	}


	// for (auto & b : buttons) {
	// 	b.mover = &moveAll;
	// 	b.parar = &paraAll;
	// }
	// buttons[0].subir = &sobeAll;
	// buttons[0].descer = &sobeAll;
	// buttons[0].parar = &sobeAll;

		unlock();

}


void readPots() {
	if (abs(prevPot - analogRead(POT_PIN)) > 3) { //tolerancia 3
		int vel = map(analogRead(POT_PIN), 0, 1023, 0, 255);
		if (debug) {
			Serial.print("velocidade :: ");
			Serial.println(vel);
		}
		for (auto & f : fixtures) {
			f.setDmx(3, vel);
		}
		prevPot = analogRead(POT_PIN);
	}
}




void updateLedcom() {
	readPots();

	if (debounceEsq.fallingEdge()) {
		moveAll();
	}
	if (debounceEsq.risingEdge()) {
		paraAll();
	}

	if (debounceDir.fallingEdge() || debounceDir.risingEdge()) {
		panic();
	}

	// for (auto & b : buttons) {
	// 	b.readButton();
	// }

	for (auto & f : fixtures) {
		f.readButton();
	}
}