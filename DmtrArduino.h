

//#include "Timer.h"

#define NBUTTONS 6
#define DMX_PIN 12
#define NCHANNELS 140

// remover
//#define NCOLORS 5
#define NPALETAS 8

float botao8;

bool debug = false;
bool piscante;

int divideTempo = 1;

// com o tempo defasar este.
float dmx[NCHANNELS];
float dmxEasy[NCHANNELS];



int pan = 127;
int tilt = 127;

// cena
int conta = 0;

// bpm tap
float bpm = 120.0;
float tempo = 60000.0 / bpm;
unsigned int nextEvent;
float lastTempo = 0;


int minX = 100;
int maxX = -100;
int minY = 100;
int maxY = -100;


// colocar aqui a posicao x, y, z, categoria de onde fica no bar da praia, etc.

enum model {
	LED, 
	LINHA,
	MOVINGHEAD, 
};

// YUMMY - comecar a usar
struct color {
public:
	float r;
	float g;
	float b;
	color (float rr = 255, float gg = 255, float bb = 255) {
		r = rr;
		g = gg;
		b = bb;

	}
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
	{ 1,1,1 }, 
	{ 2,2,2 }, 
	{ 3,3,3 }, 
	{ 4,4,4 }, 
	{ 0, 1, 4 }, 
	{ 2, 3, 3 },
	{ 1, 1, 1 },
	{ 2,2,2 }	
};


class fixture {
	public:
		int channel;
		model modelo;
		float qual;
		int nChannels;
		//float rand;
		int rand;
		int x, y;
		float xx, yy; 

		fixture(int ch = 1, model mod = LED, int xx=0, int yy=0) {
			channel = ch;
			modelo = mod;
			x = xx;
			y = yy;
			// //rand = random(0,1);
			// Serial.println("fixture initializatino channel:: " + String(channel));

			// rand = random(0,3);
			// if (modelo == LED) {
			// 	nChannels = 3;
			// } else if (modelo == MOVINGHEAD) {
			// 	nChannels = 10;
			// }
		}
};


fixture fixtures[] = {

	fixture(01 , LINHA, 0, 1),
	fixture(04 , LINHA, 0, 4),
	fixture(07 , LINHA, 0, 7),
	fixture(10 , LINHA, 0, 10),

	fixture(13 , LINHA, 5, 1),
	fixture(16 , LINHA, 5, 4),
	fixture(19 , LINHA, 5, 7),
	fixture(22 , LINHA, 5, 10),

	fixture(25 , LED, 3, 0),
	fixture(28 , LED, 4, 0),
	fixture(31 , LED, 3, 1),
	fixture(34 , LED, 4, 1),

	fixture(37 , LED, 1, 2),
	fixture(40 , LED, 2, 2),
	fixture(43 , LED, 1, 3),
	fixture(46 , LED, 2, 3),

	fixture(49 , LED, 3, 8),
	fixture(52 , LED, 4, 8),
	fixture(55 , LED, 3, 9),
	fixture(58 , LED, 4, 9),

	fixture(61, LED, 1, 10),
	fixture(64, LED, 2, 10),
	fixture(67, LED, 1, 11),
	fixture(70, LED, 2, 11),

	fixture( 73, MOVINGHEAD),
	fixture( 81, MOVINGHEAD)
};


void fixturesIndex() {
	int i = 0;
	for (auto & f : fixtures) {
		minX = min(minX, f.x);
		maxX = max(maxX, f.x);
		minY = min(minY, f.y);
		maxY = max(maxY, f.y);

		f.qual = i / (float) 28; //(float)fixtures.length;
		i++;
	}

	for (auto & f : fixtures) {
		f.xx = map(f.x, minX, maxX, 0, 100)/100.0f;
		f.yy = map(f.y, minY, maxY, 0, 100)/100.0f;
	}
}
//	int i = 0;

// aqui vao mais as bolas de cima.

// vai defasar pra paleta
//int cor = 0;
int paleta = 0;


float fade = 1.0;
bool botoes[NBUTTONS];
bool buttonPressed[NBUTTONS];
int potVal[8];
int cena;
float easing = 10.0;

void allWhite() {
	for (int a=0; a<73; a++) {
		DmxSimple.write(a, 255);
	}
	Serial.println("allWhite");
}

void setColor(fixture & f, color c) {
	// c.r = 255;
	// c.g = 255;
	// c.b = 255;
	int offset = f.modelo == MOVINGHEAD ? 3 : 0;


	// Serial.println("setColor :: " + String(c.r));
	// Serial.println(f.channel + 0 + offset);
	// Serial.println(c.r * fade);

	if (f.modelo != MOVINGHEAD) {
		int r = c.r * fade;
		int g = c.g * fade;
		int b = c.b * fade;
		int canal = f.channel + offset;
		dmx[canal+0] = r;
		dmx[canal+1] = g;
		dmx[canal+2] = b;
		DmxSimple.write(canal+0, r);
		DmxSimple.write(canal+1, g);
		DmxSimple.write(canal+2, b);
	}

}

void setColorEasy(fixture & f, color & c) {
	if (f.modelo != MOVINGHEAD) {
//		int offset = f.modelo == MOVINGHEAD ? 3 : 0;
		int offset = 0;
		dmx[f.channel + 0 + offset] = c.r * fade;
		dmx[f.channel + 1 + offset] = c.g * fade;
		dmx[f.channel + 2 + offset] = c.b * fade;
	}
}

void playColor(fixture & f, int i = -1) {
	if (i == -1) {
		i = f.rand;
	}
//	int indiceCor = paletas[paleta][f.rand];
	int indiceCor = paletas[paleta][i];
	setColorEasy(f, colors[indiceCor]);
}



// void setAll(int c, int v) {
// 	dmx[c] = v;
// 	dmxEasy[c] = v;
// 	DmxSimple.write(c, v);
// }

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
			int canal = f.channel + 3;
			dmx[canal + 0] = r;
			dmx[canal + 1] = g;
			dmx[canal + 2] = b;
		}
	}
}



void resetMoving() {
	Serial.println("resetMoving :: ");
	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			int canal = f.channel + 0;
			for (int c=6; c<=7; c++) {
			//for (auto c : { 6, 7 }) {
				dmx[canal + c] = 0;
				dmxEasy[canal + c] = 0;
				DmxSimple.write(canal + c, 0);
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

	// cores solidas rolando full
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


	// ROLL X
	else if (cena == 3) {
		for (auto & f : fixtures) {
			if (conta % maxX == f.x) {
//				playColor(f, f.xx * 3);
				playColor(f, 0);
			} else {
				setColorEasy(f, black);
			}
		}
	}

	else if (cena == 4) {
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

	Serial.println("setMovingPan :: " + String(val));
	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			int canal = f.channel + 0;
			dmx[canal] = val;
			DmxSimple.write(canal, val);
		}
	}
}

void setMovingTilt(int val) {
	Serial.println("setMovingTilt :: " + String(val));

	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			int canal = f.channel + 1;
			dmx[canal] = val;
			DmxSimple.write(canal, val);

		}
	}
}

void setMovingDimmer(int val) {
	Serial.println("setMovingDimmer :: " + String(val));

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
				cena = map(analogRead(indexPot), 5, 1023, 0, 4);
				divideTempo = map(analogRead(indexPot), 5, 1023, 0, 12) % 4;
				//if (debug) 
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
				temporizador.stop(evento);
				evento = temporizador.every(tempo, pisca);
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

