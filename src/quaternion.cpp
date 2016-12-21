/*
 * élå≥êî
 */

#include <math.h>
#include "header.h"
//#include "quaternion.h"


/*
 * élå≥êîíËêî
 */
quaternion
q_literal (double r, double i, double j, double k)
{
	quaternion x;
	x.r = r;
	x.i = i;
	x.j = j;
	x.k = k;
	return x;
}

vector
v_literal (double i, double j, double k)
{
	vector x;
	x.i = i;
	x.j = j;
	x.k = k;
	return x;
}


/*
 * â¡éZ
 */
quaternion
q_add (quaternion a, quaternion b)
{
	quaternion c;
	c.r = a.r + b.r;
	c.i = a.i + b.i;
	c.j = a.j + b.j;
	c.k = a.k + b.k;
	return c;
}

vector
v_add (vector x, vector y)
{
	vector z;
	z.i = x.i + y.i;
	z.j = x.j + y.j;
	z.k = x.k + y.k;
	return z;
}


/*
 * å∏éZ
 */
quaternion
q_sub (quaternion a, quaternion b)
{
	quaternion c;
	c.r = a.r - b.r;
	c.i = a.i - b.i;
	c.j = a.j - b.j;
	c.k = a.k - b.k;
	return c;
}

vector
v_sub (vector x, vector y)
{
	vector z;
	z.i = x.i - y.i;
	z.j = x.j - y.j;
	z.k = x.k - y.k;
	return z;
}


/*
 * ÉmÉãÉÄÇÃ2èÊ
 */
double
q_norm2 (quaternion a)
{
	double n;
	n = a.r*a.r + a.i*a.i + a.j*a.j + a.k*a.k;
	return n;
}


double
v_norm2 (vector x)
{
	double n;
	n = x.i*x.i + x.j*x.j + x.k*x.k;
	return n;
}


/*
 * ÉmÉãÉÄ
 */
double
q_norm (quaternion a)
{
	double n;
	n = sqrt(q_norm2(a));
	return n;
}

double
v_norm (vector x)
{
	double n;
	n = sqrt(v_norm2(x));
	return n;
}


/*
 * èÊéZ
 */
quaternion
q_mul (quaternion a, quaternion b)
{
	quaternion c;
	c.r = a.r*b.r - a.i*b.i - a.j*b.j - a.k*b.k;
	c.i = a.i*b.r + a.r*b.i - a.k*b.j + a.j*b.k;
	c.j = a.j*b.r + a.k*b.i + a.r*b.j - a.i*b.k;
	c.k = a.k*b.r - a.j*b.i + a.i*b.j + a.r*b.k;
	return c;
}

quaternion
v_mul (vector x, vector y)
{
	quaternion a;
	a.r = - x.i*y.i - x.j*y.j - x.k*y.k;
	a.i = - x.k*y.j + x.j*y.k;
	a.j =   x.k*y.i - x.i*y.k;
	a.k = - x.j*y.i + x.i*y.j;
	return a;
}


/*
 * ÉXÉJÉâÅ[êœÅCÉxÉNÉgÉãêœ
 */
double
v_smul (vector x, vector y)
{
	double s;
	s = x.i*y.i + x.j*y.j + x.k*y.k;
	return s;
}

vector
v_vmul (vector x, vector y)
{
	vector z;
	z.i = - x.k*y.j + x.j*y.k;
	z.j =   x.k*y.i - x.i*y.k;
	z.k = - x.j*y.i + x.i*y.j;
	return z;
}


/*
 * é¿êîî{
 */
quaternion
q_rmul (double r, quaternion a)
{
	a.r = r*a.r;
	a.i = r*a.i;
	a.j = r*a.j;
	a.k = r*a.k;
	return a;
}

vector
v_rmul (double r, vector x)
{
	x.i = r*x.i;
	x.j = r*x.j;
	x.k = r*x.k;
	return x;
}


/*
 * aÇ…ÇÊÇÈâÒì]çsóÒÇãÅÇﬂÇÈ
 * a * v * conj(a)
 */
