//------------------------------------------------------------------------------
// cadena.c
//
//   Lee del teclado matricial una cadena de caracteres terminada por retorno de carro,
//   y devuelve los n�meros de dicha cadena.
//
// Autores: Juan Manuel Montero, Rub�n San Segundo y Jos� Luis Pau
// Adaptado a C por: Javier Guill�n �lvarez
// Modificado por: �scar V�zquez Bab�n y Hussein Wehbe L�pez
//------------------------------------------------------------------------------
#include "teclado.c"

//------------------------------------------------------
// int getCadTeclado(char* cadena)
//
// Descripci�n:
//   Lee una cadena del teclado matricial, caracter a
//   caracter y la devuelve para que la use el
//   programa principal.
//
// Par�metros:
//   char* cadena
//     puntero al b�fer donde almacena la cadena le�da.
//------------------------------------------------------
int getCadTeclado(char* cadena) {
	int i = 0;													// Lleva la cuenta de la posici�n en el array
	char w;														// Variable donde se almacena cada caracter le�do del teclado
	int num = 0;													// N�mero que devuelve la funci�n
	int cifras = 0;	
	output("			");												// N�mero de cifras del n�mero
	do {														// Se ejecuta:
		w = teclado();												//	Lee el teclado matricial y almacena la tecla pulsada en w
		if (w <= '9' && w >= '0') {										//	Si la tecla pulsada es un n�mero
			if (i == 0 && w == '0') {									//		Si para la primera cifra intentamos poner un 0
				i--;											//			Se ignora la tecla pulsada
			} else {											//		Si para la primera cifra no intentamos poner un 0
				cifras++;										//			Aumenta el n�mero de cifras del n�mero
				cadena[i] = w;										// 			Almacena el caracter le�do en el array cadena
				outch(cadena[i]);									// 			Imprime en el terminal el caracter le�do
			}
		} else if (w != 'D' && w != 'E') {									//	Si la tecla pulsada no es un n�mero ni la tecla de borrar o de enter 
			i--;												//		Se ignora la tecla pulsada
		}
		if (w == 'D') { 											//	Si la tecla pulsada es la de borrar
			if (i > 0) {											//		Si no estamos en la primera posici�n del array
				cifras--;										//			Disminuye el n�mero de cifras del n�mero
				outch('\b');										//			Se borra el �ltimo n�mero imprimido en el terminal
				i -= 2;											//			Retrocede una posici�n en el array
			} else {											// 		Si estamos en la primera posici�n
				i--;											//			Se ignora la tecla pulsada
			}
		}
	} while ((w != 'E') && (++i < 4));										// Mientras no se pulse la tecla de enter y el n�mero tenga 4 cifras o menos

	output("\r\n");													// Se guardan e imprimen los caracteres de:
	cadena[i++] = '\r';												// - Retorno de carro
	cadena[i++] = '\n';												// - Cambio de l�nea
	cadena[i]   = '\0';												// - Fin de cadena
	
	if (cifras == 4) {												// Devuelve el n�mero si tiene 4 cifras
		num = 1000*(cadena[0] - 48) + 100*(cadena[1] - 48) + 10*(cadena[2] - 48) + (cadena[3] - 48);
		return num;
	}
	else if (cifras == 3) {												// Devuelve el n�mero si tiene 3 cifras
		num = 100*(cadena[0] - 48) + 10*(cadena[1] - 48) + (cadena[2] - 48);
		return num;
	}
	else if (cifras == 2) {												// Devuelve el n�mero si tiene 2 cifras
		num = 10*(cadena[0] - 48) + (cadena[1] - 48);
		return num;
	}
	else if (cifras == 1) {												// Devuelve el n�mero si tiene 1 cifra
		num = (cadena[0] - 48);
		return num;
	}
	else if (cifras == 0) {												// Devuelve un 1 si no introducimos nada
		return 1;
	}
	return 1;
}