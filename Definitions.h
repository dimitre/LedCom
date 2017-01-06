color black = color(0,0,0);

color colors[] = { 
	color(255, 255, 255),  // 	0 — branco
	color(255, 0, 0), // 		1 — vermelho
	color(255, 0, 40), //		2 — magentinha
	color(0,255,40), //			3 — verdito
	color(0, 255, 255), // 		4 — azul claro
	color(0, 255, 0), // 		5 — g
	color(0, 0, 255), // 		6 — b	
	color(130, 63, 212), // 		7 — roxo lindo
	color(202, 255, 16), // 		8 — verde limao
	color(130, 255, 212), // 		9 — verde clarissimo
	color(255, 20, 212), // 		10 — rosinha
};

int paletas[][3] = {
	{ 0, 0, 0 },  // branco
	{ 1, 1, 1 },  // vermelho
	{ 2, 2, 2 },  // magenta
	{ 3, 3, 3 },  // verde
	{ 4, 4, 4 },  // azulclaro
	{ 5, 5, 5 },  // g
	{ 6, 6, 6 },  // b
	{ 7, 7, 7 },  // roxo lindo
	{ 8, 8, 8 },  // verde limao
	{ 9, 9, 9 },  // verde clarissimo
	{ 10, 10, 10 },  // rosinha
	{ 0, 1, 4 },  // branco verm azul claro
	{ 2, 3, 3 },  // magenta verde verde
	{ 0, 1, 1 },  // branco verm verm
	{ 0, 4, 2 },  // branco azul magenta
	{ 7, 7, 3 }	  // branco azul magenta

		// 12 paletas?
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


	fixture(25 , LED, 3, 3), //ok, bola na frente dos p.a.s
	fixture(28 , LED, 3, 4), // segunda bola em frente ao p.a, menorzinha
	fixture(31 , LED, 2, 5), // terceira bola em frente ao pa, grande
	fixture(34 , LED, 4, 0), //bola zero, grande

	fixture(37 , LED, 3, 1), // bola acima do dj, grande
	fixture(40 , LED, 2, 0), // bola acima da cozinha, pequena
	fixture(43 , LED, 1, 3), // canal faltante
	fixture(46 , LED, 2, 9), // bola acima do bar, grande, na frente da placa de madeira

	fixture(49 , LED, 1, 10), // bola logo a esquerda da ultima
	fixture(52 , LED, 0, 11), // pequena na esquerda
	fixture(55 , LED, 2, 11), // fundo
	fixture(58 , LED, 4, 9), // pequena direita

	fixture(61, LED, 4, 9), // bolinha pouco a esquerda de miller
	fixture(64, LED, 3, 11), // grande ao lado da arvore
	fixture(67, LED, 4, 12), // ultima bola da direita
	fixture(70, LED, 2, 11), 

	fixture( 73, MOVINGHEAD),
	fixture( 81, MOVINGHEAD)
};
