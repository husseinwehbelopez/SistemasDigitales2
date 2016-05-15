//------------------------------------------------------------------------------
// XM-11.c
//
//   Programa principal
//
// Autores: Óscar Vázquez Babón y Hussein Wehbe López
//------------------------------------------------------------------------------

// Inclusión de ficheros fuente del proyecto
#include "m5272.h"
#include "m5272lib.c"
#include "m5272lib.h"
#include "m5272adc_dac.c"
#include "m5272lcd.c"
#include "m5272gpio.c"
#include "busquedaBinaria.c"
#include "cadena.c"
#include "imprimeLCD.c"

// Definición de constantes
#define V_BASE 0x40													// Dirección de inicio de los vectores de interrupción
#define DIR_VTMR0 4*(V_BASE+5)												// Dirección del vector de TMR0
#define DIR_VINT1 4*(V_BASE + 1)											// Dirección del vector de INT1
#define FREC_INT 4000													// Frec. de interr. TMR0 = 4000 Hz (cada 0.25ms)
#define PRESCALADO 1
#define CNT_INT1 MCF_CLK/(FREC_INT*PRESCALADO*16)									// Valor de precarga del temporizador de interrupciones TRR0
#if CNT_INT1>0xFFFF
#error PRESCALADO demasiado pequeño para esa frecuencia (CNT_INT1>0xFFFF)
#endif
#define BORRA_REF 0x0002												// Valor de borrado de interr. pendientes de tout1 para TER0
#define NUM_MUESTRAS_PERIODO_10HZ 400											// Número de muestras de la sinusoide de 10 Hz
#define MAX_FRECS 30													// Número máximo de frecuencias que puede usar el programa
#define MAX_TECLAS 12													// Número máximo de caracteres por línea del LCD
#define NUM_FILAS 4 													// Número de filas en el teclado matricial
#define NUM_COLS 4 													// Número de columnas en el teclado matricial
#define EXCIT 1	

UWORD salida = 0x0000;													// Excitación de salida bit 0

// Variables
unsigned int f = 0;													// Número de 0 a n_frecs-1 que se corresponde con la posición de cada frecuencia en el array S_out
unsigned int i;														// Contador de un bucle for usado para obtener el seno y el coseno a partir de sinusoide10Hz
unsigned int j;														// Contador de un bucle for usado para obtener el seno y el coseno a partir de sinusoide10Hz
unsigned int k;														// Contador del bucle for del main
unsigned int k_lin;													// Contador para distribución lineal
unsigned int k_log;													// Contador para distribución logarítmica
unsigned int contadorDFT = 0;												// Cuenta las interrupciones.
unsigned int contadorS_out = 0;												// Cuenta las interrupciones.
unsigned int contadorReset = 0;												// Cuenta las interrupciones.
unsigned int n_dft = 80;												// Número de muestras para la DFT
unsigned int n_frecs = 19;												// Número de frecuencias distintas con las que trabajaremos
unsigned int t_barrido = 152;												// Tiempo de barrido 
unsigned int f_min = 100;												// Frecuencia mínima
unsigned int f_max = 1900;												// Frecuencia máxima
unsigned int t_comp = 8;												// Tiempo de componente
unsigned int salto = 10;												// Valor que depende de las frecuencias máxima y mínima, así como del número de frecuencias, que se usa para rellenar el array pasos[]
unsigned int escala = 1;
unsigned int dft_lcd;													// Pico de la DFT. Posteriormente se convierte a char* para poder mostrarla el LCD
unsigned int frec_lcd;													// Frecuencia donde está el pico de la DFT. Posteriormente se convierte a char* para poder mostrarla el LCD
char* frecuencia;													// Mensaje de frecuencia para el LCD
char* frecuencia_;													// Frecuencia que muestra el LCD
char* frecuencia_a;													// Frecuencia anterior guardada para poder mostrarla en el LCD
char* dft;														// Mensaje de módulo de la DFT para el LCD
char* dft_;														// Módulo de la DFT que muestra el LCD
char* dft_a;														// Módulo de la DFT anterior guardado para poder mostrarlo en el LCD
char* err = "Error";
unsigned int scroll = 0;												// Contador para el scroll del LCD
char cadena[7];														// Array para la función getCadTeclado											// Indica si estamos en la primera línea	
int pasos[MAX_FRECS] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 				// Array con los valores correspondientes al paso, que es la frecuencia dividida entre 10
			160, 170, 180, 190, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int S_out[MAX_FRECS];													// Array que contiene los valores que finalmente sacamos por el ADC
