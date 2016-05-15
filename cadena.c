//------------------------------------------------------------------------------
// cadena.c
//
//   Lee del teclado matricial una cadena de caracteres terminada por retorno de carro,
//   y devuelve los números de dicha cadena.
//
// Autores: Juan Manuel Montero, Rubén San Segundo y José Luis Pau
// Adaptado a C por: Javier Guillén Álvarez
// Modificado por: Óscar Vázquez Babón y Hussein Wehbe López
//------------------------------------------------------------------------------
#include "teclado.c"

//------------------------------------------------------
// int getCadTeclado(char* cadena)
//
// Descripción:
//   Lee una cadena del teclado matricial, caracter a
//   caracter y la devuelve para que la use el
//   programa principal.
//
// Parámetros:
//   char* cadena
//     puntero al búfer donde almacena la cadena leída.
//------------------------------------------------------
int getCadTeclado(char* cadena) {
	int i = 0;													// Lleva la cuenta de la posición en el array
	char w;														// Variable donde se almacena cada caracter leído del teclado
	int num = 0;													// Número que devuelve la función
	int cifras = 0;	
	output("			");												// Número de cifras del número
	do {														// Se ejecuta:
		w = teclado();												//	Lee el teclado matricial y almacena la tecla pulsada en w
		if (w <= '9' && w >= '0') {										//	Si la tecla pulsada es un número
			if (i == 0 && w == '0') {									//		Si para la primera cifra intentamos poner un 0
				i--;											//			Se ignora la tecla pulsada
			} else {											//		Si para la primera cifra no intentamos poner un 0
				cifras++;										//			Aumenta el número de cifras del número
				cadena[i] = w;										// 			Almacena el caracter leído en el array cadena
				outch(cadena[i]);									// 			Imprime en el terminal el caracter leído
			}
		} else if (w != 'D' && w != 'E') {									//	Si la tecla pulsada no es un número ni la tecla de borrar o de enter 
			i--;												//		Se ignora la tecla pulsada
		}
		if (w == 'D') { 											//	Si la tecla pulsada es la de borrar
			if (i > 0) {											//		Si no estamos en la primera posición del array
				cifras--;										//			Disminuye el número de cifras del número
				outch('\b');										//			Se borra el último número imprimido en el terminal
				i -= 2;											//			Retrocede una posición en el array
			} else {											// 		Si estamos en la primera posición
				i--;											//			Se ignora la tecla pulsada
			}
		}
	} while ((w != 'E') && (++i < 4));										// Mientras no se pulse la tecla de enter y el número tenga 4 cifras o menos

	output("\r\n");													// Se guardan e imprimen los caracteres de:
	cadena[i++] = '\r';												// - Retorno de carro
	cadena[i++] = '\n';												// - Cambio de línea
	cadena[i]   = '\0';												// - Fin de cadena
	
	if (cifras == 4) {												// Devuelve el número si tiene 4 cifras
		num = 1000*(cadena[0] - 48) + 100*(cadena[1] - 48) + 10*(cadena[2] - 48) + (cadena[3] - 48);
		return num;
	}
	else if (cifras == 3) {												// Devuelve el número si tiene 3 cifras
		num = 100*(cadena[0] - 48) + 10*(cadena[1] - 48) + (cadena[2] - 48);
		return num;
	}
	else if (cifras == 2) {												// Devuelve el número si tiene 2 cifras
		num = 10*(cadena[0] - 48) + (cadena[1] - 48);
		return num;
	}
	else if (cifras == 1) {												// Devuelve el número si tiene 1 cifra
		num = (cadena[0] - 48);
		return num;
	}
	else if (cifras == 0) {												// Devuelve un 1 si no introducimos nada
		return 1;
	}
	return 1;
}