
class fixture {
	public:
		uint16_t channel;
		uint16_t port;
		bool buttonState = true;
		bool sobe = false;
		byte index;

		Bounce debounce;

		bool ready = false;

		void (*mover)() = NULL;
		void (*parar)() = NULL;
		void (*panic)() = NULL;


		fixture (uint16_t ch = 1, uint16_t p = 1, byte i = 1) : channel(ch), port(p), index(i) {
			pinMode(port, INPUT_PULLUP);
			//debounce = Bounce();
			//debounce = Bounce (button, 8);
			debounce.attach(port, 8);
		}

		void setDmx(int c, int v) {
			DmxSimple.write(c + channel, byte(v));
		}

		void para() {
			// para movimento
			setDmx(5, 0);
		}

		void move() {
			if (channel > 0) {
				sobe = digitalRead(SOBEDESCE_PIN) == LOW;
								Serial.println(sobe ? "SUBINDO" : "DESCENDO");

				if (sobe) {
					setDmx(4, 200);
				} else {
					setDmx(4, 0);
				}
				// aciona movimento
				setDmx(5, 200);
			} else {
				if (ready) {
					if (port == 33) {
						(*mover)();
						Serial.println("MOVER");
					}
					else if (port == 22) {
						Serial.println("PANIC");
						(*panic)();
					}
				}
				//xaxa
				//if ((*mover))
			}
		}

		// void aciona() {
		// 	if (digitalRead(port) == LOW) {
		// 		move();
		// 	}

		// 	else {
		// 		para();
		// 	}
		// }

		void readButton() {
			debounce.update();
			if (debounce.fallingEdge()) {
				move();
				Serial.print(sobe ? "SOBE :: " : "DESCE :: ");
				Serial.println(String(channel) + "\tbutton :: " + String(port) + "\tindex :: " + String(index));
			}
			if (debounce.risingEdge()) {
				para();
				Serial.println("Para :: " + String(channel));

			}

			// if (buttonState != digitalRead(button)) {
			// 	aciona();
			// 	buttonState = digitalRead(button);
			// }
		}
};





class button {
public:
	uint16_t port;
	//int tipo;
	//bool buttonState;
	//bool sobe = false;
	Bounce debounce;

	void (*mover)() = NULL;
	void (*parar)() = NULL;

	button( uint16_t b) : port(b) {
		pinMode(port, INPUT_PULLUP);
		debounce.attach(port, 8);
		Serial.println("debounce button port " + String(port));
	}

	void readButton() {
		//Serial.println("readbutton botao");
		//sobe = digitalRead(SOBEDESCE_PIN) == LOW;
		//Serial.println(digitalRead(port));
		debounce.update();
		if (debounce.fallingEdge()) {
			Serial.println("aperta");
			// if ((*mover) != NULL) {
			// 	(*mover)();
			// }
		}
		if (debounce.risingEdge()) {
			Serial.println("solta");
			// if ((*parar) != NULL) {
			// 	(*parar)();
			// }
		}

		// if (buttonState != digitalRead(port)) {
		// 	if (digitalRead(port) == LOW) {
		// 		if (sobe) {
		// 			if ((*subir) != NULL) {
		// 				(*subir)();
		// 			}
		// 		} else {
		// 			(*descer)();
		// 		}
		// 	} else {
		// 		(*parar)();
		// 	}
		// 	//Serial.println(tipo);
		// 	buttonState = digitalRead(port);
		// }
	}
};