static int sinusoide10Hz[NUM_MUESTRAS_PERIODO_10HZ] = {									// Array con los valores correspondientes a las 400 muestras de un periodo de un seno de 10Hz
			0, 12, 25, 38, 51, 64, 77, 89, 102, 115, 128, 140, 153, 166, 178, 191, 203, 216, 228, 240,
			253, 265, 277, 289, 301, 313, 325, 336, 348, 360, 371, 383, 394, 405, 416, 427, 438, 449, 460, 470,
			481, 491, 501, 511, 521, 531, 541, 551, 560, 569, 579, 588, 596, 605, 614, 622, 630, 639, 647, 654,
			662, 669, 677, 684, 691, 698, 704, 711, 717, 723, 729, 735, 741, 746, 751, 756, 761, 766, 770, 774,
			778, 782, 786, 790, 793, 796, 799, 802, 804, 806, 809, 810, 812, 814, 815, 816, 817, 818, 818, 819,
			819, 819, 818, 818, 817, 816, 815, 814, 812, 811, 809, 807, 804, 802, 799, 796, 793, 790, 786, 783,
			779, 775, 771, 766, 761, 757, 752, 746, 741, 736, 730, 724, 718, 712, 705, 698, 692, 685, 678, 670,
			663, 655, 647, 639, 631, 623, 615, 606, 597, 588, 579, 570, 561, 552, 542, 532, 522, 512, 502, 492,
			482, 471, 461, 450, 439, 428, 417, 406, 395, 384, 372, 361, 349, 338, 326, 314, 302, 290, 278, 266,
			254, 242, 229, 217, 204, 192, 179, 167, 154, 142, 129, 116, 103, 91, 78, 65, 52, 39, 27, 14,
			1, -12, -25, -38, -51, -63, -76, -89, -102, -115, -127, -140, -153, -165, -178, -190, -203, -215, -228, -240,
			-252, -264, -277, -289, -301, -313, -324, -336, -348, -360, -371, -382, -394, -405, -416, -427, -438, -449, -460, -470,
			-481, -491, -501, -511, -521, -531, -541, -551, -560, -569, -579, -588, -597, -605, -614, -622, -631, -639, -647, -655,
			-662, -670, -677, -684, -691, -698, -705, -711, -718, -724, -730, -735, -741, -746, -752, -757, -762, -766, -771, -775,
			-779, -783, -787, -790, -794, -797, -800, -802, -805, -807, -809, -811, -813, -815, -816, -817, -818, -819, -819, -820,
			-820, -820, -819, -819, -818, -817, -816, -815, -813, -812, -810, -808, -806, -803, -800, -798, -794, -791, -788, -784,
			-780, -776, -772, -768, -763, -758, -753, -748, -743, -737, -731, -725,	-719, -713, -707, -700, -693, -686, -679, -672,
			-665, -657, -649, -641, -633, -625, -616, -608, -599, -590, -581, -572, -563, -554, -544, -534, -524, -515, -504, -494,
			-484, -473, -463, -452, -441, -431, -420, -408, -397, -386, -375, -363, -351, -340, -328, -316, -304, -292, -280, -268,
			-256, -244, -231, -219, -207, -194, -182, -169, -156, -144, -131, -118, -106, -93, -80, -67, -55, -42, -29, -16};

float pasoLogs;			
float logs[14] = {1.5229, 1.4449, 1.3870, 1.3424, 1.3069, 1.2781, 1.2542, 1.2341, 1.2167, 1.2020, 1.1891, 1.1777, 1.1676, 1.1586};
					
