/* 
 * File:   textura.h
 * Author: sancho
 *
 * Created on 19 de septiembre de 2011, 07:12 PM
 */

unsigned char **principal(unsigned char **matriz, char *archivo);
void asignar(unsigned char **matriz);
void Llenar_Ventana();
void Rec_Ventana();
void vecinos(int x, int y, int minimoX, int maximoX, int minimoY, int maximoY);
void limpiar_matriz();
void Imprimir_Matrix_CO();
void llenar_random();
