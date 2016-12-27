

//#include "Timer.h"
// CONFIGURACOES GERAIS
#define NBUTTONS 2
#define DMX_PIN 12
#define NCHANNELS 140

// remover
#define NPALETAS 8
#define NCENAS 5

// variavel pra guardar o estado do ultimo botao
bool debug = false;
bool piscante;

float botao8;

// nao sei se fica
unsigned int divideTempo = 1;


float dmx[NCHANNELS];
float dmxEasy[NCHANNELS];

unsigned int cena;
float easing = 10.0;
unsigned int paleta = 0;

float fade = 1.0;
unsigned int pan = 127;
unsigned int tilt = 127;

int conta = 0;


// INTERFACE
bool botoes[NBUTTONS];
bool buttonPressed[NBUTTONS];
int potVal[8];

// bpm tap
float bpm = 120.0;
float tempo = 60000.0 / bpm;
unsigned int nextEvent;
float lastTempo = 0;


int minX = 100;
int maxX = 0;
int minY = 100;
int maxY = 0;


#include "Classes.h"
#include "Definitions.h"

// usada somente uma vez no setup
void fixturesIndex() {
	int i = 0;
	for (auto & f : fixtures) {
		//Serial.println("canal :: " + String(f.channel) + " -- x:" + String(f.x) + " -- y:" + String(f.y));
		minX = min(minX, f.x);
		maxX = max(maxX, f.x);
		minY = min(minY, f.y);
		maxY = max(maxY, f.y);

		f.qual = i / (float) 28; //(float)fixtures.length;
		i++;
	}

	// Serial.println("minX :: " + String(minX));
	// Serial.println("maxX :: " + String(maxX));
	// Serial.println("minY :: " + String(minY));
	// Serial.println("maxY :: " + String(maxY));

	for (auto & f : fixtures) {
		f.xx = map(f.x, minX, maxX, 0, 100)/100.0f;
		f.yy = map(f.y, minY, maxY, 0, 100)/100.0f;
	}
}

//
void debugFixtures() {
	for (auto & f : fixtures) {
		String line = String(f.channel) + "\t\t" + String(f.xx) + "\t" + String(f.yy);
		//" + String(f.x) + ":" + String(f.y) + " --- 
		Serial.println(line);
	}
}


void setupBardapraia() {
	Serial.begin(9600);
	delay(800);

	fixturesIndex();

	debugFixtures();

	for (int a = 0; a < NBUTTONS; a++ ) {
		pinMode(a, INPUT_PULLUP);
	}

	DmxSimple.usePin(DMX_PIN);
	DmxSimple.maxChannel(NCHANNELS);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
}




void allWhite() {
	for (int a=0; a<73; a++) {
		DmxSimple.write(a, 255);
	}
	Serial.println("allWhite");
}

void setColor(fixture & f, color c) {
	if (f.modelo != MOVINGHEAD) {
		int r = c.r * fade;
		int g = c.g * fade;
		int b = c.b * fade;
		dmx[f.canalRgb+0] = r;
		dmx[f.canalRgb+1] = g;
		dmx[f.canalRgb+2] = b;
		DmxSimple.write(f.canalRgb+0, r);
		DmxSimple.write(f.canalRgb+1, g);
		DmxSimple.write(f.canalRgb+2, b);
	}

}

void setColorEasy(fixture & f, color & c) {
	if (f.modelo != MOVINGHEAD) {
		dmx[f.canalRgb] = c.r * fade;
		dmx[f.canalRgb] = c.g * fade;
		dmx[f.canalRgb] = c.b * fade;
	}
}

void playColor(fixture & f, int i = -1) {
	if (i == -1) {
		i = f.rand % 3; //maximo numero de cores
	}
//	int indiceCor = paletas[paleta][f.rand];
	int indiceCor = paletas[paleta][i];
	setColorEasy(f, colors[indiceCor]);
}


void movingsColor(bool black = false) {
	int indiceCor = paletas[paleta][0];
	color c = colors[indiceCor];
	if (black) {
		c = color(0,0,0);
	}

	float r = c.r * botao8;
	float g = c.g * botao8;
	float b = c.b * botao8;

	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			dmx[f.canalRgb + 0] = r;
			dmx[f.canalRgb + 1] = g;
			dmx[f.canalRgb + 2] = b;
		}
	}
}

// acho q nao precisa dessa funcao. é alguma coisa estranha no moving
void resetMoving() {
	if (debug) {
		Serial.println("resetMoving :: ");
	}
	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			for (int c=6; c<=7; c++) {
				dmx[f.channel + c] = 0;
				dmxEasy[f.channel + c] = 0;
				DmxSimple.write(f.channel + c, 0);
			}
		}
	}		
}

