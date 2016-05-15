//------------------------------------------------------------------------------
// imprimeLCD.c
//
//   Función para imprimir por el LCD.
//
// Autores: Óscar Vázquez Babón y Hussein Wehbe López
//------------------------------------------------------------------------------
#include "m5272lib.c"
#include "m5272lcd.c"

int tam;                                                               //número de cifras del número que se le pasa como parámetro
int p;                                                                 //Variable auxiliar. Es el tamaño menos uno
char array[4];                                                         // Array que devuelve la función

//------------------------------------------------------
// char * numero array(int num)
//
// Descripción:
//   Convierte un int en un array
// Parámetros:
//   int num
//------------------------------------------------------
char * numeroarray(int num) {
	if (num > 1000) {                                                                         
		tam = 4;                                                        //Si el número es mayor que 1000 , el número de cifras es 4
	} else if (num > 100) {
		tam = 3;                                                        //Si el número es mayor que 100 , el número de cifras es 3
		array[3] = ' ';                                                 // Y la posición 3 del array que se devuelve es un espacio en blanco
	} else if (num > 10) {
		tam = 2;                                                        //Si el número es mayor que 10 , el número de cifras es 2
		array[2] = ' ';                                                 // Y la posición 2 del array que se devuelve es un espacio en blanco
		array[3] = ' ';                                                 // Y la posición 3 del array que se devuelve es un espacio en blanco
	} else {                                                            //En el resto de casos:
		tam = 1;                                                        //El número tendrá 1 cifra                                                                                         
		array[1] = ' ';                                                 //Y las posiciones 1,2,3 del array serán espacios en blanco
		array[2] = ' ';
		array[3] = ' ';
	}
	p = tam - 1;                                                       //Al número de cifras le restamos uno 
	do {                                                              
		array[p] = num%10 + 48;                                        //En la posición 'p' introducimos el resto de dividir el número entre 10 y lo pasamos a tipo char
		num = num * 0.1;                                               // Multiplicamos el número por 0.1
		p--;                                                           //Reducimos 'p' para pasar a una posición anterior del array
	} while (num !=0);                                               //Mientras que num no sea 0
	return array;
}