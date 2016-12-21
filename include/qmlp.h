#ifndef QMLP_H__
#define QMLP_H__

#include "quaternion.h"


/*
 * QMLP型
 */
typedef struct qmlp_ {
	int num_layer;
	int *topology;
	vector **neuron_out, **neuron_back;
	quaternion **weight, **dweight;
	rot_mat **rmat;
	double **weight_norm;
	vector **threshold, **dthreshold;
} QMLP;


/*
 * ネットワークを作成し，ネットワークの全パラメータをクリアする
 */
QMLP *
qmlp_create_net (int num_layer, const int *topology);

/*
 * ネットワークのメモリを開放する
 */
void
qmlp_destroy_net (QMLP *net);


/*
 * 重みweightと閾値thresholdを一様乱数[-range, range]で初期化し，
 * dweight, dthresholdをクリアする
 */
void
qmlp_init_weight (QMLP *net, double range);


/*
 * weightからrmatを作成する
 */
void
qmlp_mk_rmat (QMLP *net);


/*
 * 順方向に出力を計算する
 */
vector *
qmlp_feed_forward (QMLP *net, vector *input, double (*sigmoid)(double));


/*
 * 目標出力との2乗誤差を求める
 */
double
qmlp_error (const QMLP *net, vector *target);

void
qmlp_error_rgb (const QMLP *net, vector *target, double *error);


/*
 * 誤差逆伝搬法により誤差を出力から逆に計算し，
 * 更新量をdweight, dthresholdに加える
 */
void
qmlp_back_propagation (QMLP *net, vector *target);


/*
 * 重みと閾値を更新してdweight, dthresholdをクリアする
 */
void
qmlp_modify_weight (QMLP *net, double learning_rate);

/*
 * dweight, dthresholdをクリアする
 */
void
qmlp_clear_dweight (QMLP *net);

/*
 * ネットワークのnum_layer, topology以外のパラメータをクリアする
 */
void
qmlp_clear_net (QMLP *net);


/*
 * QMLPネットワークのトポロジと重みをファイルに書き込む
 */
int
qmlp_write_file (const QMLP *net, const char *fname);

/*
 * ファイルからQMLPネットワークのデータを読み込む
 */
QMLP *
qmlp_read_file (const char *fname);


/*
 * ネットワーク内部の状態を表示する
 */
void
qmlp_print_net_state (const QMLP *net);


#endif /* QMLP_H__ */

/* end of file */
