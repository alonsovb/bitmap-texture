
#include <stdio.h>
#include <stdlib.h>
#include "bmpFile.h"
#include "textura.h"


///////////////////////////////// Variables ////////////////////////////////////
int pixelX;
int pixelY;
unsigned char **matriz_final;
int COX = 256;
int COY = 256;
float matrizCO[256][256];
int tam = 5;
unsigned char ventana[5][5];
int posicion[2];
unsigned char **matriz_leida;

////////////////////////////// Manipulacion de matriz //////////////////////////

// Funcion que limpia la matriz de coocurrencia despues de recorrer la
// ventana de 5x5
void limpiar_matriz() {
    int x;
    int y = 0;

    while (y < COY) {
        x = 0;
        while (x < COX) {
            matrizCO[x][y] = 0;
            x++;
        }
        y++;
    }
}


// Funcion que obtiene los valores para sumar en la matriz de coocurrencia
// saca las combinaciones con posiciones 1 y 2. Obtine los 8 vecinos.
void vecinos(int x, int y, int minimoX, int maximoX, int minimoY, int maximoY) {
    int pos1 = (int) ventana[x][y];
    int pos2;

    if (x < maximoX) {
        pos2 = (int) ventana[x + 1][y];
        matrizCO[pos1][pos2]++;
    }
    if (x > minimoX) {
        pos2 = (int) ventana[x - 1][y];
        matrizCO[pos1][pos2]++; 
    }
    if (y < maximoY) {
        pos2 = (int) ventana[x][y + 1];
        matrizCO[pos1][pos2]++;
    }
    if (y > minimoY) {
        pos2 = (int) ventana[x][y - 1];
        matrizCO[pos1][pos2]++;
    }
    if ((x > minimoX) && (y > minimoY)) {
        pos2 = (int) ventana[x - 1][y - 1];
        matrizCO[pos1][pos2]++;
    }
    if ((x < maximoX) && (y < maximoY)) {
        pos2 = (int) ventana[x + 1][y + 1];
        matrizCO[pos1][pos2]++;
    }
    if ((x < maximoX) && (y > minimoY)) {
        pos2 = (int) ventana[x + 1][y - 1];
        matrizCO[pos1][pos2]++;
    }
    if ((x > minimoX) && (y < maximoY)) {
        pos2 = (int) ventana[x - 1][y + 1];
        matrizCO[pos1][pos2]++;
    }
}

// Funcion que recorre la ventana que se crea de 5 x 5, recorre cada posicion de 
// de la ventana.
void Rec_Ventana() {
    int x;
    int y = 0;

    while (y < tam) {
        x = 0;
        while (x < tam) {
            vecinos(x, y, 0, tam - 1, 0, tam - 1);
            x++;
        }
        y++;
    }
}

////////////////////////////// homogeneidad ////////////////////////////////////

// Esta funcion normaliza la matriz de coocurrencia, dividiendo cada punto/posi-
// cion de la matriz entre 144.
void normalizarM() {
    int x;
    int y = 0;
 
    while (y < COY) {
        x = 0;
        while (x < COX) {
            float n = (matrizCO[x][y] + 0.0) / 144;
            matrizCO[x][y] = n;
            x++;
        }
        y++;
    }
}


// Despues de normalizada la M.C se aplica la funcion de homogeneidad para es-
// cribir en la matriz de salida o final.
void funcHomogeneidad() {
    int x;
    int y = 0;
    float resultado = 0.0;

    while (y < COY) {
        x = 0;
        while (x < COX) {
            float div = 1 + (x - y)*(x - y);
            float sum = matrizCO[x][y] / (float) div;
            resultado = resultado + sum;
            x++;
        }
        y++;
    }
    float rango;
    rango = resultado * (float) 255;
    rango = (float) (int) (rango + 0.5);
    int var_homo = (int) rango;
    x = posicion[0];
    y = posicion[1];
    matriz_final[x][y] = (unsigned char) var_homo;

}

////////////////////////////// Manipulacion imagen  ////////////////////////////

// Funcion que toma la imagen de entrada y la recorre para llenar la ventana de 
// 5x5 y hacer todo el recorrido ejemplo: primero lo hace [0-4][0-4] luego se 
//corre uno a la derecha por lo que la llena con [1-5][0-4].
void Llenar_Ventana() {
    int bitmapX = posicion[0] - tam / 2;
    int bitmapY = posicion[1] - tam / 2;
    int finalizaX = bitmapX + tam;
    int finalizaY = bitmapY + tam;
    int ventanaX = 0;
    int ventanaY = 0;

    while (bitmapY < finalizaY) {
        bitmapX = posicion[0] - tam / 2;
        ventanaX = 0;
        while (bitmapX < finalizaX) {
            ventana[ventanaX][ventanaY] = matriz_leida[bitmapX][bitmapY];
            bitmapX++;
            ventanaX++;
        }
        bitmapY++;
        ventanaY++;
    }

    Rec_Ventana();
    normalizarM();
    funcHomogeneidad();
    limpiar_matriz();
}

// Funcion que asigna la matriz que se lee, a una variable que se necesita en 
// varias funciones
void asignar(unsigned char **matriz) {
    // asigna memoria a la matriz leida usando malloc. se utilza para almacenar 
    // la matriz de la imagen que se lee.
    matriz_leida = (unsigned char **) malloc(pixelX * sizeof (unsigned char *));
    int i = 0;
    for (; i < pixelX; i++) {
        matriz_leida[i] = (unsigned char **) malloc(pixelY * sizeof (unsigned char *));
    }
    // asigna memoria a la matriz final donde queda el resultado depues de 
    // aplicar homogeneidad.
    matriz_final = (unsigned char **) malloc(pixelX * sizeof (unsigned char *));
    i = 0;
    for (; i < pixelX; i++) {
        matriz_final[i] = (unsigned char **) malloc(pixelY * sizeof (unsigned char *));
    }

    int y = 0;
    int x;
    while (y < pixelY) {
        x = 0;
        while (x < pixelX) {
            matriz_leida[x][y] = matriz[x][y];
            x++;
        }
        y++;
    }
}

// Es la funcion principal donde se recibe lo que se lee de la imagen original,
// llama a funciones para desarrollar el proceso y por ultimo devuelve la matriz
// final que es la escrita en el archivo.
unsigned char** principal(unsigned char **matriz, char *archivo) {

    printf("Procesando imagen...\n");
    pixelX = getHeight(archivo);
    pixelY = getWidth(archivo);
    
    asignar(matriz);

    int y = tam / 2;
    while (y < pixelY - (tam / 2)) {
        int x = tam / 2;
        while (x < pixelX - (tam / 2)) {

            posicion[0] = x;
            posicion[1] = y;
            Llenar_Ventana();
            x++;
        }
        y++;
        // Mostrar el porcentaje completado en consola
        printf("Porcentaje completado: \t%i%%\n", y * 100 / (pixelY - (tam / 2)));
    }
    //Imprimir_Matrix_CO();
    return matriz_final;
}
