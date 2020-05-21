#include <stdio.h>
#include <stdlib.h>
#include <math.h>


//Se genera una estructura con la cantidad de genes, la matriz para el estado de los genes,
//la cantidad de procesos(vertices) y la matriz que contiene el estado de los procesos.
typedef struct grafo{
	int genes;
	int** matrizgen;
	int vertices;
	int** matriz;
}grafo;

//Funcion que Libera memoria dinamica utilizada
void liberarMemoria(int** matriz, int largo){
	int i;
	for(i=0;i<largo;i++){
		free(matriz[i]);//liberar matriz dinamica desde adentro hacia afuera
		matriz[i] = NULL;
	}
	free(matriz);
	matriz = NULL;
}

//Crea Matriz
int** crearMatriz(int n, int n2){
	int **matriz = (int**)malloc(sizeof(int*)*n);
	int i, j;
	if(matriz){
		for(i = 0; i < n; i++){
			matriz[i] = (int*)calloc(n2, sizeof(int));
			if(!matriz[i]){
				for(j = 0; j < i; j++){
					free(matriz[i]);
				}
				free(matriz);
				return NULL;
			}
		}
	}
	return matriz;
}

//Lee los archivos y genera las matrices para los procesos y genes
grafo leerGrafo(char* nombreArchivo, char* nombreArchivo2){
	int fil = -1, col = 0;
	char aux;
	//Abre y lee dos archivos externos
	FILE *archivo = fopen(nombreArchivo, "r");
	FILE *archivo2 = fopen(nombreArchivo2, "r");
	grafo g;
	//Si los archivos no los encuentra finaliza la ejecucion
	if(!archivo || !archivo2){
		printf("Error en la apertura del archivo\n");
		g.matriz = NULL;
		g.vertices = 0;
		return g;
	}
	
	//Verifica la integridad de los datos
	while(!feof(archivo)){
		aux = fgetc(archivo);
		//Verifica que los datos de entradas sean solo numeros, espacio, salto de linea y P
		if((aux < 48 || aux > 57) && (aux != 10 && aux != 32 && aux != -1 && aux != 71  && aux != 13 && aux != 80)){
			printf("los Datos en el Fichero estan corruptos\n");
			//Si hay adatos no validos se retorna matriz nula
			g.matriz = NULL;
			return g;
		}
	}	
	rewind(archivo);
		//Verifica la integridad de los datos
	while(!feof(archivo2)){
		aux = fgetc(archivo2);
		//Verifica que los datos de entradas sean solo numeros, espacio, salto de linea y P
		if((aux < 48 || aux > 57) && (aux != 10 && aux != 32 && aux != -1 && aux != 71  && aux != 13 && aux != 80)){
			printf("los Datos en el Fichero estan corruptos\n");
			//Si hay adatos no validos se retorna matriz nula
			g.matrizgen = NULL;
			return g;
		}
	}
	rewind(archivo2);
	//Obtiene el largo de genes y procesos(vertices)
	fscanf(archivo, "%d", &g.vertices);
	fscanf(archivo2, "%d", &g.genes);
	g.matriz = crearMatriz(g.vertices, g.vertices);
	g.matrizgen = crearMatriz(g.genes, g.vertices);
	if(!g.matriz){
		fclose(archivo);
		fclose(archivo2);
		g.vertices = 0;
		return g;
	}
	//Lee cada caracter del archivo
	while(!feof(archivo)){
		aux = fgetc(archivo);
		if(aux == 10){
			fil++;
		}
		//valida cada posicion de los procesos en la matriz
		if(aux >= 48 && aux <= 57){
			g.matriz[fil][aux-49] = 1;
		}
	}
	fclose(archivo);
	fil = -1;
	//Lee cada caracter del archivo
	while(!feof(archivo2)){
		aux = fgetc(archivo2);
		if(aux == 10){
			fil++;
			col = 0;
		}
		if(aux == 32)
			col++;
		//valida cada posicion de los genes en la matriz gen en la posicion de cada proceso
		if(aux >= 48 && aux <= 57){
			if(col != 0 && fil < g.genes){
				g.matrizgen[fil][aux-49] = 1;
			}
			if(fil >= g.genes){
				printf("El archivo esta corrupto");
				g.matriz = NULL;
				g.matrizgen = NULL;
				return g;
			}
		}
	}
	fclose(archivo2);
	//Retorna la estructura
	return g;
}

