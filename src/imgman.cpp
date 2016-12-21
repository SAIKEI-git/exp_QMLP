#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imgman.h"
//#include "header.h"

#define GRAYSCALE 0
#define PSUEDOCOLOR 1
#define FULLCOLOR 2

#define BMPFILEHEADER_LENGTH 14
#define BMPINFOHEADER_LENGTH 40


unsigned char* Paddr(img* image,int x,int y)
{
        unsigned char* temp;
        temp=image->data;
        temp+=x*3+y*((image->xsize)*3);
        return temp;
}

long decode_long(unsigned char *ptr)
{
        long value;
        value=*ptr+*(ptr+1)*256+*(ptr+2)*256*256+*(ptr+3)*256*256*256;
        return value;
}

int decode_word(unsigned char *ptr)
{
        int value;
        value=*ptr+*(ptr+1)*256;
        return value;
}

void encode_long(long data,unsigned char *ptr)
{
        int i;
        for(i=0;i<4;i++) {
                *(ptr+i)=data%256;
                data/=256;
        }
}

void encode_word(int data,unsigned char *ptr)
{
        *ptr=data%256;
        *(ptr+1)=data/256;
}


img* create_image_buffer(int x,int y)
{
    unsigned char* buffer;
    img* header;

    if ( (header=(img*)malloc(sizeof(img)))==NULL )
        return NULL;
    if ( (buffer=(unsigned char*)malloc(sizeof(unsigned char)*x*y*3))==NULL ) {
        free(header);
        return NULL;
    }

    header->xsize=x;
    header->ysize=y;
    header->data=buffer;

    return header;
}

void free_image_buffer(img* header)
{
    free(header->data);
    free(header);
}

int pset(img* image,int x,int y,unsigned char value_r,unsigned char value_g,unsigned char value_b)
{
        unsigned char* addr;
        if ( x<0||y<0||x>(image->xsize-1)||y>(image->ysize-1) )
                return 1;

        addr=Paddr(image,x,y);
        *addr=value_r;
        *(addr+1)=value_g;
        *(addr+2)=value_b;
        return 0;
}

int pget(img* image,int x,int y,unsigned char* r,unsigned char* g,unsigned char* b)
{
        if ( x<0||y<0||x>image->xsize||y>image->ysize )
                return 1;

        *r=*(Paddr(image,x,y));
        *g=*(Paddr(image,x,y)+1);
        *b=*(Paddr(image,x,y)+2);
        return 0;
}

img* save_image(img* image,char *filename)
{
        int y;
        FILE *fp;

        if ( (fp=fopen(filename,"wb"))==NULL ) {
                fprintf(stderr,"can't open file %s for writing\n",filename);
                return NULL;
        }

        for(y=0;y<image->ysize;y++)
                fwrite(Paddr(image,0,y),sizeof(unsigned char),image->xsize*3,fp);

        fclose(fp);
        return image;
}

