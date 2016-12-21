
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#include "qmlp.h"
#include "header.h"

/*
 * QMLP?t?@?C???????????magic number
 */
#define QMLP_MAGIC	"QMLP"

/*
 * ???l???t?@?C????o?????????t?H?[?}?b?g
 */
#define NUM_FORMAT	"%.16e"


/*
 * norm??0.0????????????????????
 */
#define EPS	1e-30


/*
 * ???????????дн???
 */
static void *
my_malloc (size_t size)
{
	void *ptr;

	if ((ptr=malloc(size)) == NULL) {
		fprintf(stderr, "qmlp: Cannot allocate memory object.\n");
		exit(-1);
	}

	return ptr;
}


/*
 * 0.0 ?` 1.0??????????????
 */
static double
rnd (void)
{
	return (double)rand()/RAND_MAX;
}

/*
 * ?l?b?g???[?N???????C?l?b?g???[?N??S?p?????[?^???N???A????
 */
QMLP *
qmlp_create_net (int num_layer, const int *topology)
{
	int k, n, m, num;
	QMLP *net;

	if (num_layer < 2) {
		fprintf(stderr, "qmlp: Num of layers must be greater than 1.\n");
		return NULL;
	}

	net = (QMLP*)my_malloc(sizeof(QMLP));

	net->num_layer = num_layer;
	net->topology = (int*)my_malloc(sizeof(int)*num_layer);
	for (k=0; k<num_layer; ++k)
		net->topology[k] = topology[k];
	
	net->neuron_out = (vector**)my_malloc(sizeof(vector*)*num_layer);
	for (k=0; k<num_layer; ++k) {
		if (topology[k] < 1) {
			fprintf(stderr,
				"qmlp: Num of neurons must be positive.\n");
			return NULL;
		}

		num = topology[k];
		net->neuron_out[k] = (vector*)my_malloc(sizeof(vector)*num);
	}

	net->neuron_back = (vector**)my_malloc(sizeof(vector*)*(num_layer - 1));
	for (k=1; k<num_layer; ++k) {
		num = topology[k];
		net->neuron_back[k - 1] = (vector*)my_malloc(sizeof(vector)*num);
	} 
	net->threshold = (vector**)my_malloc(sizeof(vector*)*(num_layer - 1));
	net->weight = (quaternion**)my_malloc(sizeof(quaternion*)*(num_layer - 1));
	net->rmat = (rot_mat**)my_malloc(sizeof(rot_mat*)*(num_layer - 1));
	net->weight_norm = (double**)my_malloc(sizeof(quaternion*)*(num_layer - 1));
	for (k=0; k<num_layer-1; ++k) {
		m = topology[k];
		n = topology[k + 1];

		net->threshold[k] = (vector*)my_malloc(sizeof(vector)*n);
		net->weight[k] = (quaternion*)my_malloc(sizeof(quaternion)*m*n);
		net->rmat[k] = (rot_mat*)my_malloc(sizeof(rot_mat)*m*n);
		net->weight_norm[k] = (double*)my_malloc(sizeof(double)*m*n);
	}

	net->dthreshold = (vector**)my_malloc(sizeof(vector*)*(num_layer - 1));
	net->dweight = (quaternion**)my_malloc(sizeof(quaternion*)*(num_layer - 1));
	for (k=0; k<num_layer-1; ++k) {
		m = topology[k];
		n = topology[k + 1];

		net->dthreshold[k] = (vector*)my_malloc(sizeof(vector)*n);
		net->dweight[k] = (quaternion*)my_malloc(sizeof(quaternion)*m*n);
	}

	qmlp_clear_net(net);
	return net;
}

/*
 * ?l?b?g???[?N??????????J??????
 */
void
qmlp_destroy_net (QMLP *net)
{
	int k;

	for (k=0; k<net->num_layer-1; ++k) {
		free(net->threshold[k]);
		free(net->weight[k]);
		free(net->rmat[k]);
		free(net->weight_norm[k]);
		free(net->dthreshold[k]);
		free(net->dweight[k]);
	}
	free(net->threshold);
	free(net->weight);
	free(net->rmat);
	free(net->weight_norm);
	free(net->dthreshold);
	free(net->dweight);

	for (k=0; k<net->num_layer; ++k) {
		free(net->neuron_out[k]);
	}
	free(net->neuron_out);

	for (k=1; k<net->num_layer; ++k) {
		free(net->neuron_back[k - 1]);
	}
	free(net->neuron_back);

	free(net->topology);
	free(net);
}

