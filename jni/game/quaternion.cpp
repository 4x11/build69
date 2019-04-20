#include "main.h"
#include "game.h"
#include <cmath>

void CQuaternion::SetFromMatrix(MATRIX4X4 mat)
{
	w = sqrt( std::max( (float)0, 1.0f + mat.right.X + mat.up.Y + mat.at.Z ) ) * 0.5f;
	x = sqrt( std::max( (float)0, 1.0f + mat.right.X - mat.up.Y - mat.at.Z ) ) * 0.5f;
	y = sqrt( std::max( (float)0, 1.0f - mat.right.X + mat.up.Y - mat.at.Z ) ) * 0.5f;
	z = sqrt( std::max( (float)0, 1.0f - mat.right.X - mat.up.Y + mat.at.Z ) ) * 0.5f;

	x = static_cast < float > ( copysign( x, mat.at.Y - mat.up.Z ) );
	y = static_cast < float > ( copysign( y, mat.right.Z - mat.at.X ) );
	z = static_cast < float > ( copysign( z, mat.up.X - mat.right.Y ) );
}

void CQuaternion::GetMatrix(PMATRIX4X4 mat)
{
	if(!mat) return;

	float sqw = w * w; // v13 = a1 * a1;
	float sqx = x * x; // v14 = a2 * a2;
	float sqy = y * y; // v15 = a3 * a3;
	float sqz = z * z; // v16 = a4 * a4;

	mat->right.X = ( sqx - sqy - sqz + sqw); 	// a5 = v14 - v15 - v16 + v13;
	mat->up.Y = (-sqx + sqy - sqz + sqw);		// a9 = v15 - v14 - v16 + v13;
	mat->at.Z = (-sqx - sqy + sqz + sqw);		// a13 = v16 - (v15 + v14) + v13;

	float tmp1 = x * y;				// v17 = a2 * a3;
	float tmp2 = z * w;				// v18 = a1 * a4;
	mat->up.X = 2.0 * (tmp1 + tmp2);		// a8 = v18 + v17 + v18 + v17;
	mat->right.Y = 2.0 * (tmp1 - tmp2);		// a6 = v17 - v18 + v17 - v18;

	tmp1 = x*z;						// v20 = a2 * a4;
	tmp2 = y*w;						// v21 = a1 * a3;
	mat->at.X = 2.0 * (tmp1 - tmp2);		// a11 = v20 - v21 + v20 - v21;
	mat->right.Z = 2.0 * (tmp1 + tmp2);		// a7 = v21 + v20 + v21 + v20;
	tmp1 = y*z;						// v22 = a3 * a4;
	tmp2 = x*w;						// v23 = a1 * a2;
	mat->at.Y = 2.0 * (tmp1 + tmp2);		// a12 = v23 + v22 + v23 + v22;
	mat->up.Z = 2.0 * (tmp1 - tmp2);		// a10 = v22 - v23 + v22 - v23;
}

void CQuaternion::Normalize()
{
	double n = sqrt(x*x + y*y + z*z + w*w);

	w /= n;
	x /= n;
	y /= n;
	z /= n;
}

#define SLERP_DELTA 0.1
void CQuaternion::Slerp(CQuaternion *pQ1, CQuaternion *pQ2, float t)
{
	CQuaternion *pOut = this;

	if(!pQ1 || !pQ2) return;
	if(t > 1) return;

	float p1[4];
	double omega, cosom, sinom, scale0, scale1;
	cosom = pQ1->x*pQ2->x + pQ1->y*pQ2->y + pQ1->z*pQ2->z + pQ1->w*pQ2->w;

	if(cosom < 0.0)
	{
		cosom = -cosom;
		p1[0] = - pQ2->x;  p1[1] = - pQ2->y;
		p1[2] = - pQ2->z;  p1[3] = - pQ2->w;
	}	
	else
	{
		p1[0] = pQ2->x;    p1[1] = pQ2->y;
		p1[2] = pQ2->z;    p1[3] = pQ2->w;
	}

	if((1.0 - cosom) > SLERP_DELTA)
	{
		// стандартный случай (slerp)
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0 - t) * omega) / sinom;
		scale1 = sin(t * omega) / sinom;
	}
	else
	{
		// если маленький угол - линейная интерполяция
		scale0 = 1.0 - t;
		scale1 = t;
	}
	Set(scale0 * pQ1->x + scale1 * p1[0],
		scale0 * pQ1->y + scale1 * p1[1],
		scale0 * pQ1->z + scale1 * p1[2],
		scale0 * pQ1->w + scale1 * p1[3]);
}