//Busca el ancestro comun mas cercano para ambos procesos
int ancestroComun(grafo g, int PA, int PB){
	int i, j, comun = 0, aux = 0;
	//verifica todos los ancestros del proceso A
	for(i = 0; i < g.vertices; i++){
		if(g.matriz[PA][i] != 0){
			//verifica todos los ancestros del proceso B
			for(j = 0; j < g.vertices; j++){
				if(g.matriz[PB][j] != 0){
					//si ambos procesos tienen un ancestro en comun se almacena el ancestro
					if(i == j){
						//pueden tener muchos ancestros en comun pero el mas cercano sera el valido
						if(comun < i+1);
							comun = i+1;
					}
					if(PB != j){
						//para verificar los padres de los padres (abuelos) de los procesos (llamada recursiva)
						aux = ancestroComun( g, PA, j);
						if(comun < aux)
							comun = aux;
					}
				}
			}
			if(PA != i){
				//para verificar los padres de los padres (abuelos) de los procesos (llamada recursiva)
				aux = ancestroComun( g, i, PB);
				if(comun < aux)
					comun = aux;
				}
		}
	}
	//retorna el ancestro en comun
	return comun;
}

//Calcula la distancia entre dos procesos
int distancia(grafo g, int NA,int NB,int dist){
	int i, j, aux = 0;
	//si el proceso A es igual al proceso B, retorna la distancia entre ambos procesos
	if(NA == NB){
		return dist;
	}
	else{
		for(i = 0; i < g.vertices; i++){
			//verifica si el siguiente proceso de A esta conectado con el proceso de B
			if(g.matriz[NA][i] != 0 && dist < g.vertices && aux == 0){
				//llama a la funcion recursivamente con el siguiente nodo (padre) para ver si esta conectado
				//con el nodo B agregandole 1+ a la distancia
				aux = distancia(g, i, NB, dist +1);
			}
		}
	}
	//retorna la distancia entre ambos procesos
	return aux;
}

//Calcula la similitud mediante Wu-Palmer
double simWP(grafo g, int NA, int NB){
	int comun = ancestroComun(g, NA, NB);
	double D1 = distancia(g, NA, comun -1, 0);
	double D2 = distancia(g, NB, comun -1, 0);
	double D3 = distancia(g, comun -1, 0, 0);
	return ((2*D3)/(D1 + D2 + (2*D3)));
}

//Calcula la similitud mediante Leacok-Chorodow
double simLC(grafo g, int NA, int NB){
		int comun = ancestroComun(g, NA, NB);
		double D1 = distancia(g, NA, comun -1, 0);
		double D2 = distancia(g, NB, comun -1, 0);
		double profundidad = distancia(g, g.vertices -1, 0, 0) +1;
		return (-log10((D1 + D2 + 1)/(2 * profundidad)));
}

//Calcula la similitud entre dos Genes
void simg(grafo g, int Gx, int Gy){
	int i, j, cont = 0;
	double resultWP = 0, resultLC = 0;
	//buscaen que proceso se encuentra el GenX
	for(i = 0; i < g.vertices; i++){
		if(g.matrizgen[Gx][i] != 0){
			//buscaen que proceso se encuentra el GenY
			for(j = 0; j < g.vertices; j++){
				if(g.matrizgen[Gy][j] != 0){
					cont++;
					//calcula la similitud de WP y LC
					resultWP = resultWP + simWP(g, i, j);
					resultLC = resultLC + simLC(g, i, j);
				}
			}
		}
	}
	//Imprime el resultado de WP y LC
	printf("Similitud de genes Wu-Palmer: %f\n",resultWP/cont);
	printf("Similitud de genes Leacok-Chorodow: %f\n",resultLC/cont);
	return;
}