/*
 * ?d??weight???lthreshold????l????[-range, range]??????????C
 * dweight, dthreshold???N???A????
 */
void
qmlp_init_weight (QMLP *net, double range)
{
	int k, i, m, n;
	vector x;
	quaternion a;

	srand((unsigned)time(NULL));
	for (k=0; k<net->num_layer-1; ++k) {
		m = net->topology[k];
		n = net->topology[k+1];

		for (i=0; i<n; ++i) {
			x.i = range*(2.0*rnd() - 1.0);
			x.j = range*(2.0*rnd() - 1.0);
			x.k = range*(2.0*rnd() - 1.0);
			net->threshold[k][i] = x;
			net->dthreshold[k][i] = v_literal(0.0, 0.0, 0.0);
		}
		for (i=0; i<m*n; ++i) {
			a.r = range*(2.0*rnd() - 1.0);
			a.i = range*(2.0*rnd() - 1.0);
			a.j = range*(2.0*rnd() - 1.0);
			a.k = range*(2.0*rnd() - 1.0);
			net->weight[k][i] = a;
			net->dweight[k][i] = q_literal(0.0, 0.0, 0.0, 0.0);
		}
	}

	qmlp_mk_rmat(net);
}


/*
 * weight????rmat, weight_norm????????
 */
void
qmlp_mk_rmat (QMLP *net)
{
	int k, i, m, n;

	for (k=0; k<net->num_layer-1; ++k) {
		m = net->topology[k];
		n = net->topology[k+1];

		for (i=0; i<m*n; ++i) {
			mk_rot_mat(net->weight[k][i], &net->rmat[k][i]);
			net->weight_norm[k][i] =
				q_norm(net->weight[k][i]) + EPS;
		}
	}
}


/*
 * vector??o????
 */
static vector
sigmoid_v (vector x, double (*sigmoid)(double))
{
	vector y;
	y.i = sigmoid(x.i);
	y.j = sigmoid(x.j);
	y.k = sigmoid(x.k);
	return y;
}

static vector
ramp_v (vector x)
{
	vector y;
	y.i = x.i * (x.i>0);
	y.j = x.j * (x.j>0);
	y.k = x.k * (x.k>0);
	return y;
}

/*
 * ????????o????v?Z????
 */
vector *
qmlp_feed_forward (QMLP *net, vector *input, double (*sigmoid)(double))
{
	int i, j, k, n, m;
	int output_layer;
	vector sum;
	vector x;
	double norm;

	output_layer = net->num_layer - 1;

	for (i=0; i < net->topology[0]; ++i)
		net->neuron_out[0][i] = input[i];
	for (k=0; k < net->num_layer-1; ++k) {
		m = net->topology[k];
		n = net->topology[k + 1];

		for (i=0; i<n; ++i) {
			sum = net->threshold[k][i];
			for (j=0; j<m; ++j) {
				x = v_mat_rot(&net->rmat[k][i*m + j],
					net->neuron_out[k][j]);
				norm = net->weight_norm[k][i*m + j];
				x = v_rmul(1.0/norm, x);
				sum = v_add(sum, x);
			}
			net->neuron_out[k + 1][i] = sigmoid_v(sum, sigmoid);
		}
	}

	return net->neuron_out[output_layer];
}


/*
 * ??W?o????2??????????
 */
double
qmlp_error (const QMLP *net, vector *target)
{
	int i, k, m;
	double error;
	vector d;

	error = 0.0;

	k = net->num_layer - 1;
	m = net->topology[k];
	for (i=0; i<m; ++i) {
		d = v_sub(target[i], net->neuron_out[k][i]);
		error += v_norm2(d);
	}

	return error;
}


void
qmlp_error_rgb (const QMLP *net, vector *target, double *error)
{
	int i, k, m;
	vector d;

	k = net->num_layer - 1;
	m = net->topology[k];
	
	for(i=0;i<4;++i)
		error[i]=0.0;

	for (i=0; i<m; ++i) {
		d = v_sub(target[i], net->neuron_out[k][i]);
		error[0] += d.i*d.i;
		error[1] += d.j*d.j;
		error[2] += d.k*d.k;
		error[3] += v_norm2(d);
	}
}

/*
 * ?V?O???C?h?????o??l????????l???????
 * (d/dx) sigmoid(x) = sigmoid(x)*(1.0 - sigmoid(x))
 */
