/*
 *ƒJƒ‰[‰æ‘œˆµ‚¢
 */

#ifndef IMAGEMAN_H__
#define IMAGEMAN_H__



/*
 *‰æ‘œŒ^
 */
typedef struct _img {
    int xsize;
    int ysize;
    unsigned char* data;
} img;

unsigned char* Paddr(img* image,int x,int y);

long decode_long(unsigned char *ptr);

int decode_word(unsigned char *ptr);

void encode_long(long data,unsigned char *ptr);

void encode_word(int data,unsigned char *ptr);


img* create_image_buffer(int x,int y);


void free_image_buffer(img* header);

int pset(img* image,int x,int y,unsigned char value_r,unsigned char value_g,unsigned char value_b);


int pget(img* image,int x,int y,unsigned char* r,unsigned char* g,unsigned char* b);


img* save_image(img* image,char *filename);


img* load_bmpfile(char *filename);

img* save_bmpfile(img* image,char* filename,int mode);


#endif /* IMAGEMAN_H__ */

/* end of file */
