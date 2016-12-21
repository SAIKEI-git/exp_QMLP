#ifndef QMLP_H__
#define QMLP_H__

#include "quaternion.h"


/*
 * QMLP�^
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
 * �l�b�g���[�N���쐬���C�l�b�g���[�N�̑S�p�����[�^���N���A����
 */
QMLP *
qmlp_create_net (int num_layer, const int *topology);

/*
 * �l�b�g���[�N�̃��������J������
 */
void
qmlp_destroy_net (QMLP *net);


/*
 * �d��weight��臒lthreshold����l����[-range, range]�ŏ��������C
 * dweight, dthreshold���N���A����
 */
void
qmlp_init_weight (QMLP *net, double range);


/*
 * weight����rmat���쐬����
 */
void
qmlp_mk_rmat (QMLP *net);


/*
 * �������ɏo�͂��v�Z����
 */
vector *
qmlp_feed_forward (QMLP *net, vector *input, double (*sigmoid)(double));


/*
 * �ڕW�o�͂Ƃ�2��덷�����߂�
 */
double
qmlp_error (const QMLP *net, vector *target);

void
qmlp_error_rgb (const QMLP *net, vector *target, double *error);


/*
 * �덷�t�`���@�ɂ��덷���o�͂���t�Ɍv�Z���C
 * �X�V�ʂ�dweight, dthreshold�ɉ�����
 */
void
qmlp_back_propagation (QMLP *net, vector *target);


/*
 * �d�݂�臒l���X�V����dweight, dthreshold���N���A����
 */
void
qmlp_modify_weight (QMLP *net, double learning_rate);

/*
 * dweight, dthreshold���N���A����
 */
void
qmlp_clear_dweight (QMLP *net);

/*
 * �l�b�g���[�N��num_layer, topology�ȊO�̃p�����[�^���N���A����
 */
void
qmlp_clear_net (QMLP *net);


/*
 * QMLP�l�b�g���[�N�̃g�|���W�Əd�݂��t�@�C���ɏ�������
 */
int
qmlp_write_file (const QMLP *net, const char *fname);

/*
 * �t�@�C������QMLP�l�b�g���[�N�̃f�[�^��ǂݍ���
 */
QMLP *
qmlp_read_file (const char *fname);


/*
 * �l�b�g���[�N�����̏�Ԃ�\������
 */
void
qmlp_print_net_state (const QMLP *net);


#endif /* QMLP_H__ */

/* end of file */
