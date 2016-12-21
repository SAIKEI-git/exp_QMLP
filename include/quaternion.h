/*
 * 四元数
 */

#ifndef QUATERNION_H__
#define QUATERNION_H__


/*
 * 四元数型
 */
typedef struct quaternion_ {
	double r, i, j, k;
} quaternion;

/*
 * 純虚四元数型（3次元ベクトル）
 */
typedef struct vector {
	double i, j, k;
} vector;

/*
 * 回転行列
 */
#define ROT_MAT_SIZE	9

typedef struct rot_mat_ {
	double m[ROT_MAT_SIZE];
} rot_mat;


/*
 * 四元数定数
 */
quaternion
q_literal (double r, double i, double j, double k);

vector
v_literal (double i, double j, double k);


/*
 * 加算
 */
quaternion
q_add (quaternion a, quaternion b);

vector
v_add (vector x, vector y);


/*
 * 減算
 */
quaternion
q_sub (quaternion a, quaternion b);

vector
v_sub (vector x, vector y);


/*
 * ノルムの2乗
 */
double
q_norm2 (quaternion a);

double
v_norm2 (vector x);


/*
 * ノルム
 */
double
q_norm (quaternion a);

double
v_norm (vector x);


/*
 * 乗算
 */
quaternion
q_mul (quaternion a, quaternion b);

quaternion
v_mul (vector x, vector y);

/*
 * スカラー積，ベクトル積
 */
double
v_smul (vector x, vector y);

vector
v_vmul (vector x, vector y);


/*
 * 実数倍
 */
quaternion
q_rmul (double r, quaternion a);

vector
v_rmul (double r, vector x);


/*
 * aによる回転行列を求める
 */
rot_mat *
mk_rot_mat (quaternion a, rot_mat *mat);


/*
 * ベクトルの回転
 * a * v * conj(a)
 */
vector
v_rot (quaternion a, vector x);

/*
 * 回転行列によるベクトルの回転
 */
vector
v_mat_rot (const rot_mat *mat, vector x);

/*
 * 転置した回転行列によるベクトルの回転（逆回転）
 */
vector
v_tr_mat_rot (const rot_mat *mat, vector x);



/*
 * x * a * conj(y)
 */
quaternion
q_rot_by_v (vector x, vector y, quaternion a);


/*
 * 要素同士の乗算
 * component product
 */
vector
v_comp_mul (vector x, vector y);


/*
 * 共役
 */
quaternion
q_conj (quaternion a);


#endif /* QUATERNION_H__ */

/* end of file */