img* load_bmpfile(char *filename)
{
        int x,y;
        long filesize,offset_to_img;
        FILE *fp;
        unsigned char bmpfileheader[BMPFILEHEADER_LENGTH];
        unsigned char bmpinfoheader[BMPINFOHEADER_LENGTH];
        unsigned char *rgbquad;
        long img_width,img_height,compression,img_size;
        int num_color;
        int bytes_per_line;
        unsigned char *buffer,*tmp_buffer;
        img* image;

        if ( (fp=fopen(filename,"rb"))==NULL ) {
                fprintf(stderr,"can't open file %s for reading\n",filename);
                return NULL;
        }

        fread(bmpfileheader,sizeof(unsigned char),14,fp);
        fread(bmpinfoheader,sizeof(unsigned char),40,fp);
        if ( bmpfileheader[0]!='B'||bmpfileheader[1]!='M' ) {
                fprintf(stderr,"this file isn't bmpfile %s\n",filename);
                fclose(fp);
                return NULL;
        }
        filesize=decode_long(bmpfileheader+2);
        offset_to_img=decode_long(bmpfileheader+10);
        img_width=decode_long(bmpinfoheader+4);
        img_height=decode_long(bmpinfoheader+8);
        num_color=decode_word(bmpinfoheader+14);
        compression=decode_long(bmpinfoheader+16);
        img_size=decode_long(bmpinfoheader+20);

        if ( num_color!=8&&num_color!=24 ) {
                fprintf(stderr,"This library does not support depth %d\n",num_color);
                fclose(fp);
                return NULL;
        }

        switch (num_color) {

        case 8:
                if ( (rgbquad=(unsigned char *)malloc(4*256))==NULL ) {
                        fprintf(stderr,"can't allocate memory for palette\n");
                        fclose(fp);
                        return NULL;
                }
                fread(rgbquad,sizeof(unsigned char),4*256,fp);

                bytes_per_line=img_width;
                if ( bytes_per_line%4!=0 )
                        bytes_per_line=((bytes_per_line/4)+1)*4;
                if ( (buffer=(unsigned char*)malloc(bytes_per_line))==NULL ) {
                        fprintf(stderr,"can't allocate memory for line buffer\n");
                        fclose(fp);
                        return NULL;
                }
                if ( (image=create_image_buffer(img_width,img_height))==NULL ) {
                        fprintf(stderr,"can't allocate memory for image\n");
                        fclose(fp);
                        return NULL;
                }
                fseek(fp,offset_to_img,SEEK_SET);
                for(y=0;y<img_height;y++) {
                        fread(buffer,sizeof(unsigned char),bytes_per_line,fp);
                        for(x=0;x<img_width;x++) {
                                tmp_buffer=Paddr(image,x,img_height-y-1);
                                *(tmp_buffer+2)=*(rgbquad+*(buffer+x)*4);
                                *(tmp_buffer+1)=*(rgbquad+*(buffer+x)*4+1);
                                *(tmp_buffer+0)=*(rgbquad+*(buffer+x)*4+2);
                        }
                }
                fclose(fp);
                break;

        case 24:
                bytes_per_line=img_width*3;
                if ( bytes_per_line%4!=0 )
                        bytes_per_line=((bytes_per_line/4)+1)*4;
                if ( (buffer=(unsigned char*)malloc(bytes_per_line))==NULL ) {
                        fprintf(stderr,"can't allocate memory for line buffer\n");
                        fclose(fp);
                        return NULL;
                }
                if ( (image=create_image_buffer(img_width,img_height))==NULL ) {
                        fprintf(stderr,"can't allocate memory for image\n");
                        fclose(fp);
                        return NULL;
                }
                fseek(fp,offset_to_img,SEEK_SET);
                for(y=0;y<img_height;y++) {
                        fread(buffer,sizeof(unsigned char),bytes_per_line,fp);
                        tmp_buffer=Paddr(image,0,img_height-y-1);
                        for(x=0;x<img_width*3;x+=3) {
                                *(tmp_buffer+x+2)=*(buffer+x);
                                *(tmp_buffer+x+1)=*(buffer+x+1);
                                *(tmp_buffer+x+0)=*(buffer+x+2);
                        }
                }
                fclose(fp);
                free(buffer);
                break;
        }

        return image;
}

