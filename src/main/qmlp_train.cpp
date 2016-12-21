#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <io.h>
#include "header.h"
#include "windows.h"

#define STR_LENGTH 120

#define MAX_SIZE_X 320
#define MAX_SIZE_Y 240

#define DEN 255         //カラー最大濃度
#define MAX_H 360
#define MAX_S 255
#define MAX_V 255

#define MODE_RGB 0
#define MODE_HSV 1
#define MODE_YUV 2

static char *Usage_text =
"USAGE: qmlp_tain [-d num_hidden] [-s ram_seed] [-r learning_rate]  [-l limit] [-w weight_file] [-h] \n input_bmp teach_bmp write_weight write_num write_psnr \n";
#define OPT_STR		"hd:w:l:r:s:m:o:n:x:C:y:R:g:c:" 

int X = 3;
int Y = 3;
int X2 = 3;
int Y2 = 3;

int shuffle = 0;

int mode = 0;

#define GRAYSCALE 0
#define PSUEDOCOLOR 1
#define FULLCOLOR 2

/*
* トポロジ
*/
#define NUM_LAYER	3

#define showpsnr 100

int Topology[NUM_LAYER] = {
	9,
	3,
	9
};

int NUM_INPUT = 9;
int NUM_OUTPUT = 9;

typedef struct {
	vector **input, **target;
	int pixel;
	int xsize;
	int ysize;
	int pattern;
	int *number;
}NetData;

int groomy;

NetData teach[5];
NetData test[5];

int num_file = 1;

/*
* デフォルト学習条件
*/
int LOOP_LIMIT = 1000;
double LEARNING_RATE = 0.7;

/*
* 学習パターン
*/

int allpixel;

int psnr_record = 0;

int Exit_int;
int flag1 = 0;

double sigmoid(double x)
{
	return 1.0 / (1.0 + exp(-x));
}

double ramp(double x)
{
	return x * (x>0);
}

int randam(double max) {
	return (int)(max*rand() / RAND_MAX);
}

void handler(int t)
{
	fprintf(stderr, "signal catch!");
	Exit_int = 1;
}

//教師データのpsnr値を記録
void show_psnr(QMLP *net, int count, FILE *fw) {
	double error[4], temp[4], psnr, psnr_r, psnr_g, psnr_b;
	int i, j, k;

	for (i = 0; i<4; ++i) {
		error[i] = 0.0;
	}
	for (i = 0; i<num_file; i++) {
		for (j = 0; j<teach[i].pattern; j++) {
			qmlp_feed_forward(net, teach[i].input[j], sigmoid);
			qmlp_error_rgb(net, teach[i].target[j], temp);
			for (k = 0; k<4; k++)
				error[k] += temp[k];
		}
	}
	psnr = 10.0*log10((3.0*allpixel) / error[3]);
	psnr_r = 10.0*log10((allpixel) / (error[0]));
	psnr_g = 10.0*log10((allpixel) / (error[1]));
	psnr_b = 10.0*log10((allpixel) / (error[2]));
	fprintf(fw, "%d,%10.2lf,%10.2lf,%10.2lf,%10.2lf", count, psnr, psnr_r, psnr_g, psnr_b);
	if (count%showpsnr == 0) {
		static DWORD st = 0, et = 0;
		et = GetTickCount();
		printf("%d\t", et - st);
		st = et;
		printf("%d\t%10.2lf\t%10.2lf\t%10.2lf\t%10.2lf\n", count, psnr, psnr_r, psnr_g, psnr_b);
	}
	fprintf(fw, "\n");
}
/*
void DataShuffle(NetData *image){
int i,j,t;
for(i=0;i<image->pattern;i++){
j = randam(image->pattern-1);
t = image->number[i];
image->number[i] = image->number[j];
image->number[j] = t;
}
}
*/