void pisca() {

	//resetMoving();
	//Serial.println("Pisca! ---- " + conta % 2 ? "sim" : "nao");
	conta ++;
	digitalWrite(LED_BUILTIN, conta % 2 ? LOW : HIGH);

	// random no beat
	if (cena == 0) {
		for (auto & f : fixtures) {
			if (random(0, 6) < 1) {
				playColor(f);
			} else {
				setColorEasy(f, black);
			}
		}
	}

	// cores solidas rolando full
	else if (cena == 1) {
		for (auto & f : fixtures) {
			if (conta % 2 == 0) {
				playColor(f, (conta/2)%3);
			} else {
				setColorEasy(f, black);
			}
		}
	}

	// cores solidas rolando full somente nas linhas
	else if (cena == 2) {
		for (auto & f : fixtures) {
			if (f.modelo == LINHA) {
				if (conta % 2 == 0) {
					playColor(f, (conta/2)%3);
				} else {
					setColorEasy(f, black);
				}
			} else {
				setColorEasy(f,black);
			}
		}
	}

	else if (cena == 3) {
		int resto = conta % 4;
		for (auto & f : fixtures) {
			switch (resto) {
				case 0:
					if (f.modelo == LINHA) {
						playColor(f, 0);
					}
					break;
				case 1:
					setColorEasy(f, black);
					break;
				case 2:
					if (f.modelo == LED) {
						playColor(f, 2);
					}
					break;
				case 3:
					setColorEasy(f, black);
					break;
			}
		}
	}

	/* fazer uma cena aqui piscando ora linha ora 
	*/


	// ROLL X
	else if (cena == 4) {
		for (auto & f : fixtures) {
			if (conta % maxX == f.x) {
//				playColor(f, f.xx * 3);
				playColor(f, 0);
			} else {
				setColorEasy(f, black);
			}
		}
	}

	else if (cena == 5) {
		for (auto & f : fixtures) {
			if (conta % maxY == f.y) {
//				playColor(f, f.yy * 3);
				playColor(f, 0);

			} else {
				setColorEasy(f, black);
			}
		}
	}

	// movings pisca
	movingsColor(conta % 2);

}

float somaTempo = 0;
int tapCount = 0;

void bpmTap() {
	tempo = millis() - lastTempo;
	lastTempo = millis();
	if (tempo > 0 && tempo < 1000) {
		if (debug) {
			Serial.println("tap :: " + String(tapCount));
		}
		somaTempo += tempo;
		tapCount ++;
	} else {
		tapCount = 0;
		somaTempo = 0;
	}

	int nTap = 3;
	if (tapCount == nTap) {
		if (debug) {
			Serial.println("resetTempo");
		}
		tempo = somaTempo / (float) nTap;

	    nextEvent = millis() + tempo;

		// temporizador.stop(evento);
		// evento = temporizador.every(tempo / divideTempo, pisca);

		tapCount = 0;
		somaTempo = 0;
	}
	if (debug) {
		Serial.println("BpmTap :: " + String(tempo));
	}
}

void buttonDown(int a) {
	buttonPressed[a] = true;
	if (a == 1) {
		bpmTap();
	}
	if (debug) {
		Serial.println("button down ::: " + String(a));
	}
}

void buttonUp(int a) {
	buttonPressed[a] = false;
	if (debug) {
		Serial.println("button up ::: " + String(a));
	}
}

void readButtons() {
	// handling buttons
	for (int a = 0; a < NBUTTONS; a++) {
		if (botoes[a] != digitalRead(a)) {
			if (digitalRead(a) == LOW) {
				buttonDown(a);
			}
			if (digitalRead(a) == HIGH) {
				buttonUp(a);
			}
			botoes[a] = digitalRead(a);
		}
	}
}

void sendDmxEasy() {

// so faz o easing com todas.
	for (int a=0; a<140; a++) {
      if (easing > 0) {
        dmxEasy[a] += (dmx[a] - dmxEasy[a]) / easing;
        //DmxSimple.write(a + 1, byte(dmxEasy[a]));
      } else {
        dmxEasy[a] = dmx[a];
        //DmxSimple.write(a + 1, byte(dmx[a]));
      }
	}
	// deixo os movings de fora
    for (int a = 0; a < 73; a++) {
//    for (int a = 0; a < NCHANNELS; a++) {
      if (easing > 0) {
        //dmxEasy[a] += (dmx[a] - dmxEasy[a]) / easing;
        DmxSimple.write(a, byte(dmxEasy[a]));
      } else {
        //dmxEasy[a] = dmx[a];
        DmxSimple.write(a, byte(dmx[a]));
      }
    }
}




void setMovingPan(int val) {
	if (debug) {
		Serial.println("setMovingPan :: " + String(val));
	}
	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			int canal = f.channel + 0;
			dmx[canal] = val;
			DmxSimple.write(canal, val);
		}
	}
}

