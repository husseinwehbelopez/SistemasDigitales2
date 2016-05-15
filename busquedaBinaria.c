//------------------------------------------------------------------------------
// busquedaBinaria.c
//
//   B�squeda binaria para escalar del m�dulo de la DFT
//
// Autores: �scar V�zquez Bab�n y Hussein Wehbe L�pez
//------------------------------------------------------------------------------
#define N_UMBRALES 32

//------------------------------------------------------
// int busquedaBinaria(int numero)
//
// Descripci�n:
//   Encuentra el valor que hay que sacar por
//   el DAC seg�n el valor del m�dulo de la DFT
//
// Par�metros:
//   int numero
//     m�dulo de la DFT
//------------------------------------------------------
int busquedaBinaria(int numero) {
	int contadorUmbrales;									// Contador que llegar� hasta 5 y se reiniciar�
	int izquierda = 0;									// Margen inferior de la b�squeda
	int derecha = N_UMBRALES - 1;								// Margen superior de la b�squeda
	int centro = (izquierda + derecha)/2;							// Centro (la media de izquierda y derecha)

	// Array con los distintos umbrales que puede alcanzar la se�al 
	static int umbrales[N_UMBRALES] = {63095,84924,114304,153849,207075,278715,375140,504923,679607,914724,1231182,1657123,2230422,3002059,4040653,5438559,
					7320085,9852544,13261133,17848959,24023991,32335339,43522083,58578997,78845006,106122250,142836338,192252044,258763624,348285572,468778561,686492401};
	// Array con los valores cuantificados de la se�al seg�n su valor. umbrales[i] se escalar� a adapt_DAC[i] 			
	static int adapt_DAC[N_UMBRALES] = {122,245,368,491,614,737,860,983,1105,1228,1351,1474,1597,1720,1843,1966,2088,2211,2334,2457,2580,2703,2826,2949,3072,3194,3317,3440,3563,3686,3809,3932};
   
	if (numero < umbrales[0]) return adapt_DAC[0];						// Si el n�mero que recibe busquedaBinaria es menor que el m�nimo valor de los umbrales, devuelve el valor m�nimo de los valores de adapt_DAC
	if (numero > umbrales[N_UMBRALES - 2]) return adapt_DAC[N_UMBRALES - 1];		// Si el numero que recibe busquedaBinaria es mayor que el segundo m�ximo valor de los umbrales, devuelve el valor m�ximo de los valores de adapt_DAC

	for (contadorUmbrales = 0; contadorUmbrales < 5; contadorUmbrales++) {			// El proceso de la b�squeda binaria dura 5 iteraciones
		if (numero < umbrales[centro]) {						// Si el n�mero que recibe busquedaBinaria es menor que el valor central del array umbrales[] 
			derecha = centro;							// El margen superior pasa a ser el valor previo de centro
			centro = (izquierda + derecha)/2;					// Recalculamos el nuevo valor de centro para el valor actualizado de derecha
		} else {									// Si el n�mero que recibe busquedaBinaria es mayor que el valor central del array umbrales[]
			izquierda = centro;							// El margen inferior pasa a ser el valor previo de centro
			centro = (izquierda + derecha)/2;					// Recalculamos el nuevo valor de centro para el valor actualizado de izquierda
		}		
	}
	contadorUmbrales = 0;									// Reseteamos contadorUmbrales
	return adapt_DAC[centro];								// Devolvemos el valor encontrado en el array de valores escalados para el valor que recibe la funci�n busquedaBinaria
}