img* save_bmpfile(img* image,char* filename,int mode)
{
        int x,y,i,tmp;
        unsigned char bmpfileheader[BMPFILEHEADER_LENGTH];
        unsigned char bmpinfoheader[BMPINFOHEADER_LENGTH];
        unsigned char rgbquad_tmp[4];
        unsigned char* buffer;
        unsigned char* addr;
        int bytes_per_line;
        FILE *fp;


        for(i=0;i<BMPFILEHEADER_LENGTH;i++)
                *(bmpfileheader+i)='\0';
        for(i=0;i<BMPINFOHEADER_LENGTH;i++)
                *(bmpinfoheader+i)='\0';

        switch (mode) {
                case GRAYSCALE:
                case PSUEDOCOLOR:
                        bytes_per_line=image->xsize;
                        if ( bytes_per_line%4!=0 )
                                bytes_per_line=((bytes_per_line/4)+1)*4;
                        encode_long(BMPFILEHEADER_LENGTH+BMPINFOHEADER_LENGTH+4*256+bytes_per_line*image->ysize,bmpfileheader+2);
                        encode_long(BMPFILEHEADER_LENGTH+BMPINFOHEADER_LENGTH+4*256,bmpfileheader+10);
                        encode_word(8,bmpinfoheader+14);
                        break;
                case FULLCOLOR:
                        bytes_per_line=image->xsize*3;
                        if ( bytes_per_line%4!=0 )
                                bytes_per_line=((bytes_per_line/4)+1)*4;
                        encode_long(BMPFILEHEADER_LENGTH+BMPINFOHEADER_LENGTH+bytes_per_line*image->ysize,bmpfileheader+2);
                        encode_long(BMPFILEHEADER_LENGTH+BMPINFOHEADER_LENGTH,bmpfileheader+10);
                        encode_word(24,bmpinfoheader+14);
                        break;
                default:
                        return NULL;
                        break;
        }

        *bmpfileheader='B';
        *(bmpfileheader+1)='M';

        encode_long(BMPINFOHEADER_LENGTH,bmpinfoheader);
        encode_long(image->xsize,bmpinfoheader+4);
        encode_long(image->ysize,bmpinfoheader+8);
        encode_word(1,bmpinfoheader+12);
        encode_long(bytes_per_line*image->ysize,bmpinfoheader+20);

        if ( (fp=fopen(filename,"wb"))==NULL ) {
                fprintf(stderr,"can't open file %s for writing\n",filename);
                return NULL;
        }

        fwrite(bmpfileheader,sizeof(unsigned char),BMPFILEHEADER_LENGTH,fp);
        fwrite(bmpinfoheader,sizeof(unsigned char),BMPINFOHEADER_LENGTH,fp);

        switch (mode) {
                case GRAYSCALE:
                        rgbquad_tmp[3]='\0';
                        for(i=0;i<256;i++) {
                                rgbquad_tmp[0]=rgbquad_tmp[1]=rgbquad_tmp[2]=i;
                                fwrite(rgbquad_tmp,sizeof(unsigned char),4,fp);
                        }
                        if ( (buffer=(unsigned char*)malloc(bytes_per_line))==NULL ) {
                                fprintf(stderr,"can't allocate memory for buffer\n");
                                fclose(fp);
                                return NULL;
                        }
                        for(i=0;i<bytes_per_line;i++)
                                *(buffer+i)='\0';
                        for(y=0;y<image->ysize;y++) {
                                addr=Paddr(image,0,image->ysize-y-1);
              for(x=0;x<(image->xsize);x++) {
                tmp=*addr++;
                tmp+=*addr++;
                tmp+=*addr++;
                tmp/=3;
                *(buffer+x)=tmp;
              }
/*                              for(x=0;x<(image->xsize)*3;x+=3)
                                        *(buffer+x)=(*(addr+x)+*(addr+x+1)+*(addr+x+2))/3; */
                                fwrite(buffer,sizeof(unsigned char),bytes_per_line,fp);
                        }
                        free(buffer);
                        fclose(fp);
                        break;

                case PSUEDOCOLOR:
                        fclose(fp);
                        break;

                case FULLCOLOR:
                        if ( (buffer=(unsigned char*)malloc(bytes_per_line))==NULL ) {
                                fprintf(stderr,"can't allocate memory for buffer\n");
                                fclose(fp);
                                return NULL;
                        }
                        for(i=0;i<bytes_per_line;i++)
                                *(buffer+i)='\0';
                        for(y=0;y<image->ysize;y++) {
                                addr=Paddr(image,0,image->ysize-y-1);
                                for(x=0;x<(image->xsize)*3;x+=3) {
                                        *(buffer+x+2)=*(addr+x);
                                        *(buffer+x+1)=*(addr+x+1);
                                        *(buffer+x+0)=*(addr+x+2);
                                }
                                fwrite(buffer,sizeof(unsigned char),bytes_per_line,fp);
                        }
                        free(buffer);
                        break;
        }

        return image;

}



/* end of file */

