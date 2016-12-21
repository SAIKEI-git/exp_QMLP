#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "wingetopt.h"
//#include "imgman.c"
#include "header.h"

#define DEN 256

double *image_1r,*image_1g,*image_1b,*image_2r,*image_2g,*image_2b;   


int main(int argc,char **argv)
{
        img *data;
        int x,y;
	double temp,tempr,tempg,tempb,psnr,psnr_r,psnr_g,psnr_b,psnr_gosa;
	double r,g,b;
        unsigned char red,green,blue;
        char filename[80]; 

        if(argc<2)
        {
                fprintf(stderr,"error.\n");
                exit(1);
        }




        sprintf(filename,"%s",argv[1]);
        if ((data=load_bmpfile(filename))==NULL)
        {
                fprintf(stderr,"file open error.\n",argv[1]);
                exit(1);
        }
        image_1r=malloc(data->xsize*data->ysize*sizeof(double));
        image_1g=malloc(data->xsize*data->ysize*sizeof(double));
        image_1b=malloc(data->xsize*data->ysize*sizeof(double));

        for(y=0;y<data->ysize;y++)
                for(x=0;x<data->xsize;x++)
                {
                        pget(data,x,y,&red,&green,&blue);
                        image_1r[y*data->xsize+x] = (double)red/DEN;
                        image_1g[y*data->xsize+x] = (double)green/DEN;
                        image_1b[y*data->xsize+x] = (double)blue/DEN;
                }
       

        sprintf(filename,"%s",argv[2]);
        if ((data=load_bmpfile(filename))==NULL)
        {
                fprintf(stderr,"file open error.\n",argv[1]);
                exit(1);
        }
        image_2r=malloc(data->xsize*data->ysize*sizeof(double));
        image_2g=malloc(data->xsize*data->ysize*sizeof(double));
        image_2b=malloc(data->xsize*data->ysize*sizeof(double));

        for(y=0;y<data->ysize;y++)
                for(x=0;x<data->xsize;x++)
                {
                        pget(data,x,y,&red,&green,&blue);
                        image_2r[y*data->xsize+x] = (double)red/DEN;
                        image_2g[y*data->xsize+x] = (double)green/DEN;
                        image_2b[y*data->xsize+x] = (double)blue/DEN;
                }
        temp=0.0;
		tempr=tempg=tempb=0.0;
        for(y=0;y<data->xsize*data->ysize;y++)
		{
		    r = image_1r[y]-image_2r[y];
		    g = image_1g[y]-image_2g[y];
		    b = image_1b[y]-image_2b[y];
		    temp += r*r+g*g+b*b;
			tempr += r*r;
			tempg += g*g;
			tempb += b*b;
		}
	psnr=10.0*log10( (3.0*data->xsize*data->ysize)/temp );
	psnr_r=10.0*log10( (data->xsize*data->ysize)/tempr );
	psnr_g=10.0*log10( (data->xsize*data->ysize)/tempg );
	psnr_b=10.0*log10( (data->xsize*data->ysize)/tempb );
	psnr_gosa = (psnr-psnr_r)*(psnr-psnr_r)+(psnr-psnr_g)*(psnr-psnr_g)+(psnr-psnr_b)*(psnr-psnr_b);
	printf("%s,\t%lf, %lf, %lf, %lf, %lf\n",argv[3],psnr,psnr_r,psnr_g,psnr_b,psnr_gosa);
}