//------------------------------------------------------
// void rutina_tout0(void)
//
// Descripción:
//   Función de atención a la interrupción para TIMER0
//------------------------------------------------------
void rutina_tout0(void) {
	static int sumaSeno[MAX_FRECS];											// Almacena todos los valores (el sumatorio) de la parte imaginaria de la DFT
	static int sumaCoseno[MAX_FRECS];										// Almacena todos los valores (el sumatorio) de la parte real de la DFT
	int dato = ADC_dato();												// Número obtenido tras interpretar el número leído a la entrada (int ADC_leeRxRAM() de m5272adc_dac.c)
	mbar_writeShort(MCFSIM_TER0,BORRA_REF); 									// Reset del bit de fin de cuenta

	/*	Se incrementa contadorDFT para llevar la cuenta dexm cuantas interrupciones van.
		Recorremos con i las 20 frecuencias. Si estamos en la primera interrupción, ponemos cada componente de los arrays sumaSeno[] y sumaCoseno[] a 0.
		Después se rellena cada posición de dichos arrays (cada posición representa una frecuencia distinta) con el dato multiplpicado por un valor de
		sinusoide10HZ[]. Para el seno este valor es resto de dividir entre el numero de muestras (400) el valor del paso correspondiente a cada frecuencia
		(pasos[i]) multiplicado por contadorDFT. Para el coseno es similar, hay que sumar al producto pasos[i] * contadorDFT el desfase necesario para
		convertir el seno en un coseno. Sabemos que cos(x) = sen(x + pi/2). Este desfase se traduce como un desfase de la cuarta parte de
		NUM_MUESTRAS_PERIODO_10HZ, que son 100 muestras.
		Después, si hemos hecho todas las interrupciones necesarias (contadorDFT == n_dft), contadorDFT se reinicia a 0, realizamos el desescalado de
		los arrays del seno y del coseno, y en el array S_out[] guardamos para cada frecuencia el resultado que devuelva la función busquedaBinaria
		de sumaCoseno[k]^2 + sumaSeno[k]^2, es decir, el módulo de la DFT. El objetivo de busquedaBinaria es escalar dicho módulo.
	*/
	contadorDFT++;																	// Incrementamos contadorDFT
	if (contadorDFT == 1) {																// Si contadorDFT es 1
	 	memset(sumaSeno, 0, MAX_FRECS);														//	Todas las posiciones de sumaSeno[i] se ponen a 0
	 	memset(sumaCoseno, 0, MAX_FRECS);													//	Todas las posiciones de sumaCoseno[i] se ponen a 0
	}
	for (i = 0; i < n_frecs; i++) {															// Para cada frecuencia
		sumaSeno[i] += (dato * (sinusoide10Hz[(pasos[i] * (contadorDFT - 1))%NUM_MUESTRAS_PERIODO_10HZ]));					//	Rellenamos sumaSeno[i] con los valores correspondientes a cada frecuencia 
		sumaCoseno[i] += (dato * (sinusoide10Hz[((pasos[i] * (contadorDFT - 1)) + NUM_MUESTRAS_PERIODO_10HZ/4)%NUM_MUESTRAS_PERIODO_10HZ]));	//	Rellenamos sumaCoseno[i] con los valores correspondientes a cada frecuencia 
	}
	if (contadorDFT == n_dft) {															// Si hemos hecho todas las interrupciones necesarias
		contadorDFT = 0;															//	Reiniciamos contadorDFT
		for (j = 0; j < n_frecs; j++) {														//	Para cada frecuencia
			sumaSeno[j] >>= 10;														// 		Desescalado de 1024 (mover a la derecha 10 bits) para que no se produzcan desbordamientos
			sumaCoseno[j] >>= 10;														// 		Desescalado de 1024 (mover a la derecha 10 bits) para que no se produzcan desbordamientos
			S_out[j] = busquedaBinaria((sumaCoseno[j] * sumaCoseno[j]) + (sumaSeno[j] * sumaSeno[j]));					// 		Almacenamos en S_out[j] el módulo cuantificado de la DFT de cada frecuencia	
			if (j == 0) {
				dft_lcd = S_out[0];
				frec_lcd = 0;
			}
			if (j > 0 && S_out[j] > dft_lcd) {
				dft_lcd = S_out[j];
				frec_lcd = 10 * pasos[j];
			}
		}
	}
	
	/*	Cada vez que contadorS_out sea igual a t_comp (8) reseteamos contadorS_out a 0, y si hemos sacado la última frecuencia (f=20), procedemos a volver
		a empezar, sacando la primera (f=1). Y cuando contadorS_our sea 0, sacamos por el DAC S_out[f], el cual se visualizará en el osciloscopio.
	*/
	if (contadorS_out == t_comp) {											// Si han transcurrido t_comp segundos
		contadorS_out = 0;											// 	Reseteamos contadorS_out
		if (f == n_frecs) {											// 	Si hemos contado todas las frecuencias
			f = 0;												// 		Reset de f (índice de la componente de frecuencia a visualizar)
		}
	}
	if (contadorS_out == 0) {											// Si contadorS_out es 0
		DAC_dato(S_out[f]);											// 	Sacamos S_out[f] por el DAC 
		f++;													// 	Incrementamos f
	}
	contadorS_out++;												// Incrementamos contadorS_out++;

	/*	Usamos set16_S para sacar un 1 cada t_barrido y así resetear la rampa 
		Cuando contadorReset sea 0, sacamos un 0 por una de las salidas digitales del ENT2004CF, y cuando contadorReset sea 160, sacamos un 1 por esa salida
		digital, y reseteamos contadorReset
	*/
	if (contadorReset == 0) {											// Si contadorReset es 0
		salida = salida & 0xFFEF;                                       // Realizamos un AND binario a salida para no interferir en el puerto. 
		set16_puertoS(salida);											// 	Sacamos un 0 por la salida digital
	}
	contadorReset++;												// Incrementamos contadorReset
	if (contadorReset == t_barrido) {										// Si ha pasado un tiempo t_barrido
	        salida = salida | 0x0010;                                    //Realizamos un OR binario a salida para no interferir en el puerto
		set16_puertoS(salida);											// 	Sacamos un 1 por la salida digital
		contadorReset = 0;											// 	Reseteamos contadorReset
	}

//	if (mbar_readShort(MCFSIM_TER0) & BORRA_REF) {
//		static int flag = 0;
//		if (!flag) {
//			flag = 1;
//			output("MAL\n");
//		}
//	} 
}

