
/*

Quarto knob de baixo podem ser os setores. laterais do bar da praia, meio do corredor, moving heads, DJ.
ao inves 

PAINEL é
3 2 1 0
7 6 5 4

cena, cor, easing, xxx
fader

fader
*/

//#include "Timer.h"

#define NBUTTONS 6
#define DMX_PIN 12
#define NCHANNELS 140

// remover
//#define NCOLORS 5
#define NPALETAS 5

bool debug = true;
bool piscante;

// com o tempo defasar este.
float dmx[NCHANNELS];
float dmxEasy[NCHANNELS];

int evento;

int pan = 127;
int tilt = 127;

// cena
int conta = 0;

// bpm tap
float bpm = 120.0;
float atraso = 60000.0 / bpm;
float tempo = 0;
float lastTempo = 0;

// colocar aqui a posicao x, y, z, categoria de onde fica no bar da praia, etc.

enum model {
	LED, 
	MOVINGHEAD, 
};

// YUMMY - comecar a usar
struct color {
public:
	float r;
	float g;
	float b;
	color (float r = 255, float g = 255, float b = 255) {}
	// color (float gray = 255) {
	// 	r = g = b = gray;
	// }
};

color black = color(0,0,0);

color colors[] = { 
	color(255, 255, 255), 
	color(255, 0, 0),
	color(255, 0, 40),
	color(0,255,40),
	color(0, 255, 255)
};

// int cores[][3] = {
// 	{ 255, 255, 255 }, // 	0 — branco
// 	{ 255, 0, 0 },	// 		1 — vermelho
// 	{ 255, 0, 40 }, //		2 — magentinha
// 	{ 0, 255, 40 }, //		3 — verdito
// 	{ 0, 255, 255 } // 		4 — azul claro
// };

int paletas[][3] = {
	{ 0, 0, 0 }, 
	{ 0, 1, 4 }, 
	{ 2, 3, 3 }
};


class fixture {
	public:
		int channel;
		model modelo;
		float qual;
		int nChannels;
		//float rand;
		int rand;

		fixture(int channel = 1, model modelo = LED) {
			//rand = random(0,1);
			rand = random(0,3);
			if (modelo == LED) {
				nChannels = 3;
			} else if (modelo == MOVINGHEAD) {
				nChannels = 10;
			}
		}
};

fixture fixtures[] = {

	fixture(01, LED),
	fixture(04, LED),
	fixture(07, LED),
	fixture(10, LED),

	fixture(13, LED),
	fixture(16, LED),
	fixture(19, LED),
	fixture(21, LED),

	fixture(24, LED),
	fixture(27, LED),
	fixture(30, LED),
	fixture(33, LED),

	fixture(36, LED),
	fixture(39, LED),
	fixture(42, LED),
	fixture(45, LED),

	fixture(48, LED),
	fixture(51, LED),
	fixture(54, LED),
	fixture(57, LED),

	fixture(60, LED),
	fixture(63, LED),
	fixture(66, LED),
	fixture(69, LED),

	fixture( 73, MOVINGHEAD),
	fixture( 83, MOVINGHEAD),
	fixture( 93, MOVINGHEAD),
	fixture(103, MOVINGHEAD),
};


void fixturesIndex() {
	int i = 0;
	for (auto & f : fixtures) {
		f.qual = i / (float) 28; //(float)fixtures.length;
		i++;
	}
}
//	int i = 0;

// aqui vao mais as bolas de cima.

// vai defasar pra paleta
int cor = 0;
int paleta = 0;


float fade = 1.0;
bool botoes[NBUTTONS];
bool buttonPressed[NBUTTONS];
int potVal[8];
int cena;
float easing = 10.0;


void setColor(fixture & f, color & c) {
	int offset = f.modelo == MOVINGHEAD ? 6 : 0;
	DmxSimple.write(f.channel + 0 + offset, c.r * fade);
	DmxSimple.write(f.channel + 1 + offset, c.g * fade);
	DmxSimple.write(f.channel + 2 + offset, c.b * fade);
}

void setColorEasy(fixture & f, color & c) {
	int offset = f.modelo == MOVINGHEAD ? 6 : 0;
	dmx[f.channel + 0 + offset] = c.r * fade;
	dmx[f.channel + 1 + offset] = c.g * fade;
	dmx[f.channel + 2 + offset] = c.b * fade;
}

