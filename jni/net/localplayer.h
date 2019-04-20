#pragma once

// spectate
#define SPECTATE_TYPE_NONE				0
#define SPECTATE_TYPE_PLAYER			1
#define SPECTATE_TYPE_VEHICLE			2

// special action's
#define SPECIAL_ACTION_NONE				0
#define SPECIAL_ACTION_USEJETPACK		2
#define SPECIAL_ACTION_DANCE1			5
#define SPECIAL_ACTION_DANCE2			6
#define SPECIAL_ACTION_DANCE3			7
#define SPECIAL_ACTION_DANCE4			8
#define SPECIAL_ACTION_HANDSUP			10
#define SPECIAL_ACTION_USECELLPHONE		11
#define SPECIAL_ACTION_SITTING			12
#define SPECIAL_ACTION_STOPUSECELLPHONE 13
#define SPECIAL_ACTION_NIGHTVISION		14
#define SPECIAL_ACTION_THERMALVISION	15
// added in 0.3
#define SPECIAL_ACTION_DUCK 			1
#define SPECIAL_ACTION_ENTER_VEHICLE 	3
#define SPECIAL_ACTION_EXIT_VEHICLE 	4
#define SPECIAL_ACTION_DRINK_BEER		20
#define SPECIAL_ACTION_SMOKE_CIGGY		21
#define SPECIAL_ACTION_DRINK_WINE		22
#define SPECIAL_ACTION_DRINK_SPRUNK		23
#define SPECIAL_ACTION_PISSING			68
// added in 0.3e
#define SPECIAL_ACTION_CUFFED			24
// added in 0.3x
#define SPECIAL_ACTION_CARRY			25

typedef struct _PLAYER_SPAWN_INFO
{
	uint8_t byteTeam;
	int iSkin;
	uint8_t unk;
	VECTOR vecPos;
	float fRotation;
	int iSpawnWeapons[3];
	int iSpawnWeaponsAmmo[3];
} PLAYER_SPAWN_INFO;

typedef struct _ONFOOT_SYNC_DATA
{
	uint16_t lrAnalog;				// +0
	uint16_t udAnalog;				// +2
	uint16_t wKeys;					// +4
	VECTOR vecPos;					// +6
	CQuaternion quat;				// +18
	uint8_t byteHealth;				// +34
	uint8_t byteArmour;				// +35
	uint8_t byteCurrentWeapon;		// +36
	uint8_t byteSpecialAction;		// +37
	VECTOR vecMoveSpeed;			// +38
	VECTOR vecSurfOffsets;			// +50
	uint16_t wSurfInfo;				// +62
	uint32_t dwAnimation;			// 64
} ONFOOT_SYNC_DATA;					// size = 68

typedef struct _INCAR_SYNC_DATA
{
	VEHICLEID VehicleID;			// +0
	uint16_t lrAnalog;				// +2
	uint16_t udAnalog;				// +4
	uint16_t wKeys;					// +6
	CQuaternion quat;				// +8
	VECTOR vecPos;					// +24
	VECTOR vecMoveSpeed;			// +36
	float fCarHealth;				// +48
	uint8_t bytePlayerHealth;		// +52
	uint8_t bytePlayerArmour;		// +53
	uint8_t byteCurrentWeapon;		// +54
	uint8_t byteSirenOn;			// +55
	uint8_t byteLandingGearState;	// +56
	VEHICLEID TrailerID;			// +57
	float fTrainSpeed;				// +59
} INCAR_SYNC_DATA;					// size = 63

typedef struct _PASSENGER_SYNC_DATA
{
	VEHICLEID VehicleID;			// +0
	uint8_t byteSeatFlags : 7;
	uint8_t byteDriveBy : 1;		// +2
	uint8_t byteCurrentWeapon;		// +3
	uint8_t bytePlayerHealth;		// +4
	uint8_t bytePlayerArmour;		// +5
	uint16_t lrAnalog;				// +6
	uint16_t udAnalog;				// +8
	uint16_t wKeys;					// +10
	VECTOR vecPos;					// +12
} PASSENGER_SYNC_DATA;				// size = 24

