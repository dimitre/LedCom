

//#include "Timer.h"
// CONFIGURACOES GERAIS
#define NBUTTONS 2
#define DMX_PIN 12
#define NCHANNELS 140

// remover
#define NPALETAS 15
#define NCENAS 6

// variavel pra guardar o estado do ultimo botao
bool debug = false;
bool piscante;

float botao8;

// nao sei se fica
unsigned int divideTempo = 1;


// float dmx[NCHANNELS];
// float dmxEasy[NCHANNELS];
double easing = 10.0;
double dmx[NCHANNELS];
double dmxEasy[NCHANNELS];

unsigned int cena;
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
unsigned int nextAutomatic;
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
		f.indice = i;
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

//
void debugFixtures() {
	delay(800);
	for (auto & f : fixtures) {
		String line = String(f.channel) + "\t\t" + String(f.xx) + "\t" + String(f.yy);
		Serial.println(line);
	}
}


void setupBardapraia() {
	Serial.begin(9600);
	fixturesIndex();
//	debugFixtures();

	for (int a = 0; a < NBUTTONS; a++ ) {
		pinMode(a, INPUT_PULLUP);
	}

	DmxSimple.usePin(DMX_PIN);
	DmxSimple.maxChannel(NCHANNELS);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
}


void setDmx(int c, int v) {
	dmx[c] = v;
	dmxEasy[c] = v;
	DmxSimple.write(c, byte(v));
}

void allWhite() {
	for (int a=0; a<73; a++) {
		DmxSimple.write(a, 255);
	}
	Serial.println("allWhite");
}

void setColor(fixture & f, color c) {
	if (f.modelo != MOVINGHEAD) {
		// int r = c.r * fade;
		// int g = c.g * fade;
		// int b = c.b * fade;
		setDmx(f.canalRgb + 0, c.r * fade);
		setDmx(f.canalRgb + 1, c.g * fade);
		setDmx(f.canalRgb + 2, c.b * fade);
		// setDmx(f.canalRgb + 0, r);
		// setDmx(f.canalRgb + 1, g);
		// setDmx(f.canalRgb + 2, b);
	}
}

void setColorEasy(fixture & f, color & c) {
	if (f.modelo != MOVINGHEAD) {
		dmx[f.canalRgb + 0] = c.r * fade;
		dmx[f.canalRgb + 1] = c.g * fade;
		dmx[f.canalRgb + 2] = c.b * fade;
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

	int indiceCor = paletas[paleta][(conta/2)%3];
	color c = colors[indiceCor];
	if (black) {
		c = color(0,0,0);
	}

	float r = c.r;
	float g = c.g;
	float b = c.b;

	float w = 0;

	if (r == g && g == b) {
		w = r;
		r = g = b = 0;
	}


	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			dmx[f.canalRgb + 0] = r;
			dmx[f.canalRgb + 1] = g;
			dmx[f.canalRgb + 2] = b;
			dmx[f.canalRgb + 3] = w;
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
				setDmx(f.channel + c, 0);
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
				setColorEasy(f, black);
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
				playColor(f, map(f.x, 0, 5, 0, 3));
			} else {
				setColorEasy(f, black);
			}
		}
	}

	else if (cena == 5) {
		for (auto & f : fixtures) {
			if (conta % maxY == f.y) {
//				playColor(f, f.yy * 3);
				playColor(f, map(f.y, 0, 12, 0, 3));

			} else {
				setColorEasy(f, black);
			}
		}
	}

	else if (cena == 6) {
		for (auto & f : fixtures) {
			playColor(f, f.indice % 3);
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
	for (int a=0; a<NCHANNELS; a++) {
      if (easing > 0) {
        dmxEasy[a] += (dmx[a] - dmxEasy[a]) / easing;
      } else {
        dmxEasy[a] = dmx[a];
      }
	}
	// deixo os movings de fora
    for (int a = 1; a < 73; a++) {
//    for (int a = 0; a < NCHANNELS; a++) {
      if (easing > 0) {
        //dmxEasy[a] += (dmx[a] - dmxEasy[a]) / easing;

        // xaxa - será que o byte ta dando problema na conversao?
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
			setDmx(f.channel + 0 , val);
		}
	}
}

void setMovingTilt(int val) {
	if (debug) 
	{
		Serial.println("setMovingTilt :: " + String(val));
	}

	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			setDmx(f.channel + 1 , val);
		}
	}
}

