
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
			if (channel > 0) {
				Serial.println("Para :: " + String(channel));
				setDmx(5, 0);
			} else {
				if (ready) {
					(*parar)();
				}
			}
		}

		void move() {
			if (channel > 0) {
				Serial.print(sobe ? "SOBE :: " : "DESCE :: ");
				Serial.println(String(channel) + "\tbutton :: " + String(port) + "\tindex :: " + String(index));

				sobe = digitalRead(SOBEDESCE_PIN) == LOW;
				//Serial.println(sobe ? "SUBINDO" : "DESCENDO");

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
					}
					else if (port == 22) {
						(*panic)();
					}
				}
				//xaxa
				//if ((*mover))
			}
		}


		void readButton() {
			debounce.update();
			if (debounce.fallingEdge()) {
				move();
			}
			if (debounce.risingEdge()) {
				para();
			}

		}
};




// Rever este
/*
class button {
public:
	uint16_t port;
	Bounce debounce;

	void (*mover)() = NULL;
	void (*parar)() = NULL;

	button( uint16_t b) : port(b) {
		pinMode(port, INPUT_PULLUP);
		debounce.attach(port, 8);
		Serial.println("debounce button port " + String(port));
	}

	void readButton() {
		debounce.update();
		if (debounce.fallingEdge()) {
			Serial.println("aperta");
		}
		if (debounce.risingEdge()) {
			Serial.println("solta");
		}
	}
};

*/