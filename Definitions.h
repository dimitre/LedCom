color black = color(0,0,0);

color colors[] = { 
	color(255, 255, 255),  // 	0 — branco
	color(255, 0, 0), // 		1 — vermelho
	color(255, 0, 40), //		2 — magentinha
	color(0,255,40), //			3 — verdito
	color(0, 255, 255) // 		4 — azul claro
};

int paletas[][3] = {
	{ 0, 0, 0 },  // branco
	{ 1, 1, 1 },  // vermelho
	{ 2, 2, 2 },  // magenta
	{ 3, 3, 3 },  // verde
	{ 4, 4, 4 },  // azulclaro
	{ 0, 1, 4 },  // branco verm azul claro
	{ 2, 3, 3 },  // magenta verde verde
	{ 0, 1, 1 },  // branco verm verm
	{ 0, 4, 2 }	  // branco azul magenta
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