//------------------------------------------------------
// void rutina_int1(void)
//
// Descripción:
//   Función de atención a la interrupción para la
//   interrupción externa.
//------------------------------------------------------
void rutina_int1(void) {
	if (scroll > 0) {
		frecuencia_a = frecuencia_;
		dft_a = dft_;
	}

	output("He detectado un flanco de subida\r\n");									// Imprime el mensaje de la interrupción
	LCD_reset();
	LCD_init();
	LCD_inst(CLR_DISP);												// Limpiamos display
	LCD_inst(LIN_1LCD);												// Movemos el cursor a la 1ª línea
	frecuencia = "f[Hz]=";
	dft = "|H(jf)|=";
	
	frecuencia_ = numeroarray(frec_lcd);
	while(*frecuencia) {												// Imprime el mensaje frecuencia en el display
		LCD_dato(*frecuencia++); 										// carácter a carácter
		retardo(RET_50MS);											// Mantenemos el mensaje 1 segundo
	}
	while(*frecuencia_) {												// Imprime la frecuencia en el display
		LCD_dato(*frecuencia_++); 										// carácter a carácter
		retardo(RET_50MS);											// Mantenemos el mensaje 1 segundo
	}
	
	LCD_inst(LIN_2LCD);												// Movemos el cursor a la 2ª línea
	dft_ = numeroarray(dft_lcd);
	while(*dft) {													// Imprime el mensaje módulo de la dft en el display
		LCD_dato(*dft++); 											// carácter a carácter
		retardo(RET_50MS);											// Mantenemos el mensaje 1 segundo
	}
	while(*dft_) {													// Imprime el módulo de la dft en el display
		LCD_dato(*dft_++); 											// carácter a carácter
		retardo(RET_50MS);											// Mantenemos el mensaje 1 segundo
	}
	scroll++;
	mbar_writeLong(MCFSIM_ICR1, 0xD888C888);									// Al ser interrupción externa debemos volverla a activar
}

