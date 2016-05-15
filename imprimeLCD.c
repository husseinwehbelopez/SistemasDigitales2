//------------------------------------------------------------------------------
// imprimeLCD.c
//
//   Funci�n para imprimir por el LCD.
//
// Autores: �scar V�zquez Bab�n y Hussein Wehbe L�pez
//------------------------------------------------------------------------------
#include "m5272lib.c"
#include "m5272lcd.c"

int tam;                                                               //n�mero de cifras del n�mero que se le pasa como par�metro
int p;                                                                 //Variable auxiliar. Es el tama�o menos uno
char array[4];                                                         // Array que devuelve la funci�n

//------------------------------------------------------
// char * numero array(int num)
//
// Descripci�n:
//   Convierte un int en un array
// Par�metros:
//   int num
//------------------------------------------------------
char * numeroarray(int num) {
	if (num > 1000) {                                                                         
		tam = 4;                                                        //Si el n�mero es mayor que 1000 , el n�mero de cifras es 4
	} else if (num > 100) {
		tam = 3;                                                        //Si el n�mero es mayor que 100 , el n�mero de cifras es 3
		array[3] = ' ';                                                 // Y la posici�n 3 del array que se devuelve es un espacio en blanco
	} else if (num > 10) {
		tam = 2;                                                        //Si el n�mero es mayor que 10 , el n�mero de cifras es 2
		array[2] = ' ';                                                 // Y la posici�n 2 del array que se devuelve es un espacio en blanco
		array[3] = ' ';                                                 // Y la posici�n 3 del array que se devuelve es un espacio en blanco
	} else {                                                            //En el resto de casos:
		tam = 1;                                                        //El n�mero tendr� 1 cifra                                                                                         
		array[1] = ' ';                                                 //Y las posiciones 1,2,3 del array ser�n espacios en blanco
		array[2] = ' ';
		array[3] = ' ';
	}
	p = tam - 1;                                                       //Al n�mero de cifras le restamos uno 
	do {                                                              
		array[p] = num%10 + 48;                                        //En la posici�n 'p' introducimos el resto de dividir el n�mero entre 10 y lo pasamos a tipo char
		num = num * 0.1;                                               // Multiplicamos el n�mero por 0.1
		p--;                                                           //Reducimos 'p' para pasar a una posici�n anterior del array
	} while (num !=0);                                               //Mientras que num no sea 0
	return array;
}