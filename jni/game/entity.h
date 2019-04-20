#pragma once

class CEntity
{
public:
	CEntity() {};
	virtual ~CEntity() {};

	// 0.3.7
	void GetMatrix(PMATRIX4X4 Matrix);
	// 0.3.7
	void SetMatrix(MATRIX4X4 Matrix);
	// 0.3.7
	void GetMoveSpeedVector(PVECTOR Vector);
	// 0.3.7
	void SetMoveSpeedVector(VECTOR Vector);

	void GetTurnSpeedVector(PVECTOR Vector);
	void SetTurnSpeedVector(VECTOR Vector);
	
	// 0.3.7
	uint16_t GetModelIndex();
	// 0.3.7
	void TeleportTo(float fX, float fY, float fZ);

	bool SetModelIndex(unsigned int uiModel);
	// 0.3.7
	bool IsAdded();
	// 0.3.7
	float GetDistanceFromCamera();
	float GetDistanceFromLocalPlayerPed();
	float GetDistanceFromPoint(float x, float y, float z);

public:
	ENTITY_TYPE		*m_pEntity;
	uint32_t		m_dwGTAId;
};