static vector
diff_sigmoid_v (vector y)
{
	vector z;
	z.i = y.i*(1.0 - y.i);
	z.j = y.j*(1.0 - y.j);
	z.k = y.k*(1.0 - y.k);
	return z;
}

static vector
diff_ramp_v (vector y)
{
	vector z;
	z.i = 1.0 * (y.i>0);
	z.j = 1.0 * (y.j>0);
	z.k = 1.0 * (y.k>0);
	return z;
}


/*
 * ???t?`???@????????o?????t??v?Z???C
 * ?X?V???dweight, dthreshold???????
 */
void
qmlp_back_propagation (QMLP *net, vector *target)
{
	int i, j, k, l, n, m;
	int output_layer;
	vector out, back, sum, d;
	vector x, y, dt;
	quaternion a, w, dw;
	double s, norm;

	output_layer = net->num_layer - 1;

	k = output_layer;
	l = net->topology[k - 1];
	m = net->topology[k];
	for (i=0; i<m; ++i) {
		out = net->neuron_out[k][i];
		d = v_sub(target[i], out);
		net->neuron_back[k - 1][i] =
			v_comp_mul(d, diff_sigmoid_v(out));
			//v_comp_mul(d, diff_ramp_v(out));

		back = net->neuron_back[k - 1][i];
		dt = net->dthreshold[k - 1][i];
		net->dthreshold[k - 1][i] = v_add(dt, back);

		for (j=0; j<l; ++j) {
			out = net->neuron_out[k - 1][j];
			w = net->weight[k - 1][i*l + j];
			norm = net->weight_norm[k - 1][i*l + j];

			a = q_rot_by_v(back, out, w);
			a = q_rmul(2.0, a);

			y = v_mat_rot(&net->rmat[k - 1][i*l + j], out);
			s = v_smul(y, back)/(norm*norm);

			a = q_sub(a, q_rmul(s, w));
			a = q_rmul(1.0/norm, a);

			dw = net->dweight[k - 1][i*l + j];
			net->dweight[k - 1][i*l + j] = q_add(dw, a);
		}
	}

	k = output_layer - 1;
	for ( ; k>0; --k) {
		l = net->topology[k - 1];
		m = net->topology[k];
		n = net->topology[k + 1];

		for (i=0; i<m; ++i) {
			sum = v_literal(0.0, 0.0, 0.0);
			for (j=0; j<n; ++j) {
				back = net->neuron_back[k][j];
				norm = net->weight_norm[k][j*m + i];

				x = v_tr_mat_rot(&net->rmat[k][j*m + i], back);
				x = v_rmul(1.0/norm, x);
				sum = v_add(sum, x);
			}
			out = net->neuron_out[k][i];
			net->neuron_back[k - 1][i] =
				v_comp_mul(sum, diff_sigmoid_v(out));
//				v_comp_mul(sum, diff_ramp_v(out));

			back = net->neuron_back[k - 1][i];
			dt = net->dthreshold[k - 1][i];
			net->dthreshold[k - 1][i] = v_add(dt, back);

			for (j=0; j<l; ++j) {
				out = net->neuron_out[k - 1][j];
				w = net->weight[k - 1][i*l + j];
				norm = net->weight_norm[k - 1][i*l + j];

				a = q_rot_by_v(back, out, w);
				a = q_rmul(2.0, a);

				y = v_mat_rot(&net->rmat[k - 1][i*l + j], out);
				s = v_smul(y, back)/(norm*norm);

				a = q_sub(a, q_rmul(s, w));
				a = q_rmul(1.0/norm, a);

				dw = net->dweight[k - 1][i*l + j];
				net->dweight[k - 1][i*l + j] = q_add(dw, a);
			}
		}
	}
}


/*
 * ?d????l???X?V????dweight, dthreshold???N???A????
 */
void
qmlp_modify_weight (QMLP *net, double learning_rate)
{
	int k, i, m, n;
	vector dt;
	quaternion dw;
	vector x;
	quaternion a;

	for (k=0; k<net->num_layer-1; ++k) {
		m = net->topology[k];
		n = net->topology[k+1];

		for (i=0; i<n; ++i) {
			dt = v_rmul(learning_rate, net->dthreshold[k][i]);
			x = net->threshold[k][i];
			net->threshold[k][i] = v_add(x, dt);

			net->dthreshold[k][i] = v_literal(0.0, 0.0, 0.0);
		}

		for (i=0; i<m*n; ++i) {
			dw = q_rmul(learning_rate, net->dweight[k][i]);
			a = net->weight[k][i];
			net->weight[k][i] = q_add(a, dw);

			mk_rot_mat(net->weight[k][i], &net->rmat[k][i]);
			net->weight_norm[k][i] =
				q_norm(net->weight[k][i]) + EPS;

			net->dweight[k][i] = q_literal(0.0, 0.0, 0.0, 0.0);
		}
	}
}