//------------------------------------------------------
// void __init(void)
//
// Descripción:
//   Función por defecto de inicialización del sistema
//------------------------------------------------------
void __init(void) {
	mbar_writeByte(MCFSIM_PIVR,V_BASE);										// Fija comienzo de vectores de interrupción en V_BASE.

	ACCESO_A_MEMORIA_LONG(DIR_VTMR0)= (ULONG)_prep_TOUT0; 								// Escribimos la dirección de la función para TMR0
	ACCESO_A_MEMORIA_LONG(DIR_VINT1) = (ULONG) _prep_INT1;								// Escribimos la dirección de la función para INT1

	mbar_writeShort(MCFSIM_TMR0, (PRESCALADO-1)<<8|0x3D);								// TMR0: PS=1-1=0 CE=00 OM=1 ORI=1 FRR=1 CLK=10 RST=1
	mbar_writeShort(MCFSIM_TCN0, 0x0000);										// Ponemos a 0 el contador del TIMER0
	mbar_writeShort(MCFSIM_TRR0, CNT_INT1);										// Fijamos la cuenta final del contador
	
	mbar_writeLong(MCFSIM_PITR, mbar_readLong(MCFSIM_PITR) | 0x80000000);						// Configuramos INT1 para que se active con flanco de subida
	mbar_writeLong(MCFSIM_ICR1, 0xD888C888);									// Marca la interrupción  como no pendiente y de nivel 4
	
	DAC_ADC_init();													// Inicialización DAC_ADC
	LCD_reset();													// Reset del LCD
	LCD_init();													// Inicialización del display
	output("____________________\r\n\r\nCOMIENZA EL PROGRAMA\r\n____________________\r\n\r\n");			// Imprime el mensaje de comienzo del programa
	output("Pulse F para acceder al menú\r\n\r\n");									// Imprime el mensaje de como abrir el menú
	sti();														// Habilitamos interrupciones
}

