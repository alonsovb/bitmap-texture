/* 
 * File:   bmpFile.h
 * Author: Alonso
 *
 * Created on 20 de septiembre de 2011, 04:56 PM
 */

#ifndef BMPFILE_H



#define	BMPFILE_H

char **getBitmapMatrix(char *fileName);
int getWidth(char *fileName);
int getHeight(char *fileName);
//void setBitmapFile(char **bitmap, char *fileout, char *filein, int width, int height);
int fileExists(char *fileName);

#ifdef	__cplusplus
extern "C" {
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* BMPFILE_H */

