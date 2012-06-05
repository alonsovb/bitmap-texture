#include <stdio.h>

#define bmpType 0x4d42

#pragma pack(2)

// Estructura para la cabecera de archivo
typedef struct {
    unsigned short type;
    unsigned long size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned long offsetbits;
} fileHead;

#pragma pack()

// Estructura para la cabecera de informacion
typedef struct {
    unsigned long size;
    unsigned long width;
    unsigned long height;
    unsigned short planes;
    unsigned short bitcount;
    unsigned long compression;
    unsigned long sizeimage;
    long xpelspermeter;
    long ypelspermeter;
    unsigned long colorsused;
    unsigned long colorsimportant;
} infoHead;

// Estructura para un pixel
typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char alpha;
} pixel;

// Buscar el ancho de la imagen de un archivo
int getWidth(char *fileName) {
    FILE *file;
    infoHead iHeader;

    // Abrir el archivo
    file = fopen(fileName, "rb");
    // Buscar y leer la cabecera de informacion
    fseek(file, sizeof (fileHead), SEEK_SET);
    fread(&iHeader, sizeof (infoHead), 1, file);

    fclose(file);

    // Retornar el ancho
    return iHeader.width;
}

// Buscar la altura de la imagen de un archivo
int getHeight(char *fileName) {
    FILE *file;
    infoHead iHeader;

    // Abrir archivo
    file = fopen(fileName, "rb");
    // Buscar la cabecera de informacion
    fseek(file, sizeof (fileHead), SEEK_SET);
    fread(&iHeader, sizeof (infoHead), 1, file);

    fclose(file);

    // Retornar la altura
    return iHeader.height;
}

// Leer la cabecera de informacion de un archivo
fileHead *getFileHeader(char *filename) {
    FILE *file; // Puntero al archivo de imagen
    fileHead *result; // Cabecera de archivo

    // Abrir el archivo
    file = fopen(filename, "rb");
    // Guardar espacio para la cabecera
    result = (fileHead *) malloc(sizeof (fileHead));
    // Buscar la cabecera en el archivo y leerla a memoria
    fread(result, sizeof (fileHead), 1, file);

    fclose(file);

    return result;
}

// Leer la cabecera de informacion de un archivo
infoHead *getInfoHeader(char *filename) {
    FILE *file;
    infoHead *result;

    // Abrir el archivo
    file = fopen(filename, "rb");
    // Guardar espacio para la cabecera
    result = (infoHead *) malloc(sizeof (infoHead));
    // Buscar la cabecera en el archivo y leerla a memoria
    fseek(file, sizeof (fileHead), SEEK_SET);
    fread(result, sizeof (infoHead), 1, file);

    fclose(file);

    return result;
}

// Leer la paleta de colores de un archivo
pixel *getPalette(char *fileName) {
    FILE *file;
    // Abrir el archivo
    file = fopen(fileName, "rb");
    // Buscar el inicio de los bytes de paleta
    fseek(file, sizeof (fileHead) + sizeof (infoHead), SEEK_SET);
    // Almacenar en memoria la paleta y devolverla como resultado
    pixel *result = (pixel *) malloc(sizeof (pixel) * 256);
    fread(result, sizeof (pixel) * 256, 1, file);

    fclose(file);

    return result;
}

unsigned char **getBitmapMatrix(char *fileName) {
    FILE *archivoBMP; // Puntero al archivo de imagen
    fileHead fHeader; // Cabecera de archivo
    infoHead iHeader; // Cabecera de informacion

    // Abrir el archivo en modo lectura binaria
    archivoBMP = fopen(fileName, "rb");

    // Mostrar un error si no se puede abrir el archivo
    if (archivoBMP == 0) {
        printf("El archivo no pudo ser abierto.\n");
        return NULL;
    }

    // Leer la cabecera del archivo
    fread(&fHeader, sizeof (fileHead), 1, archivoBMP);

    // Comprobar el tipo de archivo
    if (fHeader.type != bmpType) {
        printf("El archivo no tiene el formato correcto.\n");
        return NULL;
    }

    // Leer la cabecera con informacion del archivo
    fread(&iHeader, sizeof (iHeader), 1, archivoBMP);

    // Mover el puntero lector al inicio
    fseek(archivoBMP, fHeader.offsetbits, SEEK_SET);

    // Tamaño en bytes de cada fila de pixeles
    int rowSize = iHeader.width;
    int padding = 4 - (rowSize % 4);
    if (padding == 4) padding = 0;
    int rowIndex;

    // Matriz de resultado
    unsigned char **result =
            (unsigned char **) malloc(iHeader.height * sizeof (char *));

    // Leer cada fila de pixeles
    for (rowIndex = iHeader.height - 1; rowIndex >= 0; rowIndex--) {
        result[rowIndex] = (char *) malloc(rowSize);
        fread(result[rowIndex], sizeof (char) * rowSize, 1, archivoBMP);
        fseek(archivoBMP, padding, SEEK_CUR);
    }

    // Cerrar el archivo
    fclose(archivoBMP);

    return result;
}

// Almacena la matriz dada en un archivo BMP
void setBitmapFile(unsigned char **bitmap, char *outFile, char *inFile) {

    FILE *salidaBMP; // Puntero al archivo de imagen
    fileHead *fHeader; // Cabecera de archivo
    infoHead *iHeader; // Cabecera de informacion

    // Abrir archivo de salida
    salidaBMP = fopen(outFile, "wb");   

    // Copiar la cabecera de archivo y de informacion al archivo de salida
    fHeader = getFileHeader(inFile);
    fwrite(fHeader, sizeof (fileHead), 1, salidaBMP);

    iHeader = getInfoHeader(inFile);
    fwrite(iHeader, sizeof (infoHead), 1, salidaBMP);

    int n = 0; // Valor para rellenar nulos

     // Paleta en escala de grises
    int px;
    for (px = 0; px < 256; px++) {
        pixel actual;
        actual.red = px;
        actual.green = px;
        actual.blue = px;
        actual.alpha = 0x00;
        // Escribir cada valor a la paleta
        fwrite(&actual, sizeof (pixel), 1, salidaBMP);
    }

    // Buscar los valores de ancho y alto de la matriz
    int width = iHeader->width, height = iHeader->height;
    int padding = 4 - (iHeader->width % 4);
    if (padding == 4) padding = 0;

    // Buscar la posicion donde se inicia los bytes de imagen
    fseek(salidaBMP, fHeader->offsetbits, SEEK_SET);
    int y;
    // Escribir cada byte de la matriz en el archivo
    for (y = height - 1; y >= 0; y--) {
        fwrite(bitmap[y], width, 1, salidaBMP);
        fwrite(&n, padding, 1, salidaBMP);
    }

    fclose(salidaBMP);
}

// Comprueba si un archivo existe
int fileExists(char *fname) {
    FILE *file;
    if (file = fopen(fname, "r")) {
        fclose(file);
        return 1;
    }
    return 0;
}

