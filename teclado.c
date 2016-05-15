//------------------------------------------------------------------------------
// teclado.c
//
//   Programa para el manejo de un teclado matricial.
//
// Autores: Juan Manuel Montero y Rub�n San Segundo.
// Adaptado a C por: Javier Guill�n �lvarez
//------------------------------------------------------------------------------
#define NUM_FILAS 4
#define NUM_COLS 4
#define EXCIT 1

extern UWORD salida;                                          //Variable externa 
//------------------------------------------------------
// char teclado(void)
//
// Descripci�n:
//   Explora el teclado matr<icial y devuelve la tecla 
//   pulsada
//------------------------------------------------------
char teclado(void) {
	//char tecla;
	BYTE fila, columna, fila_mask;
	static char teclas[4][4] = {{"123C"},
								{"456D"},
								{"789E"},
								{"A0BF"}};
	// Bucle de exploraci�n del teclado
	while (TRUE) {
		// Excitamos una columna
		for (columna = NUM_COLS - 1; columna >= 0; columna--) {
			salida = salida & 0xFFF0;                //Realizamos un AND binario  con el valor actual de salida y 0xFFF0 para no interferir
			salida = salida | (EXCIT << columna);    
			set16_puertoS (salida);					// Se env�a la excitaci�n de columna
			retardo(20000);							// Esperamos respuesta de optoacopladores
			
			// Exploramos las filas en busca de respuesta
			for (fila = NUM_FILAS - 1; fila >= 0; fila--) {
				fila_mask = EXCIT << fila;					// M�scara para leer el bit de la fila actual
				if (lee16_puertoE() & fila_mask) {				// Si encuentra tecla pulsada,
					while (lee_puertoE() & fila_mask);				//	Esperamos a que se suelte
					retardo(20000);						//	Retardo antirrebotes
					return teclas[fila][columna];					//	Devolvemos la tecla pulsada
				}
			}
			// Siguiente columna
		}
		// Exploraci�n finalizada sin encontrar una tecla pulsada
	}
	// Reiniciamos exploraci�n
}