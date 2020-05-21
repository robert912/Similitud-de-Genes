#include <stdio.h>
#include <stdlib.h>
#include "add.h"

//funcion Principal
int main(){
	int continuar;
	char  Gx, Gy, aux;
	//lee los archivos y genera las matrices para los procesos y genes
	grafo g = leerGrafo("procesos.in","genes.in");
	if(!g.matriz || !g.matrizgen)
		return 0;	
	do{
		continuar = 0;
		do{
			printf("ingrese primer gen: ");
			scanf(" %c",&Gx);
		//verifica que se ingreso un Gen valido
		}while(Gx < 49 || Gx > g.genes + 48);
		do{
			printf("ingrese segundo gen: ");
			scanf(" %c",&Gy);
		//verifica que se ingreso un Gen valido
		}while(Gy < 49 || Gy > g.genes + 48);
		//llama a la funcion que calcula la similitud de WP y LC
		simg(g, Gx-49, Gy-49);
		printf("Si desea ingresar otro par de genes presione 1, en caso contrario presione cualquier tecla\n");
		scanf("%d",&continuar);
	}while(continuar == 1);
	//libera la memoria utilizada
	liberarMemoria(g.matriz, g.vertices);
	liberarMemoria(g.matrizgen, g.genes);
	return 0;
}