rot_mat *
mk_rot_mat (quaternion a, rot_mat *mat)
{
	double rr, ri, rj, rk;
	double ii, ij, ik;
	double jj, jk;
	double kk;

	rr = a.r*a.r;  ri = a.r*a.i;  rj = a.r*a.j;  rk = a.r*a.k;
	ii = a.i*a.i;  ij = a.i*a.j;  ik = a.i*a.k;
	jj = a.j*a.j;  jk = a.j*a.k;
	kk = a.k*a.k;

	mat->m[0] = rr + ii - jj - kk;
	mat->m[1] = 2.0*(ij - rk);
	mat->m[2] = 2.0*(rj + ik);
	mat->m[3] = 2.0*(rk + ij);
	mat->m[4] = rr - ii + jj - kk;
	mat->m[5] = 2.0*(jk - ri);
	mat->m[6] = 2.0*(ik - rj);
	mat->m[7] = 2.0*(ri + jk);
	mat->m[8] = rr - ii -jj + kk;

	return mat;
}


/*
 * ÉxÉNÉgÉãÇÃâÒì]
 * a * v * conj(a)
 */
vector
v_rot (quaternion a, vector x)
{
	vector z;
	rot_mat mat;

	mk_rot_mat(a, &mat);
	z = v_mat_rot(&mat, x);

	return z;
}

/*
 * âÒì]çsóÒÇ…ÇÊÇÈÉxÉNÉgÉãÇÃâÒì]
 */
vector
v_mat_rot (const rot_mat *mat, vector x)
{
	vector z;

	z.i = mat->m[0]*x.i + mat->m[1]*x.j + mat->m[2]*x.k;
	z.j = mat->m[3]*x.i + mat->m[4]*x.j + mat->m[5]*x.k;
	z.k = mat->m[6]*x.i + mat->m[7]*x.j + mat->m[8]*x.k;

	return z;
}

/*
 * ì]íuÇµÇΩâÒì]çsóÒÇ…ÇÊÇÈÉxÉNÉgÉãÇÃâÒì]ÅiãtâÒì]Åj
 */
vector
v_tr_mat_rot (const rot_mat *mat, vector x)
{
	vector z;

	z.i = mat->m[0]*x.i + mat->m[3]*x.j + mat->m[6]*x.k;
	z.j = mat->m[1]*x.i + mat->m[4]*x.j + mat->m[7]*x.k;
	z.k = mat->m[2]*x.i + mat->m[5]*x.j + mat->m[8]*x.k;

	return z;
}


/*
 * a * x * conj(b)
 */
quaternion
q_rot_by_v (vector x, vector y, quaternion a)
{
	quaternion b;
	double ii, ij, ik;
	double ji, jj, jk;
	double ki, kj, kk;
	double r00, r01, r02, r03;
	double r11, r12, r13;
	double r22, r23;
	double r33;

	ii = x.i*y.i;  ij = x.i*y.j;  ik = x.i*y.k;
	ji = x.j*y.i;  jj = x.j*y.j;  jk = x.j*y.k;
	ki = x.k*y.i;  kj = x.k*y.j;  kk = x.k*y.k;
	
	r00 = ii + jj + kk;
	r01 = kj - jk;
	r02 = ik - ki;
	r03 = ji - ij;

	r11 = ii - kk - jj;
	r12 = ji + ij;
	r13 = ki + ik;

	r22 = jj - kk - ii;
	r23 = kj + jk;

	r33 = kk - jj - ii;

	b.r = r00*a.r + r01*a.i + r02*a.j + r03*a.k;
	b.i = r01*a.r + r11*a.i + r12*a.j + r13*a.k;
	b.j = r02*a.r + r12*a.i + r22*a.j + r23*a.k;
	b.k = r03*a.r + r13*a.i + r23*a.j + r33*a.k;

	return b;
}


/*
 * óvëfìØémÇÃèÊéZ
 * component product
 */
vector
v_comp_mul (vector x, vector y)
{
	vector z;
	z.i = x.i*y.i;
	z.j = x.j*y.j;
	z.k = x.k*y.k;
	return z;
}


/*
 * ã§ñélå≥êî
 */
quaternion
q_conj (quaternion x)
{
	x.i = -x.i;
	x.j = -x.j;
	x.k = -x.k;
	return x;
}


/* end of file */
