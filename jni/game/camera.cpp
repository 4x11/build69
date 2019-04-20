#include "../main.h"
#include "game.h"

// 0.3.7
void CCamera::SetBehindPlayer()
{
	ScriptCommand(&lock_camera_position, 0);
  	ScriptCommand(&restore_camera_to_user);
  	m_pEntity = nullptr;
  	ScriptCommand(&set_camera_behind_player);
  	ScriptCommand(&restore_camera_jumpcut);
}

// 0.3.7
void CCamera::Restore()
{
	m_pEntity = nullptr;
	ScriptCommand(&restore_camera_jumpcut);
}

// 0.3.7
void CCamera::SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ)
{
	ScriptCommand(&restore_camera_to_user);
	m_pEntity = nullptr;
	ScriptCommand(&set_camera_position, fX, fY, fZ, fRotationX, fRotationY, fRotationZ);
}


// 0.3.7
void CCamera::LookAtPoint(float fX, float fY, float fZ, int iType)
{
	ScriptCommand(&restore_camera_to_user);
	m_pEntity = nullptr;
	ScriptCommand(&point_camera, fX, fY, fZ, iType);
}

void CCamera::GetMatrix(PMATRIX4X4 Matrix)
{
	Matrix->right.X = m_matPos->right.X;
	Matrix->right.Y = m_matPos->right.Y;
	Matrix->right.Z = m_matPos->right.Z;

	Matrix->up.X = m_matPos->up.X;
	Matrix->up.Y = m_matPos->up.Y;
	Matrix->up.Z = m_matPos->up.Z;

	Matrix->at.X = m_matPos->at.X;
	Matrix->at.Y = m_matPos->at.Y;
	Matrix->at.Z = m_matPos->at.Z;

	Matrix->pos.X = m_matPos->pos.X;
	Matrix->pos.Y = m_matPos->pos.Y;
	Matrix->pos.Z = m_matPos->pos.Z;
}

// 0.3.7
void CCamera::InterpolateCameraPos(VECTOR *posFrom, VECTOR *posTo, int time, uint8_t mode)
{
	//ScriptCommand(&restore_camera_to_user);
	//ScriptCommand(&lock_camera_position, 1);
	ScriptCommand(&set_camera_pos_time_smooth, posFrom->X, posFrom->Y, posFrom->Z, posTo->X, posTo->Y, posTo->Z, time, mode);
}

// 0.3.7
void CCamera::InterpolateCameraLookAt(VECTOR *posFrom, VECTOR *posTo, int time, uint8_t mode)
{
	ScriptCommand(&lock_camera_position, 1);
	ScriptCommand(&point_camera_transverse, posFrom->X, posFrom->Y, posFrom->Z, posTo->X, posTo->Y, posTo->Z, time, mode);
}