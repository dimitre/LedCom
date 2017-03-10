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
bool debug = true;

#include "Classes.h"
#include "Definitions.h"

uint16_t prevPot;

Bounce debounceEsq, debounceDir;

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

void moveAll() {
	bool sobe = digitalRead(SOBEDESCE_PIN) == LOW;
	Serial.println("MoveAll :: " + sobe ? "SUBINDO" : "DESCENDO");

	for (auto & f : fixtures) {

		// checa pois tem fixtures que é s´ø o botao
		if (f.channel > 0) {

			if (sobe) {
				// 	DmxSimple.write(c + f.channel, byte(v));

				f.setDmx(4, 200);
			} else {
				f.setDmx(4, 0);
			}
			// aciona movimento.
			f.setDmx(5, 200);
		}
	}		
}

void paraAll() {
	Serial.println("PARA ALL");
	for (auto & f : fixtures) {
		f.setDmx(5, 0);
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

	// debounceEsq.attach(33, 8);
	// debounceDir.attach(22, 8);


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

	unlock();
}

void readPots() {
	if (abs(prevPot - analogRead(POT_PIN)) > 4) { //tolerancia 3
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
	// debounceEsq.update();
	// debounceDir.update();
	// if (debounceEsq.fallingEdge()) {
	// 	Serial.println("esq");
	// 	moveAll();
	// }
	// if (debounceEsq.risingEdge()) {
	// 	Serial.println("esq");
	// 	paraAll();
	// }
	// if (debounceDir.fallingEdge() || debounceDir.risingEdge()) {
	// 	Serial.println("dir");
	// 	panic();
	// }

	for (auto & f : fixtures) {
		f.readButton();
	}
}