QMLP* learn(QMLP *net, int CNN, char *a, char *b)
{
	int count, i, j, k;
	FILE *fw;
	char filename[STR_LENGTH];
	int RATE;
	int **label;
	int sumPattern;
	int temp[2];

	if (flag1 == 0) {
		if ((fw = fopen(b, "w")) == NULL) {
			fprintf(stderr, "time_psnr file open error0.\n");
			exit(1);
		}
	}
	else {
		if ((fw = fopen(b, "a")) == NULL) {
			fprintf(stderr, "time_psnr file open error1.\n");
			exit(1);
		}
	}
	sumPattern = 0;
	for (i = 0; i<num_file; i++) {
		sumPattern += teach[i].pattern;
	}
	printf("%d\n", sumPattern);
	label = (int**)malloc(sizeof(int)*sumPattern);
	for (i = 0; i<sumPattern; i++) {
		label[i] = (int*)malloc(sizeof(int) * 2);
	}
	k = 0;
	for (i = 0; i<num_file; i++) {
		for (j = 0; j<teach[i].pattern; j++) {
			label[k][0] = i;
			label[k][1] = j;
			k++;
		}
	}

	RATE = (int)LEARNING_RATE;
	LEARNING_RATE /= num_file;
	LEARNING_RATE /= 100000.0;
	printf("%lf\n", LEARNING_RATE);
	count = 0;
	show_psnr(net, count, fw);
	srand((unsigned)time(NULL));
	for (count = 1; count <= LOOP_LIMIT; ++count) {
		/*
		if(count==3000/num_file){
		LEARNING_RATE/=10.0;
		printf("changeRate = %lf\n",LEARNING_RATE);
		}
		if(count==4000/num_file){
		LEARNING_RATE/=10.0;
		printf("changeRate = %lf\n",LEARNING_RATE);
		}
		*/
		/*
		for (i=0; i<num_file; i++){
		for(j=0;j<teach[i].pattern;j++){
		qmlp_feed_forward(net, teach[i].input[teach[i].number[j]],sigmoid);
		qmlp_back_propagation(net, teach[i].target[teach[i].number[j]]);
		qmlp_modify_weight(net, LEARNING_RATE);
		}
		if(shuffle){
		DataShuffle(&teach[i]);
		}
		}
		*/
		for (i = 0; i<sumPattern; i++) {
			qmlp_feed_forward(net, teach[label[i][0]].input[label[i][1]], sigmoid);
			qmlp_back_propagation(net, teach[label[i][0]].target[label[i][1]]);
			qmlp_modify_weight(net, LEARNING_RATE);
		}
		if (shuffle) {
			for (i = 0; i<sumPattern; i++) {
				j = randam(sumPattern - 1);
				temp[0] = label[i][0];
				temp[1] = label[i][1];
				label[i][0] = label[j][0];
				label[i][1] = label[j][1];
				label[j][0] = temp[0];
				label[j][1] = temp[1];
			}
		}
		if (count % 1 == 0) {
			show_psnr(net, count, fw);
		}
		//ネット重み保存
		if (count % 100 == 0) {
			if (CNN == 0) {
				sprintf(filename, "weight/weight_%s-%d_%d-%d-%d_c%d_r%d_sfl%d.txt", a, groomy, Topology[0], Topology[1], Topology[2], count, RATE, shuffle);
				printf("save_net c:%d: %s\n", count, filename);
			}
			else {
				sprintf(filename, "weight/weight_%s-%d_%d-%d-%d_c%d_r%d_m%d_sfl%d.txt", a, groomy, Topology[0], Topology[1], Topology[2], count, RATE, CNN, shuffle);
				printf("save_net c:%d: %s\n", count, filename);
			}
			qmlp_write_file(net, filename);
		}
		if (Exit_int) break;
	}
	printf("# loop count: %d\n", count - 1);
	fclose(fw);

	return net;
}