void setMovingTilt(int val) {
	if (debug) {
		Serial.println("setMovingTilt :: " + String(val));
	}

	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			int canal = f.channel + 1;
			dmx[canal] = val;
			DmxSimple.write(canal, val);
		}
	}
}

void setMovingDimmer(int val) {
	if (debug) {
		Serial.println("setMovingDimmer :: " + String(val));
	}

	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			// pan, tilt
			// xxx ? strobo?
			// r g b
			// 6 ? 
			// 7 ?
			int canal = f.channel + 2;
			dmx[canal] = val;
			dmxEasy[canal] = val;
			DmxSimple.write(canal, val);

			//DmxSimple.write(f.channel + 2, 255);
		}
	}
}


void readPots() {
	int tolerancia = 3;

	// checa se o valor dos pots mudou
	for (int a = 0; a < 8; a++) {
		int indexPot = 14 + a;
		if (abs(potVal[a] - analogRead(indexPot)) > tolerancia) {
			//if (potVal[a] != analogRead(indexPot)) {
			if (debug) {
				Serial.println("Change Value Pot" + String(a) + " - Pin:" + String(indexPot) + " ::: " + String(analogRead(indexPot)));
			}
			potVal[a] = analogRead(indexPot);

			if (indexPot == 17) { //primeiro pot
				cena = map(analogRead(indexPot), 5, 1023, 0, NCENAS);
				divideTempo = map(analogRead(indexPot), 5, 1023, 0, NCENAS * 4) % 4;
				if (debug) 
				{
					Serial.println("cena :: " + String(cena));
				}
			}

			else if (indexPot == 16) { //segundo pot
				easing = map(analogRead(indexPot), 5, 1023, 0, 30);
				if (debug) {
					Serial.println(easing);
				}
			}
			else if (indexPot == 15) { //terceiro pot
				paleta = map(analogRead(indexPot), 5, 1023, 0, NPALETAS);
				if (debug) {
					Serial.println("paleta :: " + String(paleta));
				}
			}

			else if (indexPot == 14) { //quarto pot
//				bpm = map(analogRead(indexPot), 5, 1023, 60, 180);
				bpm = map(analogRead(indexPot), 5, 1023, 100, 140);
//				bpm = map(analogRead(indexPot), 5, 1023, 20, 280);
				tempo = 60000 / bpm;
				//temporizador.stop(evento);
				//evento = temporizador.every(tempo, pisca);
				if (debug) {
					Serial.println("bpm :: " + String(bpm));
				}
			}

			else if (indexPot == 21) { //primeiro pot de baixo - fade
				fade =	map(analogRead(indexPot), 5, 1023, 0, 100) / 100.0;
				if (debug) {
					Serial.println("fade :: " + String(fade));
				}
			}

			else if (indexPot == 20) { //segundo pot de baixo - pan
				int val =	map(analogRead(indexPot), 5, 1023, 0, 255);
				setMovingPan(val);
				//setMovingDimmer(255);
			}
			else if (indexPot == 19) { //terceiro pot de baixo - tilt
				int val =	map(analogRead(indexPot), 5, 1023, 0, 255);
				setMovingTilt(val);
				//setMovingDimmer(255);
			}
			else if (indexPot == 18) { //quarto pot de baixo

				botao8 = map(analogRead(indexPot), 5, 1023, 0, 255);
				setMovingDimmer(botao8);
				// talvez fazer cena de moving head
				// fade =	map(analogRead(indexPot), 5, 1023, 0, 100) / 100.0;
				// Serial.println(fade);
			}// continua 20, 19, 18
		}
	}
}


void strobo() {
	// if (debug) {
	// 	Serial.println("strobo");
	// }
	piscante = !piscante;
	for (int a=0; a<73; a++) {
		DmxSimple.write(a, piscante ? 255 : 0 );
	}
	// for (auto & f : fixtures) {
	// 	setColor(f, piscante ?  color(255,255,255) : color(0,0,0) );
	// }
}

void chuvisco() {
	if (debug) {
		Serial.println("chuvisco");
	}
	for (auto & f : fixtures) {
		color c = random(0,2) > 1 ? color(0,0,0) : color(random(0,255), random(0,255), random(0,255));
		setColor(f, c);
	}
}

void blackout() {
	//evita um branco solido la no inicio
	for (int a = 0; a < NCHANNELS; a++) {
		DmxSimple.write(a , 0);
	}
}

void movingEasy() {
	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			int canal = f.channel + 3;
			for (int a = 0; a <  3; a++) {
				int c = canal + a;
				DmxSimple.write(c, dmxEasy[c]);
			}
		}
	}
}