void setMovingDimmer(int val) {
	if (debug) 
	{
		Serial.println("setMovingDimmer :: " + String(val));
	}

	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			setDmx(f.channel + 2 , val);
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
				cena = map(analogRead(indexPot), 0, 1023, 0, NCENAS + 1);
				if (cena > NCENAS) {
					cena = NCENAS;
				}
				//cena = constrain(cena, 0, NCENAS);
				divideTempo = map(analogRead(indexPot), 0, 1023, 0, NCENAS * 4) % 4;
				if (debug) 
				{
					Serial.println("cena :: " + String(cena));
				}
			}

			else if (indexPot == 16) { //segundo pot
				easing = map(analogRead(indexPot), 0, 1023, 0, 20);
				if (debug) {
					Serial.println(easing);
				}
			}
			else if (indexPot == 15) { //terceiro pot
				paleta = map(analogRead(indexPot), 0, 1023, 0, NPALETAS + 1);
				if (paleta > NPALETAS) {
					paleta = NPALETAS;
				}
				//paleta = constrain(paleta, 0, NPALETAS);
				if (debug) 
				{
					Serial.println("paleta :: " + String(paleta));
				}
			}

			else if (indexPot == 14) { //quarto pot
//				bpm = map(analogRead(indexPot), 0, 1023, 60, 180);
				bpm = map(analogRead(indexPot), 0, 1023, 100, 140);
//				bpm = map(analogRead(indexPot), 0, 1023, 20, 280);
				tempo = 60000 / bpm;
				//temporizador.stop(evento);
				//evento = temporizador.every(tempo, pisca);
				if (debug) {
					Serial.println("bpm :: " + String(bpm));
				}
			}

			else if (indexPot == 21) { //primeiro pot de baixo - fade
				fade =	map(analogRead(indexPot), 0, 1023, 0, 100) / 100.0;
				if (debug) {
					Serial.println("fade :: " + String(fade));
				}
			}

			else if (indexPot == 20) { //segundo pot de baixo - pan
				int val =	map(analogRead(indexPot), 0, 1023, 0, 255);
				setMovingPan(val);
			}
			else if (indexPot == 19) { //terceiro pot de baixo - tilt
				int val =	map(analogRead(indexPot), 0, 1023, 0, 255);
				setMovingTilt(val);
			}
			else if (indexPot == 18) { //quarto pot de baixo

				//botao8 = ;
				setMovingDimmer(map(analogRead(indexPot), 0, 1023, 0, 255));
				// talvez fazer cena de moving head
				// fade =	map(analogRead(indexPot), 0, 1023, 0, 100) / 100.0;
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
	for (int a=1; a<73; a++) {
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
	for (int a = 1; a < NCHANNELS; a++) {
		DmxSimple.write(a , 0);
	}
}

void movingsEasy() {
	for (auto & f : fixtures) {
		if (f.modelo == MOVINGHEAD) {
			// mudei pra 4 pra ter o rgbw agora
			for (int a = 0; a <  4; a++) {
				int c = f.canalRgb + a;
				DmxSimple.write(c, byte(dmxEasy[c]));
			}
		}
	}
}





void loopBardapraia() {
	if (millis() > nextEvent) {
		pisca();
		nextEvent = millis() + tempo;
	}

	if (buttonPressed[1] || buttonPressed[0]) {
		if (buttonPressed[0] && buttonPressed[1]) {
			chuvisco();
		} else {

			if (buttonPressed[0]) {
				strobo();
			}
		}
	} else {
		sendDmxEasy();
		movingsEasy();
	}
}

void randomAll() {
	cena = random(0,4);
	paleta = random(0, NPALETAS);
	bpm = random(60, 180);
	tempo = random(400,800);
	easing = random(0,20);
	//setBpm(bpm);
}

void automatic() {
	if (millis() > nextAutomatic) {
		randomAll();
		nextAutomatic = millis() + 180000;
	}
}

