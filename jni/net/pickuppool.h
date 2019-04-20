#pragma once

#define MAX_PICKUPS 4096

#pragma pack(1)
typedef struct _PICKUP
{
	int iModel;
	int iType;
	float fX;
	float fY;
	float fZ;
} PICKUP;

typedef struct _DROPPED_WEAPON
{
	bool bDroppedWeapon;
	PLAYERID fromPlayer;
} DROPPED_WEAPON;

/*
https://github.com/BlastHackNet/mod_s0beit_sa/blob/master/src/samp.h
struct stPickupPool
{
	int				iPickupsCount;							+ 0h 		0d
	uint32_t		ul_GTA_PickupID[SAMP_MAX_PICKUPS];		+ 4h 		4d
	int				iPickupID[SAMP_MAX_PICKUPS];			+ 4004h 	16388d
	int				iTimePickup[SAMP_MAX_PICKUPS];			+ 8004h 	32772d
	uint8_t			DROPPED_WEAPON[SAMP_MAX_PICKUPS*3];		+ C004h		49152d
	struct stPickup pickup[SAMP_MAX_PICKUPS];				+ F004h		61440d
};
*/

#pragma pack(1)
class CPickupPool
{
private:
	int 			m_iPickupCount;
	uint32_t 		m_dwHnd[MAX_PICKUPS];
	uint32_t 		m_iPickupID[MAX_PICKUPS];
	uint32_t 		m_iTimer[MAX_PICKUPS];
	DROPPED_WEAPON 	m_droppedWeapon[MAX_PICKUPS];
	PICKUP 			m_Pickups[MAX_PICKUPS];


public:
	CPickupPool();
	~CPickupPool();

	void New(PICKUP* pPickup, int iPickup);
	void Destroy(int iPickup);
	void PickedUp(int iPickup);
	void Process();

	int GetNumberFromID(int id);
};