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
	color (int rr = 255, int gg = 255, int bb = 255) {
		r = rr;
		g = gg;
		b = bb;
	}
};


class fixture {
	public:
		unsigned int channel;
		model modelo;
		float qual;
		unsigned int canalRgb;
		unsigned int rand;
		int x, y;
		float xx, yy; 

		fixture(int ch = 1, model mod = LED, int estex=0, int estey=0) {
			channel = ch;
			modelo = mod;
			x = estex;
			y = estey;
			rand = random(0,3);
			canalRgb = modelo == MOVINGHEAD ? ch + 3 : ch;
		}
};