enum eWeaponState
{
	WS_NO_BULLETS = 0,
	WS_LAST_BULLET = 1,
	WS_MORE_BULLETS = 2,
	WS_RELOADING = 3,
};

typedef struct _AIM_SYNC_DATA
{
	uint8_t byteCamMode;
	VECTOR vecAimf1;
	VECTOR vecAimPos;
	float fAimZ;
	uint8_t byteCamExtZoom : 6;		// 0-63 normalized
	uint8_t byteWeaponState : 2;	// see eWeaponState
	uint8_t byteAspectRatio;
} AIM_SYNC_DATA;

typedef struct _SPECTATOR_SYNC_DATA
{
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	VECTOR vecPos;
} SPECTATOR_SYNC_DATA;

class CLocalPlayer
{
public:
	CLocalPlayer();
	~CLocalPlayer();

	void ResetAllSyncAttributes();

	bool Process();

	void SendWastedNotification();

	void HandleClassSelection();
	void HandleClassSelectionOutcome();

	void SendNextClass();
	void SendPrevClass();
	void SendSpawn();

	void ApplySpecialAction(uint8_t byteSpecialAction);

	uint32_t GetPlayerColor();
	void SetPlayerColor(uint32_t dwColor);
	void RequestClass(int iClass);
	void RequestSpawn();
	bool HandlePassengerEntry();
	void UpdateSurfing();
	
	void SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger);
	void SendExitVehicleNotification(VEHICLEID VehicleID);
	void UpdateRemoteInterior(uint8_t byteInterior);
	void SetSpawnInfo(PLAYER_SPAWN_INFO *pSpawn);
	bool Spawn();
	int GetOptimumOnFootSendRate();
	uint8_t DetermineNumberOfPlayersInLocalRange();
	int GetOptimumInCarSendRate();
	void ReturnToClassSelection() { m_bWantsAnotherClass = true; }
	void ProcessSpectating();
	void ToggleSpectating(bool bToggle);
	void SpectatePlayer(PLAYERID playerId);
	void SpectateVehicle(VEHICLEID VehicleID);
	bool IsSpectating() { return m_bIsSpectating; }

	CPlayerPed * GetPlayerPed() { return m_pPlayerPed; };

	bool IsInRCMode() { return m_bInRCMode; };

	void SendOnFootFullSyncData();
	void SendInCarFullSyncData();
	void SendPassengerFullSyncData();
	void SendAimSyncData();

public:
	bool				m_bWaitingForSpawnRequestReply;

	bool				m_bIsSpectating;
	uint8_t				m_byteSpectateMode;
	uint8_t				m_byteSpectateType;
	uint32_t			m_SpectateID; // Vehicle or player id
	bool				m_bSpectateProcessed;

	VEHICLEID			m_CurrentVehicle;
	VEHICLEID 			m_LastVehicle;

private:
	CPlayerPed			*m_pPlayerPed;
	bool				m_bIsActive;
	bool				m_bIsWasted;
	uint8_t				m_byteCurInterior;
	bool				m_bInRCMode;

	uint32_t			m_dwPassengerEnterExit;

	PLAYER_SPAWN_INFO 	m_SpawnInfo;
	ONFOOT_SYNC_DATA 	m_OnFootData;
	INCAR_SYNC_DATA 	m_InCarData;
	PASSENGER_SYNC_DATA m_PassengerData;

	int					m_iSelectedClass;
	bool				m_bHasSpawnInfo;
	bool				m_bWantsAnotherClass;
	bool				m_bClearedToSpawn;

	uint32_t 			m_dwLastSendTick;
	uint32_t			m_dwLastSendSpecTick;
	uint32_t			m_dwLastSendAimTick;
	uint32_t 			m_dwLastUpdateOnFootData;
	uint32_t			m_dwLastUpdateInCarData;
	uint32_t 			m_dwLastUpdatePassengerData;
};