void playColor(fixture & f, int i = -1) {
	if (i == -1) {
		i = f.rand;
	}
//	int indiceCor = paletas[paleta][f.rand];
	int indiceCor = paletas[paleta][i];
	setColorEasy(f, colors[indiceCor]);
}


void pisca() {
	// int red = cores[cor][0] * fade;
	// int green = cores[cor][1] * fade;
	// int blue = cores[cor][2] * fade;

	// random no beat
	if (cena == 0) {
		for (auto & f : fixtures) {
			if (random(0, 4) < 1) {
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

	// int nFitas = 3;
	// for (int a = 0; a < nFitas; a++) {
	// 	bool ligado = conta % nFitas == a;
	// 	dmx[a * 3 + 0] = ligado ? red : 0;
	// 	dmx[a * 3 + 1] = ligado ? green : 0;
	// 	dmx[a * 3 + 2] = ligado ? blue : 0;
	// }

	conta ++;
	digitalWrite(LED_BUILTIN, conta % 2 ? LOW : HIGH);
}

float somaTempo = 0;
int tapCount = 0;

void bpmTap() {
	tempo = millis() - lastTempo;
	lastTempo = millis();
	Serial.println(tempo);
	if (tempo > 0 && tempo < 1000) {
		Serial.println("tap" + String(tapCount));
		somaTempo += tempo;
		tapCount ++;
	} else {
		tapCount = 0;
		somaTempo = 0;
	}

	int nTap = 3;
	if (tapCount == nTap) {
		Serial.println("resetTempo");
		tempo = somaTempo / (float) nTap;
		t.stop(evento);
		evento = t.every(tempo, pisca);
		tapCount = 0;
		somaTempo = 0;
	}
	Serial.println("BpmTap" + String(tempo));
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
		// painel é assim: 430215
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

	// fake por enquanto. logo fazer enviar de todos os DMX Easy.
    for (int a = 0; a < NCHANNELS; a++) {
      if (easing > 0) {
        dmxEasy[a] += (dmx[a] - dmxEasy[a]) / easing;
        DmxSimple.write(a + 1, byte(dmxEasy[a]));
      } else {
        dmxEasy[a] = dmx[a];
        DmxSimple.write(a + 1, byte(dmx[a]));
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
				cena = map(analogRead(indexPot), 5, 1023, 0, 2);
				if (debug) {
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
//				cor = map(analogRead(indexPot), 5, 1023, 0, NCORES);
				paleta = map(analogRead(indexPot), 5, 1023, 0, NPALETAS);
				if (debug) {
					Serial.println(cor);
				}
			}

			else if (indexPot == 14) { //quarto pot
				bpm = map(analogRead(indexPot), 5, 1023, 60, 180);
				float tempo = 60000 / bpm;
				t.stop(evento);
				evento = t.every(tempo, pisca);
				if (debug) {
					Serial.println(bpm);
				}
			}

			else if (indexPot == 21) { //primeiro pot de baixo - fade
				fade =	map(analogRead(indexPot), 5, 1023, 0, 100) / 100.0;
				if (debug) {
					Serial.println(fade);
				}
			}

			else if (indexPot == 20) { //segundo pot de baixo - pan
				for (auto & f : fixtures) {
					if (f.modelo == MOVINGHEAD) {
						int val =	map(analogRead(indexPot), 5, 1023, 0, 255);
						DmxSimple.write(f.channel, val);
					}
				}
			}
			else if (indexPot == 19) { //terceiro pot de baixo - tilt
				for (auto & f : fixtures) {
					if (f.modelo == MOVINGHEAD) {
						int val =	map(analogRead(indexPot), 5, 1023, 0, 255);
						DmxSimple.write(f.channel + 2, val);
					}
				}
			}
			else if (indexPot == 18) { //quarto pot de baixo
				// talvez fazer cena de moving head
				// fade =	map(analogRead(indexPot), 5, 1023, 0, 100) / 100.0;
				// Serial.println(fade);
			}// continua 20, 19, 18
		}
	}
}


void strobo() {
	piscante = !piscante;
	// melhorar isso aqui pra nao pegar os pan e tilt
	// for (int a = 0; a < NCHANNELS; a++) {
	// 	DmxSimple.write(a + 1, piscante ? 255 : 0);
	// }

	color c = piscante ? color(0,0,0) : color(255,255,255);
	for (auto & f : fixtures) {
		setColor(f, c);
	}
}

void blackout() {
	//evita um branco solido la no inicio
	for (int a = 0; a < NCHANNELS; a++) {
		DmxSimple.write(a , 0);
	}
}