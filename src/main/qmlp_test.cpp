#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <io.h>
#include "header.h"
#include <windows.h>


#define GRAYSCALE 0
#define PSUEDOCOLOR 1
#define FULLCOLOR 2


#define DEN 255         //カラー最大濃度
#define MAX_H 360
#define MAX_S 255
#define MAX_V 255

#define MODE_RGB 0
#define MODE_HSV 1
#define MODE_YUV 2

int mode = 0;

static char *Usage_text =
"USAGE: qmlp_test [-h] read_weight input_bmp write_bmp \n";
#define OPT_STR		"h:m:i:o:R:c:"

/*
* トポロジ
*/

#define NUM_LAYER	3
int NUM_INPUT = 9;
int NUM_OUTPUT = 1;

int X = 3;
int Y = 3;
int X2 = 1;
int Y2 = 1;

int shuffle = 0;

int Topology[NUM_LAYER] = {
	9,
	3,
	1
};

int NUM_X;
int NUM_Y;
int NUM_PTTERN;
int NUM_PX;
int NUM_PY;

vector **Input;
vector **Output;

int XSIZE;
int YSIZE;

double
sigmoid(double x)
{
	return 1.0 / (1.0 + exp(-x));
}

double
ramp(double x)
{
	return x * (x>0);
}

vector RGBtoHSV(double r, double g, double b) {
	vector hsv;
	// H 色相  いろいろ
	// S 彩度  
	// V 明度　max(r,g,b)
	double max,min,H,S,V;

	r /= DEN;
	g /= DEN;
	b /= DEN;

	max = min = 0;
	max = (r > g) ? r : g;
	max = (max > b) ? max : b;
	min = (r < g) ? r : g;
	min = (min < b) ? min : b;

	S = max - min;
	V = max;

	if(max - min > 0){
		if(max == r){
			H = (g - b)/(max - min);
			if(H < 0){
				H += 6.0;
			}
		}else if(max == g){
			H = 2.0 + (b - r) /(max - min);
		}else{
			H = 4.0 + (r - g) /(max - min);
		}
	}
	H /= 6.0;
	if(max != 0.0){
		S /= max;
	}

	hsv.i = H;
	hsv.j = S;
	hsv.k = V;

	return hsv;
}

void HSVtoRGB(vector hsv, unsigned char *r, unsigned char *g, unsigned char *b) {
	double F, S;
	int H;
	double R,G,B;

	S = hsv.j;
	R = G = B = hsv.k;

	if (hsv.j > 0) {
		hsv.i *= 6.0;
		H = (int)hsv.i;
		F = hsv.i - (double)H;
		switch(H){
		case 0:
			G *= 1 - S * (1 - F);
			B *= 1 - S * F;
			break;
		case 1:
			R *= 1 - S * F;
			B *= 1 - S;
			break;
		case 2:
			R *= 1 - S;
			B *= 1 - S * (1 - F);
			break;
		case 3:
			R *= 1 - S;
			G *= 1 - S * F;
			break;
		case 4:
			R *= 1 - S * (1 - F);
			G *= 1 - S;
			break;
		case 5:
			G *= 1 - S;
			B *= 1 - S * F;
			break;
		}
	}

	*r = (int)(R * DEN);
	*g = (int)(G * DEN);
	*b = (int)(B * DEN);
}
vector RGBtoYUV(double r, double g, double b) {
	double Y, U, V;
	vector yuv;

	double e;

	e = 0.8588;

	r = r*e + 16;
	g = g*e + 16;
	b = b*e + 16;

	Y = 0.299 * r + 0.587 * g + 0.114 * b;
	U = -0.169 * r - 0.3316 * g + 0.500 * b + 128;
	V = 0.500 * r - 0.4186 * g - 0.0813 * b + 128;

	yuv.i = Y;
	yuv.j = U;
	yuv.k = V;

	return yuv;
}

