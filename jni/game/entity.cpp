#include "../main.h"
#include "game.h"
#include "net/netgame.h"
#include "chatwindow.h"

#include <cmath>

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

// 0.3.7
void CEntity::GetMatrix(PMATRIX4X4 Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	Matrix->right.X = m_pEntity->mat->right.X;
	Matrix->right.Y = m_pEntity->mat->right.Y;
	Matrix->right.Z = m_pEntity->mat->right.Z;

	Matrix->up.X = m_pEntity->mat->up.X;
	Matrix->up.Y = m_pEntity->mat->up.Y;
	Matrix->up.Z = m_pEntity->mat->up.Z;

	Matrix->at.X = m_pEntity->mat->at.X;
	Matrix->at.Y = m_pEntity->mat->at.Y;
	Matrix->at.Z = m_pEntity->mat->at.Z;

	Matrix->pos.X = m_pEntity->mat->pos.X;
	Matrix->pos.Y = m_pEntity->mat->pos.Y;
	Matrix->pos.Z = m_pEntity->mat->pos.Z;
}

// 0.3.7
void CEntity::SetMatrix(MATRIX4X4 Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	m_pEntity->mat->right.X = Matrix.right.X;
	m_pEntity->mat->right.Y = Matrix.right.Y;
	m_pEntity->mat->right.Z = Matrix.right.Z;

	m_pEntity->mat->up.X = Matrix.up.X;
	m_pEntity->mat->up.Y = Matrix.up.Y;
	m_pEntity->mat->up.Z = Matrix.up.Z;

	m_pEntity->mat->at.X = Matrix.at.X;
	m_pEntity->mat->at.Y = Matrix.at.Y;
	m_pEntity->mat->at.Z = Matrix.at.Z;

	m_pEntity->mat->pos.X = Matrix.pos.X;
	m_pEntity->mat->pos.Y = Matrix.pos.Y;
	m_pEntity->mat->pos.Z = Matrix.pos.Z;
}

// 0.3.7
void CEntity::GetMoveSpeedVector(PVECTOR Vector)
{
	Vector->X = m_pEntity->vecMoveSpeed.X;
	Vector->Y = m_pEntity->vecMoveSpeed.Y;
	Vector->Z = m_pEntity->vecMoveSpeed.Z;
}

// 0.3.7
void CEntity::SetMoveSpeedVector(VECTOR Vector)
{
	m_pEntity->vecMoveSpeed.X = Vector.X;
	m_pEntity->vecMoveSpeed.Y = Vector.Y;
	m_pEntity->vecMoveSpeed.Z = Vector.Z;
}

void CEntity::GetTurnSpeedVector(PVECTOR Vector)
{
	Vector->X = m_pEntity->vecTurnSpeed.X;
	Vector->Y = m_pEntity->vecTurnSpeed.Y;
	Vector->Z = m_pEntity->vecTurnSpeed.Z;
}

void CEntity::SetTurnSpeedVector(VECTOR Vector)
{
	m_pEntity->vecTurnSpeed.X = Vector.X;
	m_pEntity->vecTurnSpeed.Y = Vector.Y;
	m_pEntity->vecTurnSpeed.Z = Vector.Z;
}

// 0.3.7
uint16_t CEntity::GetModelIndex()
{
	return m_pEntity->nModelIndex;
}

// 0.3.7
bool CEntity::IsAdded()
{
	if(m_pEntity)
	{
		if(m_pEntity->vtable == g_libGTASA+0x5C7358) // CPlaceable
			return false;

		if(m_pEntity->dwUnkModelRel)
			return true;
	}

	return false;
}

