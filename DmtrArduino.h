
/*
PAINEL é
3 2 1 0
7 6 5 4

cena, cor, easing, xxx
fader

fader
*/




//#include "Timer.h"


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


#define BUTTON_PIN 4
#define NBUTTONS 6
#define DMX_PIN 12


int colors[5][3] = {
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
}


void bpmTap() {
  Serial.println("Button");
  tempo = millis() - lastTempo;
  lastTempo = millis();
  Serial.println(tempo);
  if (tempo > 0) {
    t.stop(evento);
    evento = t.every(tempo, pisca);
    Serial.println("pisca");
  }
  Serial.println("BpmTap" + String(tempo));
}

void buttonDown(int a) {
  if (a == 4) {
    bpmTap();
  }
  Serial.println("button down ::: " + String(a));
}

void buttonUp(int a) {
  Serial.println("button up ::: " + String(a));
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
        easing = map(analogRead(16), 5, 1023, 0, 60);
        Serial.println(easing);
      }
      else if (indexPot == 15) { //terceiro pot
        cor =  map(analogRead(15), 5, 1023, 0, 3);
        Serial.println(cor);
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
