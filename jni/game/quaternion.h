#pragma once

class CQuaternion
{
public:
	CQuaternion() : w(1), x(0), y(0), z(0) {};
	CQuaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {};

	void Set(float _x, float _y, float _z, float _w)
	{
		w = _w;
		x = _x;
		y = _y;
		z = _z;
	}

	void Set(CQuaternion q)
	{
		w = q.w;
		x = q.x;
		y = q.y;
		z = q.z;
	}

	void SetFromMatrix(MATRIX4X4 mat);
	void GetMatrix(PMATRIX4X4 mat);
	void Normalize();
	void Slerp(CQuaternion *pQ1, CQuaternion *pQ2, float t);

public:
	float w;
	float x;
	float y;
	float z;
};