void Init_NetDataMemory(NetData *memory) {
	int i;

	printf("pattern:%d,IN:%d,OUT:%d\n", memory->pattern, NUM_INPUT, NUM_OUTPUT);

	memory->input = (vector**)malloc(memory->pattern * sizeof(vector*));
	for (i = 0; i<memory->pattern; i++)
		memory->input[i] = (vector*)malloc(sizeof(vector)*NUM_INPUT);

	memory->target = (vector**)malloc(memory->pattern * sizeof(vector*));
	for (i = 0; i<memory->pattern; i++)
		memory->target[i] = (vector*)malloc(sizeof(vector)*NUM_OUTPUT);

}

/*	学習データ読み込み関数	*/

void Bright(img *image) {
	double bright;
	int i, j;
	unsigned char red, green, blue;

	bright = 0;
	for (i = 0; i<image->ysize; i++) {
		for (j = 0; j<image->xsize; j++) {
			pget(image, j, i, &red, &green, &blue);
			bright += (double)(0.299*red + 0.587*green + 0.114*blue);
		}
	}
	bright /= image->xsize * image->ysize;
	printf("average bright:%lf", bright);
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

vector RGBtoYUV(double r, double g, double b) {
	double Y, U, V;
	vector yuv;

	//  Y = 0.299 x R + 0.587  x G + 0.114  x B
	//	U = -0.169 x R - 0.3316 x G + 0.500  x B
	//	V = 0.500 x R - 0.4186 x G - 0.0813 x B
	//「16～235（UVに関しては16～240）」

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

NetData InitLearnData(char *a, char *b)
{
	img *data;
	int x, y, i, j;
	unsigned char red, green, blue;
	char filename[STR_LENGTH];
	//	FILE *stream;
	vector **temp;
	NetData image;

	int pX, pY;

	sprintf(filename, "%s", a);
	if ((data = load_bmpfile(filename)) == NULL)
	{
		fprintf(stderr, "Input_file open error.\n");
		exit(-1);
	}

	image.xsize = data->xsize;
	image.ysize = data->ysize;
	image.pixel = data->xsize*data->ysize;
	image.pattern = ((data->xsize - (X - X2)) / X2)*((data->ysize - (Y - Y2)) / Y2);
	image.number = (int*)malloc(sizeof(int)*image.pattern);

	for (i = 0; i<image.pattern; i++) {
		image.number[i] = i;
	}

	printf("パターン数:%d\n", image.pattern);

	Init_NetDataMemory(&image);

	//printf("%d,%d,%d,%d\n",teach.xsize,teach.ysize,teach.pattern,teach.pixel);

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

	//printf("%d,%d\n",pX, pY);

	for (y = 0; y<pY; y++) {
		for (x = 0; x<pX; x++) {
			for (j = 0; j<Y; j++) {
				for (i = 0; i<X; i++) {
					image.input[y*pX + x][X*j + i] = temp[y*Y2 + j][X2*x + i];
				}
			}
		}
	}
	free_image_buffer(data);

	sprintf(filename, "%s", b);
	if ((data = load_bmpfile(filename)) == NULL)
	{
		fprintf(stderr, "teach_file open error.\n");
		exit(-1);
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

	for (y = 0; y < pY; y++) {
		for (x = 0; x < pX; x++) {
			for (j = 0; j < Y2; j++) {
				for (i = 0; i < X2; i++) {
					image.target[y*pX + x][X2*j + i] = temp[(Y - Y2) / 2 + y*Y2 + j][(X - X2) / 2 + X2*x + i];
				}
			}
		}
	}
	free_image_buffer(data);

	return image;
}

int	main(int argc, char **argv) {
	QMLP *net;
	int ram, c;//i,ram,c;
	char filename[STR_LENGTH];
	int i;
	int RATE;
	int CNN = 0;
	int comb = 0;

	if (argc < 6)
	{
		fprintf(stderr, "1 %s", Usage_text);
		exit(-1);
	}

	/*オプションコマンドの処理*/
	/*
	s:新規学習
	r:学習係数変更
	d:隠れ層の変更
	l:学習回数の変更
	w:ネットワーク読み込み
	h:ヘルプ
	n:学習データ数
	*/

	while ((c = getopt(argc, argv, OPT_STR)) != -1) {
		switch (c) {
		case 's':
			flag1 = 0;
			ram = atoi(optarg);
			break;
		case 'r':
			LEARNING_RATE = atof(optarg);
			RATE = atoi(optarg);
			break;
		case 'd':
			Topology[1] = atoi(optarg);
			break;
		case 'x':
			X = Y = atoi(optarg);
			Topology[0] = X*Y;
			break;
		case 'y':
			X2 = Y2 = atoi(optarg);
			Topology[2] = X2*Y2;
			break;
		case 'l':
			LOOP_LIMIT = atoi(optarg);
			break;
		case 'w':
			sprintf(filename, "%s", optarg);
			printf("readfile:%s\n", filename);
			flag1 = 1;
			break;
		case 'h':
			printf("%s", Usage_text);
			exit(1);
			break;
		case 'o':
			psnr_record = 1;
			break;
		case 'C':
			CNN = atoi(optarg);
			break;
		case 'n':
			num_file = atoi(optarg);
			if (num_file > 5) num_file = 5;
			break;
		case 'R':
			shuffle = atoi(optarg);
			break;
		case 'g':
			groomy = atoi(optarg);
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

	if (argc < 3)
	{
		fprintf(stderr, "2 %s", Usage_text);
		exit(-1);
	}
	NUM_INPUT = Topology[0];
	NUM_OUTPUT = Topology[2];
	printf("topolgy %d - %d - %d\n", Topology[0], Topology[1], Topology[2]);
	if (X*Y != NUM_INPUT || X != Y)
	{
		fprintf(stderr, "INPUT error\n");
		exit(-1);
	}
	if (flag1 == 0) {
		if ((net = qmlp_create_net(NUM_LAYER, Topology)) == NULL)
			return -1;
		srand((unsigned)ram);
		qmlp_init_weight(net, 1.0);
	}
	else {
		if ((net = qmlp_read_file(filename)) == NULL)
			return -1;
	}

	allpixel = 0;
	for (i = 0; i < num_file; i++) {
		teach[i] = InitLearnData(argv[num_file + 1 + i * 2], argv[num_file + 2 + i * 2]);
		printf("LordComp %s,%s\n", argv[num_file + 1 + i * 2], argv[num_file + 2 + i * 2]);
		allpixel += teach[i].pixel;
		/*
		read_data(argv[0],argv[1]);
		printf("OK!\n");
		if(psnr_record){
		test[0] = read_testbmp(argv[4],argv[5]);
		test[1] = read_testbmp(argv[6],argv[7]);
		test[2] = read_testbmp(argv[8],argv[9]);
		}*/
	}
	 
	/* 学習 */

	signal(SIGINT, handler);

	if (CNN == 0) {
		sprintf(filename, "psnr\\psnr_%s-%d_%d-%d-%d_c%d_r%d_sfl%d.csv", argv[num_file], groomy, Topology[0], Topology[1], Topology[2], LOOP_LIMIT, RATE, shuffle);
		printf("%s\n", filename);
	}
	else {
		sprintf(filename, "psnr\\psnr_%s-%d_%d-%d-%d_c%d_r%d_m%d_sfl%d.csv", argv[num_file], groomy, Topology[0], Topology[1], Topology[2], LOOP_LIMIT, RATE, CNN, shuffle);
		printf("%s\n", filename);
	}

	//学習させながらネットの重み保存
	learn(net, CNN, argv[num_file], filename);
	signal(SIGINT, SIG_DFL);

	qmlp_destroy_net(net);

	return 0;
}

/* end of file */
