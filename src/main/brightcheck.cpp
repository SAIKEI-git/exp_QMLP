#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imgman.h"

int randam(int max){
	return (int)(max*1.0*rand()/RAND_MAX);
}

double bright(img *image){
	double bright;
	int i,j;
	unsigned char red,green,blue;
	bright=0;
	for(i=0;i<image->ysize;i++){
		for(j=0;j<image->xsize;j++){
			pget(image,j,i,&red,&green,&blue);
			bright += (double)(0.299*red + 0.587*green + 0.114*blue);
		}
	}
	bright /= image->xsize * image->ysize;
	return bright;
}

int main(int argc,char **argv){
	img *image;
	char filename[120];
	double ret;

	if(argc<2){
		printf("error\n");
		return -1;
	}
	sprintf(filename,"%s",argv[1]);
	if ((image=load_bmpfile(filename))==NULL)
	{
		fprintf(stderr,"Input_file open error.\n");
		exit(-1);
	}
	ret = bright(image);
	printf("%s:%lf\n",argv[1],ret);
	return 0;
}