//------------------------------------------------------
// void bucleMain(void)
//
// Descripción:
//   Función del programa principal
//------------------------------------------------------
void bucleMain(void) {
	if (teclado() == 'F') {
		output("	Pulse 1 para cambiar el número de frecuencias\r\n");
		output("	Pulse 2 para cambiar la frecuencia mínima\r\n");
		output("	Pulse 3 para cambiar la frecuencia máxima\r\n");
		output("	Pulse 4 para cambiar el número de muestras para la DFT\r\n");
		output("	Pulse 5 para cambiar el tiempo de barrido\r\n");		
		output("	Pulse 6 para elegir el tipo de distribución de las frecuencias\r\n");
		output("	Pulse 7 para mostrar los datos anteriores del LCD\r\n");
		output("	Pulse cualquier otro botón para salir\r\n\r\n");
		
		// Escribimos en el display la tecla pulsada
		switch (teclado()) {
			case '1':															// Si pulsamos 1
				output("		Introduzca número de frecuencias (máx: 30)\r\n");						//	Imprime mensaje
				n_frecs = getCadTeclado(cadena);											//	Lee una cadena del teclado y la asigna a n_frecs
				if (n_frecs > 30) {													//	Limita el valor máximo
					n_frecs = 30;
				}
				t_comp = t_barrido/n_frecs;
				if (escala == 1) {
					salto = (0.1 * (f_max - f_min)/(n_frecs - 1));									// 	"salto" es la distancia entre frecuencias
					for (k = 1; k < n_frecs - 1; k++) {										// 	Para cada frecuencia
						pasos[k] = (pasos[0] + k * salto);									// 		Rellenamos el array pasos con su valor correspondiente
					}
				} else {
					if (n_frecs < 7) {
						output("			Para la distribución logarítmica, el mínimo número de frecuencias es de 7 frecuencias: n_frecs se fijará en 7 frecuencias\r\n");
						n_frecs = 7;
						t_comp = t_barrido/7;
					}
					pasoLogs = logs[n_frecs - 7];
					pasos[0] = f_min * 0.1;
					pasos[n_frecs - 1] = f_max * 0.1;
					for (i = 1; i < n_frecs; i++) {
						pasos[i] = f_min * 0.1 * pasoLogs;
						pasoLogs *= pasoLogs;
					}
				}
				output("		Número de frecuencias: ");
				outNum(10,n_frecs,0);													//	Imprime el número de frecuencias
				output(" frecuencias\r\n\r\n");												//	Retorno de carro y salto de línea
				break;
			case '2':															// Si pulsamos 2
				output("		Introduzca la frecuencia mínima (mín: 50 Hz, máx: 1971 Hz)\r\n");				//	Imprime mensaje
				f_min = getCadTeclado(cadena);												//	Lee una cadena del teclado y la asigna a f_min
				if (f_min > 1971) {													//	Limita el valor máximo
					f_min = 1971;
				} else if (f_min < 50) {												//	Limita el valor mínimo
					f_min = 50;
				}
				if (f_max < f_min + n_frecs) {												//	Limita diferencia entre f_min y f_max
					f_min = f_max - n_frecs;
				}
				pasos[0] = f_min * 0.1;													// 	La primera posición de pasos[] es la frecuencia mínima entre 10
				salto = (0.1 * (f_max - f_min)/(n_frecs - 1));										// 	"salto" es la distancia entre frecuencias
				for (k = 1; k < n_frecs - 1; k++) {											// 	Para cada frecuencia
					pasos[k] = (pasos[0] + k * salto);										// 		Rellenamos el array pasos con su valor correspondiente
				}
				output("		Frecuencia mínima: ");	
				outNum(10,f_min,0);													// 	Imprime el valor de la frecuencia mínima
				output(" Hz\r\n\r\n");													// 	Retorno de carro y salto de línea
				break;
			case '3':															// Si pulsamos 3
				output("		Introduzca la frecuencia máxima (mín: 79 Hz, máx: 2000 Hz)\r\n");				// 	Imprime mensaje
				f_max = getCadTeclado(cadena);												// 	Lee una cadena del teclado y la asigna a f_max
				if (f_max > 2000) {													//	Limita el valor máximo
					f_max = 2000;
				} else if (f_max < 79) {												//	Limita el valor mínimo
					f_max = 79;
				}
				if (f_max < f_min + n_frecs) {												//	Limita diferencia entre f_min y f_max
					f_max = f_min + n_frecs;
				}
				pasos[n_frecs - 1] = f_max * 0.1;											// 	La última posición de pasos[] es la frecuencia máxima entre 10
				salto = (0.1 * (f_max - f_min)/(n_frecs - 1));										//	 "salto" es la distancia entre frecuencias
				for (k = 1; k < n_frecs - 1; k++) {											// 	Para cada frecuencia
					pasos[k] = (pasos[0] + k * salto);										// 		Rellenamos el array pasos con su valor correspondiente
				}
				output("		Frecuencia máxima: ");	
				outNum(10,f_max,0);													// 	Imprime el valor de la frecuencia máxima
				output(" Hz\r\n\r\n");													// 	Retorno de carro y salto de línea
				break;			
			case '4':															// Si pulsamos 4
				output("		Introduzca el número de muestras para la DFT (máx: 80)\r\n");					// 	Imprime mensaje
				n_dft = getCadTeclado(cadena);												// 	Lee una cadena del teclado y la asigna a n_dft
				if (n_dft > 80) {													//	Limita el valor máximo
					n_dft = 80;
				}
				output("		Número de muestras: ");
				outNum(10,n_dft,0);													// 	Imprime el número de muestras para la DFT
				output(" muestras\r\n\r\n");												// 	Retorno de carro y salto de línea
				break;
			case '5':															// Si pulsamos 5
				output("		Introduzca el tiempo de barrido (máx: 40 ms)\r\n");						// 	Imprime mensaje
				t_barrido = 4 * getCadTeclado(cadena);											// 	Lee una cadena del teclado, multiplica el número obtenido de la cadena por 4 y la asigna a t_barrido
				if (t_barrido > 160) {													//	Limita el valor máximo
					t_barrido = 160;
				}
				t_comp = t_barrido/n_frecs;												//	Calcula el tiempo de componente
				output("		Tiempo de barrido: ");
				outNum(10,t_barrido * 0.25,0);												// 	Imprime el tiempo de barrido
				output(" ms\r\n\r\n");													// 	Retorno de carro y salto de línea
				break;
			case '6':															// Si pulsamos 6
				output("		Pulse 1 para distribución lineal\r\n		Pulse 2 para distribución logarítmica");	//	Imprime mensaje
				switch (teclado()) {
					case '1':
						output("\r\n			1\r\n");
						escala = 1;
						salto = (0.1 * (f_max - f_min)/(n_frecs - 1));								// 	"salto" es la distancia entre frecuencias
						for (k_lin = 1; k_lin < n_frecs - 1; k_lin++) {								// 	Para cada frecuencia
							pasos[k_lin] = (pasos[0] + k_lin * salto);							// 		Rellenamos el array pasos con su valor correspondiente
						}
						output("		Distribución de frecuencias: lineal");
						break;
					case '2':
						output("\r\n			2\r\n");
						escala = 2;
						if (n_frecs < 7) {
							output("			Para la distribución logarítmica, el mínimo número de frecuencias es de 7 frecuencias: n_frecs se fijará en 7 frecuencias\r\n");
							n_frecs = 7;
							t_comp = t_barrido/7;
						}
						output("			Para la distribución logarítmica, la frecuencia mínima se fijará a 100 Hz y la máxima a 1900 Hz\r\n");
						f_min = 100;
						f_max = 1900;
						pasoLogs = logs[n_frecs - 7];
						pasos[0] = f_min * 0.1;
						pasos[n_frecs - 1] = f_max * 0.1;
						for (k_log = 1; k_log < n_frecs; k_log++) {
							pasos[k_log] = f_min * 0.1 * pasoLogs;
							pasoLogs *= pasoLogs;
						}
						output("		Distribución de frecuencias: logarítmica");
						break;
					default:
						output("");
				}
				output("\r\n\r\n");													// 	Retorno de carro y salto de línea
				break;
			case '7':															// Si pulsamos 7
				if (scroll > 0) {
					LCD_reset();
					LCD_init();
					LCD_inst(CLR_DISP);												// 	Limpiamos display
					LCD_inst(LIN_1LCD);												//	 Movemos el cursor a la 1ª línea
				
					while(*frecuencia) {												//	 Imprime el mensaje frecuencia en el display
						LCD_dato(*frecuencia++); 										//	 carácter a carácter
						retardo(RET_50MS);											//	 Mantenemos el mensaje 1 segundo
					}
					while(*frecuencia_a) {												//	 Imprime la frecuencia en el display
						LCD_dato(*frecuencia_a++);	 									// 	carácter a carácter
						retardo(RET_50MS);											// 	Mantenemos el mensaje 1 segundo
					}
				
					LCD_inst(LIN_2LCD);												//	 Movemos el cursor a la 2ª línea
					while(*dft) {													//	 Imprime el mensaje módulo de la dft en el display
						LCD_dato(*dft++); 											//	 carácter a carácter
						retardo(RET_50MS);											//	 Mantenemos el mensaje 1 segundo
					}
					while(*dft_a) {													//	 Imprime el módulo de la dft en el display
						LCD_dato(*dft_a++); 											// 	carácter a carácter
						retardo(RET_50MS);											//	 Mantenemos el mensaje 1 segundo
					}
				} else {
					output("No hay datos anteriores almacenados");
					err = "Error";
					LCD_reset();
					LCD_init();
					LCD_inst(CLR_DISP);												// 	Limpiamos display
					LCD_inst(LIN_1LCD);												//	 Movemos el cursor a la 1ª línea
				
					while(*err) {													//	 Imprime el mensaje de error en el display
						LCD_dato(*err++); 											//	 carácter a carácter
						retardo(RET_50MS);											//	 Mantenemos el mensaje 1 segundo
					}
				}
				output("\r\n\r\n");													// 	Retorno de carro y salto de línea
				break;	
			default:															// Si pulsamos cualquier otra tecla
				output("");														//	Sale del menú
		}
		output("\r\nPulse F para acceder al menú\r\n\r\n");											// Imprime el mensaje de como abrir el menú
	}
}

//------------------------------------------------------
// Definición de rutinas de atención a la interrupción
// Es necesario definirlas aunque estén vacías
void rutina_int2(void) {}
void rutina_int3(void) {}
void rutina_int4(void) {}
void rutina_tout1(void) {}
void rutina_tout2(void) {}
void rutina_tout3(void) {}