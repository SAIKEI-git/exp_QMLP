/*
 * �l����
 */

#ifndef QUATERNION_H__
#define QUATERNION_H__


/*
 * �l�����^
 */
typedef struct quaternion_ {
	double r, i, j, k;
} quaternion;

/*
 * �����l�����^�i3�����x�N�g���j
 */
typedef struct vector {
	double i, j, k;
} vector;

/*
 * ��]�s��
 */
#define ROT_MAT_SIZE	9

typedef struct rot_mat_ {
	double m[ROT_MAT_SIZE];
} rot_mat;


/*
 * �l�����萔
 */
quaternion
q_literal (double r, double i, double j, double k);

vector
v_literal (double i, double j, double k);


/*
 * ���Z
 */
quaternion
q_add (quaternion a, quaternion b);

vector
v_add (vector x, vector y);


/*
 * ���Z
 */
quaternion
q_sub (quaternion a, quaternion b);

vector
v_sub (vector x, vector y);


/*
 * �m������2��
 */
double
q_norm2 (quaternion a);

double
v_norm2 (vector x);


/*
 * �m����
 */
double
q_norm (quaternion a);

double
v_norm (vector x);


/*
 * ��Z
 */
quaternion
q_mul (quaternion a, quaternion b);

quaternion
v_mul (vector x, vector y);

/*
 * �X�J���[�ρC�x�N�g����
 */
double
v_smul (vector x, vector y);

vector
v_vmul (vector x, vector y);


/*
 * �����{
 */
quaternion
q_rmul (double r, quaternion a);

vector
v_rmul (double r, vector x);


/*
 * a�ɂ���]�s������߂�
 */
rot_mat *
mk_rot_mat (quaternion a, rot_mat *mat);


/*
 * �x�N�g���̉�]
 * a * v * conj(a)
 */
vector
v_rot (quaternion a, vector x);

/*
 * ��]�s��ɂ��x�N�g���̉�]
 */
vector
v_mat_rot (const rot_mat *mat, vector x);

/*
 * �]�u������]�s��ɂ��x�N�g���̉�]�i�t��]�j
 */
vector
v_tr_mat_rot (const rot_mat *mat, vector x);



/*
 * x * a * conj(y)
 */
quaternion
q_rot_by_v (vector x, vector y, quaternion a);


/*
 * �v�f���m�̏�Z
 * component product
 */
vector
v_comp_mul (vector x, vector y);


/*
 * ����
 */
quaternion
q_conj (quaternion a);


#endif /* QUATERNION_H__ */

/* end of file */
