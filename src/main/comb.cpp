#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <signal.h>
//#include <unistd.h>
#include <io.h>
#include "header.h"
//#include "wingetopt.h"
//#include "qmlp.h"
//#include "imgman.h"
#include "windows.h"

#define GRAYSCALE 0
#define PSUEDOCOLOR 1
#define FULLCOLOR 2

#define STR_LENGTH 100

int	main (int argc ,char **argv){
	char filename[STR_LENGTH];
	int sandmode=0;
	int i,x,y,m,sumM;
	img *data;
	unsigned char red,blue,green;
	int *image_r,*image_g,*image_b;
	int sizeX,sizeY;


	if(argc < 2)
	{
		fprintf(stderr,"error");
		exit(-1);
	}
	
	//基準bmpをロード
	sprintf(filename,"%s_s%s.bmp",argv[1],argv[2]);
	if ((data=load_bmpfile(filename))==NULL)
	{
		fprintf(stderr,"Input_file open error.\n");
		exit(-1);
	}
	sizeX=data->xsize;
	sizeY=data->ysize;

	image_r=(int*)malloc(sizeof(int)*data->xsize*data->ysize);
	image_g=(int*)malloc(sizeof(int)*data->xsize*data->ysize);
	image_b=(int*)malloc(sizeof(int)*data->xsize*data->ysize);

	sumM=0;
	m = 100*100;
	sumM+=m;
	for(y=0;y<data->ysize;y++){
		for(x=0;x<data->xsize;x++){
			pget(data,x,y,&red,&green,&blue);
			image_r[y*data->xsize+x] = red*m;
			image_g[y*data->xsize+x] = green*m;
			image_b[y*data->xsize+x] = blue*m;
		}
	}

	//スケーリング済みbmpをロードして加算
	for(i=0;i<5;i++){
		sprintf(filename,"%s_m%d_s%s.bmp",argv[1],90-i*10,argv[2]);
		printf("%s\n",argv[1]);
		if ((data=load_bmpfile(filename))==NULL)
		{
			fprintf(stderr,"Input_file open error.\n");
			exit(-1);
		}
		m = (90 - i*10)*(90 - i*10);
		sumM+=m;
		for(y=0;y<data->ysize;y++){
			for(x=0;x<data->xsize;x++){
				pget(data,x,y,&red,&green,&blue);
				image_r[y*data->xsize+x] += red * m;
				image_g[y*data->xsize+x] += green * m;
				image_b[y*data->xsize+x] += blue * m;
			}
		}
	}
	free_image_buffer(data);

	//bmp書き込み
	sprintf(filename,"%s-s%s-CNN.bmp",argv[1],argv[2]);
	data=create_image_buffer(sizeX,sizeY);
	for(y=0;y<sizeY;y++){
    	for(x=0;x<sizeX;x++){
			red = image_r[y*sizeX+x]/sumM;
			green = image_g[y*sizeX+x]/sumM;
			blue = image_b[y*sizeX+x]/sumM;
			pset(data,x,y,red,green,blue);
		}
	}
	save_bmpfile(data,filename,FULLCOLOR);

	free_image_buffer(data);
	return 0;
}

/* end of file */