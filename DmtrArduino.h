
/*
PAINEL é
3 2 1 0
7 6 5 4

cena, cor, easing, xxx
fader

fader
*/




//#include "Timer.h"


#define BUTTON_PIN 4
#define NBUTTONS 6
#define DMX_PIN 12

#define NCOLORS 5


bool debug = true;


float dmx[12];
float dmxEasy[12];

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




int colors[NCOLORS][3] = {
  { 255, 255, 255 }, //branco
  { 255, 0, 0 }, //vermelho
  { 255, 0, 40 }, //magentinha
  { 0, 255, 40 }, //verdito
  { 0, 255, 255 }
};

int cor = 0;
float fade = 1.0;
bool botoes[NBUTTONS];
int potVal[8];
int cena;
float easing = 10.0;


void pisca() {
  int red = colors[cor][0] * fade;
  int green = colors[cor][1] * fade;
  int blue = colors[cor][2] * fade;

  int nFitas = 3;
  for (int a = 0; a < nFitas; a++) {
    bool ligado = conta % nFitas == a;
    dmx[a * 3 + 0] = ligado ? red : 0;
    dmx[a * 3 + 1] = ligado ? green : 0;
    dmx[a * 3 + 2] = ligado ? blue : 0;
  }
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

  int nTap = 4;
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
  if (a == 1) {
    bpmTap();
  }
  if (debug) {
  	Serial.println("button down ::: " + String(a));
  }
}

void buttonUp(int a) {
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

void readPots() {
    int tolerancia = 3;

  // checa se o valor dos pots mudou
  for (int a = 0; a < 8; a++) {
    int indexPot = 14 + a;
    if (abs(potVal[a] - analogRead(indexPot)) > tolerancia) {
      //if (potVal[a] != analogRead(indexPot)) {
      Serial.println("Change Value Pot" + String(a) + " - Pin:" + String(indexPot) + " ::: " + String(analogRead(indexPot)));
      potVal[a] = analogRead(indexPot);
      if (indexPot == 17) { //primeiro pot
        cena = map(analogRead(17), 5, 1023, 0, 2);
      }

      else if (indexPot == 16) { //segundo pot
        easing = map(analogRead(16), 5, 1023, 0, 30);
        if (debug) {
        	Serial.println(easing);
        }
      }
      else if (indexPot == 15) { //terceiro pot
        cor =  map(analogRead(15), 5, 1023, 0, NCOLORS);
        if (debug) {
	    	Serial.println(cor);
	    }
      }

      else if (indexPot == 14) { //quarto pot
        bpm = map(analogRead(14), 5, 1023, 60, 180);
        float tempo = 60000 / bpm;
        t.stop(evento);
        evento = t.every(tempo, pisca);
        Serial.println(bpm);
      }

      else if (indexPot == 21) { //terceiro pot
        fade =  map(analogRead(21), 5, 1023, 0, 100) / 100.0;
        Serial.println(fade);
      }
    }
  }
}
