#pragma once

typedef struct _NEW_VEHICLE
{
	VEHICLEID 	VehicleID;
	int 		iVehicleType;
	VECTOR		vecPos;
	float 		fRotation;
	uint8_t		aColor1;
	uint8_t		aColor2;
	float		fHealth;
	uint8_t		byteInterior;
	uint32_t	dwDoorDamageStatus;
	uint32_t 	dwPanelDamageStatus;
	uint8_t		byteLightDamageStatus;
	uint8_t		byteTireDamageStatus;
	uint8_t		byteAddSiren;
	uint8_t		byteModSlots[14];
	uint8_t	  	bytePaintjob;
	uint32_t	cColor1;
	uint32_t	cColor2;
	uint8_t		byteUnk;
} NEW_VEHICLE;

class CVehiclePool
{
public:
	CVehiclePool();
	~CVehiclePool();

	void Process();

	bool New(NEW_VEHICLE* pNewVehicle);
	bool Delete(VEHICLEID VehicleID);

	CVehicle* GetAt(VEHICLEID VehicleID)
	{
		if(VehicleID >= MAX_VEHICLES || !m_bVehicleSlotState[VehicleID])
			return nullptr;
		return m_pVehicles[VehicleID];
	}

	bool GetSlotState(VEHICLEID VehicleID)
	{
		if(VehicleID >= MAX_VEHICLES)
			return false;
		return m_bVehicleSlotState[VehicleID];
	}

	VEHICLEID FindIDFromGtaPtr(VEHICLE_TYPE * pGtaVehicle);
	int FindGtaIDFromID(int ID);
	
	void AssignSpecialParamsToVehicle(VEHICLEID VehicleID, uint8_t byteObjective, uint8_t byteDoorsLocked);

	int FindNearestToLocalPlayerPed();

	void LinkToInterior(VEHICLEID VehicleID, int iInterior);

	void NotifyVehicleDeath(VEHICLEID VehicleID);

private:
	CVehicle*		m_pVehicles[MAX_VEHICLES];
	VEHICLE_TYPE*	m_pGTAVehicles[MAX_VEHICLES];
	bool			m_bVehicleSlotState[MAX_VEHICLES];

	bool			m_bIsActive[MAX_VEHICLES];
	bool			m_bIsWasted[MAX_VEHICLES];
};