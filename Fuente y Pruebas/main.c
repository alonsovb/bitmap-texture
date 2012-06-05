/* 
 * File:   main.c
 * Author: Alonso
 *
 * Created on 20 de septiembre de 2011, 04:54 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "bmpFile.h"
#include "textura.h"

/*
 * 
 */
int main(int argc, char** argv) {

    unsigned char **bitmap;
    char *inFile;
    if (argc > 1)
        inFile = argv[1];
    else {
        printf("Ingrese el nombre del archivo: ");
        scanf("%s", inFile);
        if (!fileExists(inFile)) {
            printf("El archivo especificado no se encuetra.\n");
            return 2;
        }
    }
    // Leer la imagen a una matriz en memoria
    bitmap = getBitmapMatrix(inFile);

    // Aplicar la funcion principal de procesamiento
    bitmap = principal(bitmap, inFile);

    // Guardar la matriz de resultado en un nuevo archivo
    setBitmapFile(bitmap, "salida.bmp", inFile);

    return (EXIT_SUCCESS);
}