void YUVtoRGB(vector yuv,unsigned char *r, unsigned char *g, unsigned char *b) {
	double Y, U, V;

	yuv.i = yuv.i*DEN;
	yuv.j = yuv.j*DEN - 128;
	yuv.k = yuv.k*DEN - 128;

	Y = yuv.i + 1.402 * yuv.k - 16;
	U = yuv.i - 0.34414 * yuv.j - 0.71414 * yuv.k - 16;
	V = yuv.i + 1.772 * yuv.j - 16;

	Y *= 1.164;
	U *= 1.164;
	V *= 1.164;

	if (Y < 0) {
		Y = 0;
	}else if (Y > 255) {
		Y = 255;
	}
	if (U < 0) {
		U = 0;
	}else if (U > 255) {
		U = 255;
	}
	if (V < 0) {
		V = 0;
	}else if (V > 255) {
		V = 255;
	}

	*r = (int)Y;
	*g = (int)U;
	*b = (int)V;

}

/*	テストデータ読み込み関数	*/
void read_bmp(char *a)
{
	img *data;
	int x, y, i, j;
	unsigned char red, green, blue;
	char filename[40];
	vector **temp;
	int pX, pY;

	sprintf(filename, "%s", a);
	if ((data = load_bmpfile(filename)) == NULL)
	{
		fprintf(stderr, "Input_file open error.\n");
		exit(-1);
	}

	NUM_PX = (data->xsize - (X - X2)) / X2;
	NUM_PY = (data->ysize - (Y - Y2)) / Y2;
	NUM_PTTERN = NUM_PX * NUM_PY;

	XSIZE = data->xsize;
	YSIZE = data->ysize;

	Input = (vector**)malloc(NUM_PTTERN * sizeof(vector*));
	for (i = 0; i<NUM_PTTERN; i++)
		Input[i] = (vector*)malloc(sizeof(vector)*NUM_INPUT);
	Output = (vector**)malloc(NUM_PTTERN * sizeof(vector*));
	for (i = 0; i<NUM_PTTERN; i++)
		Output[i] = (vector*)malloc(sizeof(vector)*NUM_OUTPUT);

	temp = (vector**)malloc(sizeof(vector*)*data->ysize);
	for (i = 0; i < data->ysize; i++) {
		temp[i] = (vector*)malloc(sizeof(vector)*data->xsize);
	}

	for (i = 0; i<data->ysize; i++) {
		for (j = 0; j<data->xsize; j++) {
			pget(data, j, i, &red, &green, &blue);
			if(mode == MODE_RGB){
				temp[i][j].i = (double)red / DEN;
				temp[i][j].j = (double)green / DEN;
				temp[i][j].k = (double)blue / DEN;
			}else if(mode == MODE_HSV){
				temp[i][j] = RGBtoHSV(red, green, blue);
			}else if(mode == MODE_YUV){
				temp[i][j] = RGBtoYUV(red, green, blue);
				temp[i][j].i /= DEN;
				temp[i][j].j /= DEN;
				temp[i][j].k /= DEN;
			}
			if (i%10 == 0 && j%10 == 0){
				//printf("%lf,%lf,%lf\n", temp[i][j].i, temp[i][j].j, temp[i][j].k);
			}
		}
	}
	
	pX = (data->xsize - (X - X2)) / X2;
	pY = (data->ysize - (Y - Y2)) / Y2;

	//printf("%d,%d\n", pX, pY);

	for (y = 0; y<pY; y++) {
		for (x = 0; x<pX; x++) {
			for (j = 0; j<Y; j++) {
				for (i = 0; i<X; i++) {
					Input[y*pX + x][X*j + i] = temp[y*Y2 + j][X2*x + i];
				}
			}
		}
	}
	free_image_buffer(data);
}

