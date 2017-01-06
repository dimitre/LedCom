enum model {
	LED, 
	LINHA,
	MOVINGHEAD, 
};

struct color {
public:
	unsigned int r;
	unsigned int g;
	unsigned int b;
	color (int rr, int gg, int bb) : r(rr), g(gg), b(bb) {};

	// color (int rr = 255, int gg = 255, int bb = 255) {
	// 	r = rr;
	// 	g = gg;
	// 	b = bb;
	// }
};

class fixture {
	public:
		// unsigned int channel;
		// unsigned int canalRgb;
		// unsigned int rand;

		// sera problema?

		byte channel;
		model modelo;
		int x, y;
		byte canalRgb;
		byte rand;

		float xx, yy; 
		byte indice;
		float qual;
		// fixture (int ch = 1, model mod = LED, int estex=0, int estey=0) {
		// 	channel = ch;
		// 	modelo = mod;
		// 	x = estex;
		// 	y = estey;

		fixture (byte ch = 1, model mod = LED, int estex=0, int estey=0) : channel(ch), modelo(mod), x(estex), y(estey) {
			rand = random(0,3);
			canalRgb = modelo == MOVINGHEAD ? ch + 3 : ch;
		}
};