/*
 * dweight, dthreshold???N???A????
 */
void
qmlp_clear_dweight (QMLP *net)
{
	int k, i, m, n;

	for (k=0; k<net->num_layer-1; ++k) {
		m = net->topology[k];
		n = net->topology[k + 1];

		for (i=0; i<n; ++i) {
			net->dthreshold[k][i] = v_literal(0.0, 0.0, 0.0);
		}
		for (i=0; i<n*m; ++i) {
			net->dweight[k][i] = q_literal(0.0, 0.0, 0.0, 0.0);
		}
	}
}


/*
 * ?l?b?g???[?N??num_layer, topology??O??p?????[?^???N???A????
 */
void
qmlp_clear_net (QMLP *net)
{
	int k, i, m, n;

	for (k=0; k<net->num_layer; ++k) {
		for (i=0; i<net->topology[k]; ++i)
			net->neuron_out[k][i] = v_literal(0.0, 0.0, 0.0);
	}
	for (k=1; k<net->num_layer; ++k) {
		for (i=0; i<net->topology[k]; ++i)
			net->neuron_back[k - 1][i] = v_literal(0.0, 0.0, 0.0);
	}

	for (k=0; k<net->num_layer-1; ++k) {
		m = net->topology[k];
		n = net->topology[k + 1];

		for (i=0; i<n; ++i) {
			net->threshold[k][i] = v_literal(0.0, 0.0, 0.0);
			net->dthreshold[k][i] = v_literal(0.0, 0.0, 0.0);
		}
		for (i=0; i<n*m; ++i) {
			net->weight[k][i] = q_literal(0.0, 0.0, 0.0, 0.0);
			net->dweight[k][i] = q_literal(0.0, 0.0, 0.0, 0.0);
		}
	}

	qmlp_mk_rmat(net);
}


/*
 * ?t?@?C????QMLP?l?b?g???[?N??g?|???W??d???????????
 */
int
qmlp_write_file (const QMLP *net, const char *fname)
{
	FILE *fp;
	int i, j, n, m, k;
	quaternion a;
	vector x;

	if ((fp = fopen(fname, "w")) == NULL) {
		fprintf(stderr,
			"qmlp: Cannot open file for writing: `%s'.\n",fname);
		return 0;
	}

	fprintf(fp, "%s\n\n", QMLP_MAGIC);

	fprintf(fp, "%d\n", net->num_layer);
	for (k=0; k<net->num_layer; ++k) {
		fprintf(fp, " %d", net->topology[k]);
	}
	fprintf(fp, "\n\n");


	for (k=0; k<net->num_layer-1; ++k) {
		m = net->topology[k];
		n = net->topology[k + 1];
		
		for (j=0; j<n; ++j) {
			for (i=0; i<m; ++i) {
				a = net->weight[k][j*m + i];
				fprintf(fp, "   " NUM_FORMAT " " NUM_FORMAT
					" " NUM_FORMAT " " NUM_FORMAT,
					a.r, a.i, a.j, a.k);
			}
			x = net->threshold[k][j];
			fprintf(fp, "   " NUM_FORMAT " " NUM_FORMAT
				" " NUM_FORMAT, x.i, x.j, x.k);
			fprintf(fp, "\n");
		}
		fprintf(fp, "\n");
	}

	fclose(fp);

	return 1;
}


/*
 * ?t?@?C??????QMLP?l?b?g???[?N??f?[?^???????
 */
