#pragma once

#define PLAYER_STATE_NONE						0
#define PLAYER_STATE_ONFOOT						17
#define PLAYER_STATE_PASSENGER					18
#define PLAYER_STATE_DRIVER						19
#define PLAYER_STATE_WASTED						32
#define PLAYER_STATE_SPAWNED					33
#define PLAYER_STATE_EXIT_VEHICLE				4
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER		5
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER	6
#define PLAYER_STATE_SPECTATING					9

#define UPDATE_TYPE_NONE		0
#define UPDATE_TYPE_ONFOOT		1
#define UPDATE_TYPE_INCAR		2
#define UPDATE_TYPE_PASSENGER	3

class CRemotePlayer
{
public:
	CRemotePlayer();
	~CRemotePlayer();

	void Process();

	bool Spawn(uint8_t byteTeam, unsigned int iSkin, VECTOR *vecPos, float fRotation, uint32_t dwColor, uint8_t byteFightingStyle, bool bVisible);
	void Remove();

	CPlayerPed* GetPlayerPed() { return m_pPlayerPed; }

	void StateChange(uint8_t byteNewState, uint8_t byteOldState);
	void SetState(uint8_t byteState)
	{
		if(byteState != m_byteState)
		{
			StateChange(byteState, m_byteState);
			m_byteState = byteState;
		}
	}
	uint8_t GetState() { return m_byteState; };

	void SetID(PLAYERID playerId) { m_PlayerID = playerId; }
	PLAYERID GetID() { return m_PlayerID; }

	void SetNPC(bool bNPC) { m_bIsNPC = bNPC; }
	bool IsNPC() { return m_bIsNPC; }

	bool IsAFK() { return m_bIsAFK; }

	void SetPlayerColor(uint32_t dwColor);

	void Say(unsigned char* szText);

	bool IsActive()
	{
		if(m_pPlayerPed && m_byteState != PLAYER_STATE_NONE)
			return true;

		return false;
	}
	void RemoveFromVehicle();

	void StoreOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync, uint32_t time);
	void StoreInCarFullSyncData(INCAR_SYNC_DATA *picSync, uint32_t time);
	void StorePassengerFullSyncData(PASSENGER_SYNC_DATA *psSync);

	void UpdateOnFootPositionAndSpeed(VECTOR *vecPos, VECTOR *vecMoveSpeed);
	void UpdateOnFootTargetPosition();
	void SlerpRotation();

	void UpdateInCarMatrixAndSpeed(MATRIX4X4* mat, VECTOR* pos, VECTOR* speed);
	void UpdateInCarTargetPosition();
	void UpdateVehicleRotation();
	void HandleVehicleEntryExit();
	void EnterVehicle(VEHICLEID VehicleID, bool bPassenger);
	void ExitVehicle();

	float GetReportedHealth() { return m_fReportedHealth; };
	float GetReportedArmour() { return m_fReportedArmour; };

	uint32_t GetPlayerColor();
	void ShowGlobalMarker(short sX, short sY, short sZ);
	void HideGlobalMarker();

public:
	bool				m_bShowNameTag;
	float 				m_fReportedHealth;
	float 				m_fReportedArmour;

private:
	CPlayerPed			*m_pPlayerPed;
	CVehicle			*m_pCurrentVehicle;

	PLAYERID			m_PlayerID;
	VEHICLEID 			m_VehicleID;
	bool				m_bIsNPC;
	bool				m_bIsAFK;
	uint32_t 			m_dwMarkerID;
	uint32_t 			m_dwGlobalMarkerID;
	uint8_t 			m_byteState;
	uint8_t				m_byteUpdateFromNetwork;
	uint32_t			m_dwLastRecvTick;
	uint32_t			m_dwUnkTime;

	ONFOOT_SYNC_DATA	m_ofSync;
	INCAR_SYNC_DATA		m_icSync;
	PASSENGER_SYNC_DATA m_psSync;

	VECTOR 				m_vecPosOffset;

	VECTOR				m_vecOnFootTargetPos;
	VECTOR 				m_vecOnFootTargetSpeed;

	VECTOR 				m_vecInCarTargetPos;
	VECTOR 				m_vecInCarTargetSpeed;
	CQuaternion			m_InCarQuaternion;

	uint8_t				m_byteSpecialAction;
	uint8_t				m_byteSeatID;
};