/*	出力データ書き込み関数	*/
void write_bmp(char *a)
{
	int x, y, i, j;
	img *data;
	char filename[100];
	unsigned char red, green, blue;
	int pX, pY;
	vector **temp;

	sprintf(filename, "%s", a);
	data = create_image_buffer(XSIZE, YSIZE);
	temp = (vector**)malloc(sizeof(vector*)*data->ysize);
	for (i = 0; i < data->ysize; i++) {
		temp[i] = (vector*)malloc(sizeof(vector)*data->xsize);
	}
	pX = (data->xsize - (X - X2)) / X2;
	pY = (data->ysize - (Y - Y2)) / Y2;

	for (y = 0; y<pY; y++) {
		for (x = 0; x<pX; x++) {
			for (j = 0; j < Y2; j++) {
				for (i = 0; i < X2; i++) {
					temp[y*Y2 + j][X2*x + i] = Output[y*pX + x][X2*j + i];
				}
			}
		}
	}

	for (j = 0; j<YSIZE; j++) {
		for (i = 0; i < XSIZE; i++) {
			pset(data, i, j, 0, 0, 0);
			pset(data, i, j , 0, 0, 0);
		}
	}
	//	printf("paddingOK\n");
	//	red = green = blue = 0.0;
	for (y = 0; y < YSIZE - (Y - Y2); y++) {
		for (x = 0; x < XSIZE - (X - X2); x++) {
			if(mode == MODE_RGB){
				red = (int)(temp[y][x].i*DEN);
				green = (int)(temp[y][x].j*DEN);
				blue = (int)(temp[y][x].k*DEN);
			}else if(mode == MODE_HSV){
				HSVtoRGB(temp[y][x], &red, &green, &blue);
			}else if(mode == MODE_YUV){
				YUVtoRGB(temp[y][x],&red,&green,&blue);
			}
			if (x % 10 == 0 && y % 10 == 0){
				//printf("%lf,%lf,%lf\n", temp[y][x].i, temp[y][x].j, temp[y][x].k);
			}
			pset(data, x + (X - X2) / 2, y + (Y - Y2) / 2, red, green, blue);
		}
	}

	save_bmpfile(data, filename, FULLCOLOR);
}

int main(int argc, char **argv){
	QMLP *net;
	int i, j, c;
	char filename[100];
	vector *out;
	DWORD st, et;

	if (argc<4)
	{
		fprintf(stderr, "%s", Usage_text);
		exit(-1);
	}

	while ((c = getopt(argc, argv, OPT_STR)) != -1) {
		switch (c) {
		case 'h':
			printf("opt: h\n");
			printf("%s", Usage_text);
			exit(1);
		case 'i':
			Topology[0] = atoi(optarg);
			break;
		case 'm':
			Topology[1] = atoi(optarg);
			break;
		case 'o':
			Topology[2] = atoi(optarg);
			break;
		case 'c':
			mode = atoi(optarg);
			if(mode>=0&&mode<3)
			{
				printf("mode:%d\n",mode);
			}else{
				mode = 0;
			}
			break;
		case '?':
		default:
			fprintf(stderr, "Try `qmlp_train -h' for more.\n");
			exit(-1);
		}
	}
	argc -= optind;
	argv += optind;

	if ((net = qmlp_read_file(argv[0])) == NULL) {
		return -1;
	}

	X = Y = Topology[0];
	X2 = Y2 = Topology[2];
	NUM_INPUT = X * Y;
	NUM_OUTPUT = X2 * Y2;

	//	printf("%d,%d,%d,%d,%d,%d\n",X,Y,X2,Y2,NUM_INPUT,NUM_OUTPUT);

	read_bmp(argv[1]);

	st = GetTickCount();

	for (i = 0; i<NUM_PTTERN; i++) {
		out = qmlp_feed_forward(net, Input[i], sigmoid);
		for (j = 0; j<NUM_OUTPUT; j++) {
			Output[i][j] = out[j];
		}
	}
	et = GetTickCount();
	printf("1 %d\n", et - st);
	sprintf(filename, "bmpoutput\\%s", argv[2]);
	write_bmp(filename);
	printf("save..%s\n", argv[2]);
	qmlp_destroy_net(net);
	return 0;
}

/* end of file */