// 0.3.7
bool CEntity::SetModelIndex(unsigned int uiModel)
{
	if(!m_pEntity) return false;

	int iTryCount = 0;
	if(!pGame->IsModelLoaded(uiModel) && !IsValidModel(uiModel))
	{
		pGame->RequestModel(uiModel);
		pGame->LoadRequestedModels();
		while(!pGame->IsModelLoaded(uiModel))
		{
			usleep(1000);
			if(iTryCount > 200)
			{
				if(pChatWindow) pChatWindow->AddDebugMessage("Warning: Model %u wouldn't load in time!", uiModel);
				return false;
			}

			iTryCount++;
		}
	}

	// CEntity::DeleteRWObject()
	(( void (*)(ENTITY_TYPE*))(*(void**)(m_pEntity->vtable+0x24)))(m_pEntity);
	m_pEntity->nModelIndex = uiModel;
	// CEntity::SetModelIndex()
	(( void (*)(ENTITY_TYPE*, unsigned int))(*(void**)(m_pEntity->vtable+0x18)))(m_pEntity, uiModel);

	return true;
}

// 0.3.7
void CEntity::TeleportTo(float fX, float fY, float fZ)
{
	if(m_pEntity && m_pEntity->vtable != (g_libGTASA+0x5C7358)) /* CPlaceable */
	{
		uint16_t modelIndex = m_pEntity->nModelIndex;
		if(	modelIndex != TRAIN_PASSENGER_LOCO &&
			modelIndex != TRAIN_FREIGHT_LOCO &&
			modelIndex != TRAIN_TRAM)
			(( void (*)(ENTITY_TYPE*, float, float, float, bool))(*(void**)(m_pEntity->vtable+0x3C)))(m_pEntity, fX, fY, fZ, 0);
		else
			ScriptCommand(&put_train_at, m_dwGTAId, fX, fY, fZ);
	}
}

float CEntity::GetDistanceFromCamera()
{
	MATRIX4X4 matEnt;

	if(!m_pEntity || m_pEntity->vtable == g_libGTASA+0x5C7358 /* CPlaceable */)
		return 100000.0f;

	this->GetMatrix(&matEnt);
	
	float tmpX = (matEnt.pos.X - *(float*)(g_libGTASA+0x8B1134));
	float tmpY = (matEnt.pos.Y - *(float*)(g_libGTASA+0x8B1138));
	float tmpZ = (matEnt.pos.Z - *(float*)(g_libGTASA+0x8B113C));

	return sqrt( tmpX*tmpX + tmpY*tmpY + tmpZ*tmpZ );
}

float CEntity::GetDistanceFromLocalPlayerPed()
{
	MATRIX4X4	matFromPlayer;
	MATRIX4X4	matThis;
	float 		fSX, fSY, fSZ;

	CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
	CLocalPlayer *pLocalPlayer  = nullptr;

	if(!pLocalPlayerPed) return 10000.0f;

	GetMatrix(&matThis);

	if(pNetGame)
	{
		pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		if(pLocalPlayer && (pLocalPlayer->IsSpectating() || pLocalPlayer->IsInRCMode()))
		{
			pGame->GetCamera()->GetMatrix(&matFromPlayer);
		}
		else
		{
			pLocalPlayerPed->GetMatrix(&matFromPlayer);
		}
	}
	else
	{
		pLocalPlayerPed->GetMatrix(&matFromPlayer);
	}

	fSX = (matThis.pos.X - matFromPlayer.pos.X) * (matThis.pos.X - matFromPlayer.pos.X);
	fSY = (matThis.pos.Y - matFromPlayer.pos.Y) * (matThis.pos.Y - matFromPlayer.pos.Y);
	fSZ = (matThis.pos.Z - matFromPlayer.pos.Z) * (matThis.pos.Z - matFromPlayer.pos.Z);

	return (float)sqrt(fSX + fSY + fSZ);
}

float CEntity::GetDistanceFromPoint(float X, float Y, float Z)
{
	MATRIX4X4	matThis;
	float		fSX,fSY,fSZ;

	GetMatrix(&matThis);
	fSX = (matThis.pos.X - X) * (matThis.pos.X - X);
	fSY = (matThis.pos.Y - Y) * (matThis.pos.Y - Y);
	fSZ = (matThis.pos.Z - Z) * (matThis.pos.Z - Z);
	
	return (float)sqrt(fSX + fSY + fSZ);
}