QMLP *
qmlp_read_file (const char *fname)
{
	FILE *fp;
	char buf[256];
	int num_layer, *topology;
	int i, j, k, n, m, count;
	QMLP *net;
	quaternion a;
	vector x;

	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr,
			"qmlp: Cannot open file for reading: `%s'\n", fname);
		return NULL;
	}

	fgets(buf, 32, fp);
	if (strncmp(buf, QMLP_MAGIC, strlen(QMLP_MAGIC)) != 0) {
		fprintf(stderr, "qmlp: Illegal file format: `%s'\n", fname);
		fclose(fp);
		return NULL;
	}

	if (fscanf(fp, "%d", &num_layer) != 1 || num_layer < 2) {
		fprintf(stderr, "qmlp: Illegal file format: `%s'\n", fname);
		fclose(fp);
		return NULL;
	}

	topology = (int*)my_malloc(sizeof(int)*num_layer);

	for (i=0; i<num_layer; ++i) {
		if (fscanf(fp, "%d", &topology[i]) != 1) {
			fprintf(stderr,
				"qmlp: Illegal file format: `%s'\n", fname);
			free(topology);
			fclose(fp);
			return NULL;
		}
	}

	if ((net=qmlp_create_net(num_layer, topology)) == NULL) {
		free(topology);
		fclose(fp);
		return NULL;
	}

	free(topology);
	qmlp_clear_net(net);

	for (k=0; k<net->num_layer-1; ++k) {
		m = net->topology[k];
		n = net->topology[k + 1];

		for (i=0; i<n; ++i) {
			for (j=0; j<m; ++j) {
				count = fscanf(fp, "%lf %lf %lf %lf",
					&a.r, &a.i, &a.j, &a.k);
				net->weight[k][i*m + j] = a;
				if (count < 4) {
					fprintf(stderr,
					    "qmlp: Illegal file format: `%s'\n",
					    fname);
					qmlp_destroy_net(net);
					fclose(fp);
					return NULL;
				}
			}

			count = fscanf(fp, "%lf %lf %lf", &x.i, &x.j, &x.k);
			net->threshold[k][i] = x;
			if (count < 3) {
				fprintf(stderr,
				    "qmlp: Illegal file format: `%s'\n", fname);
				qmlp_destroy_net(net);
				fclose(fp);
				return NULL;
			}
		}
	}

	fclose(fp);
	qmlp_mk_rmat(net);

	return net;
}



/*
 * ?l?b?g???[?N?????????\??????
 */
void
qmlp_print_net_state (const QMLP *net)
{
	int i, j, num, n, m, k;
	vector x;
	quaternion a;

	printf("num_layer: %d\n", net->num_layer);
	printf("topology:");
	for (k=0; k<net->num_layer; ++k) {
		printf(" %d", net->topology[k]);
	}
	printf("\n");

	printf("neuron_out:\n");
	for (k=0; k < net->num_layer; ++k) {
		printf("    L%d ", k);

		num = net->topology[k];
		for (i=0; i<num; ++i) {
			x = net->neuron_out[k][i];
			printf(" (%8.4f, %8.4f, %8.4f)", x.i, x.j, x.k);
		}
		printf("\n");
	}

	printf("neuron_back:\n");
	for (k=1; k < net->num_layer; ++k) {
		printf("    L%d ", k);
		num = net->topology[k];
		for (i=0; i<num; ++i) {
			x = net->neuron_back[k - 1][i];
			printf(" (%8.4f, %8.4f, %8.4f)", x.i, x.j, x.k);
		}
		printf("\n");
	}

	printf("weight, threshold:\n");
	for (k=0; k<net->num_layer-1; ++k) {
		m = net->topology[k];
		n = net->topology[k+1];

		printf("    L%d-%d\n", k, k+1);
		
		for (j=0; j<n; ++j) {
			printf("       ");
			for (i=0; i<m; ++i) {
				a = net->weight[k][j*m + i];
				printf(" (%8.4f, %8.4f, %8.4f, %8.4f)",
					a.r, a.i, a.j, a.k);
			}
			x = net->threshold[k][j];
			printf(" (%8.4f, %8.4f, %8.4f)", x.i, x.j, x.k);
			printf("\n");
		}
	}

	printf("dweight, dthreshold:\n");
	for (k=0; k<net->num_layer-1; ++k) {
		m = net->topology[k];
		n = net->topology[k+1];

		printf("    L%d-%d\n", k, k+1);
		
		for (j=0; j<n; ++j) {
			printf("       ");
			for (i=0; i<m; ++i) {
				a = net->dweight[k][j*m + i];
				printf(" (%8.4f, %8.4f, %8.4f, %8.4f)",
					a.r, a.i, a.j, a.k);
			}
			x = net->dthreshold[k][j];
			printf(" (%8.4f, %8.4f, %8.4f)", x.i, x.j, x.k);
			printf("\n");
		}
	}
}


/* end of file */
