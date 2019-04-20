#include "main.h"
#include "game.h"

extern CGame *pGame;

CVehicle::CVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation, bool bSiren)
{
	MATRIX4X4 mat;
	uint32_t dwRetID = 0;

	m_pVehicle = nullptr;
	m_dwGTAId = 0;
	m_pTrailer = nullptr;

	if( (iType != TRAIN_PASSENGER_LOCO) &&
		(iType != TRAIN_FREIGHT_LOCO) &&
		(iType != TRAIN_PASSENGER) &&
		(iType != TRAIN_FREIGHT) &&
		(iType != TRAIN_TRAM)) 
	{
		// normal vehicle
		if(!pGame->IsModelLoaded(iType))
		{
			pGame->RequestModel(iType);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(iType)) usleep(10);
		}

		if(bSiren)
		{
			// допилить
		}
		else
		{

		}

		ScriptCommand(&create_car, iType, fPosX, fPosY, fPosZ, &dwRetID);
		ScriptCommand(&set_car_z_angle, dwRetID, fRotation);
		ScriptCommand(&car_gas_tank_explosion,dwRetID, 0);
		ScriptCommand(&set_car_hydraulics, dwRetID, 0);
		ScriptCommand(&toggle_car_tires_vulnerable, dwRetID, 0);

		m_pVehicle = (VEHICLE_TYPE*)GamePool_Vehicle_GetAt(dwRetID);
		m_pEntity = (ENTITY_TYPE*)m_pVehicle;
		m_dwGTAId = dwRetID;

		if(m_pVehicle)
		{
			m_pVehicle->dwDoorsLocked = 0;
			m_bIsLocked = false;

			GetMatrix(&mat);
			mat.pos.X = fPosX;
			mat.pos.Y = fPosY;
			mat.pos.Z = fPosZ;

			if( GetVehicleSubtype() != VEHICLE_SUBTYPE_BIKE && 
				GetVehicleSubtype() != VEHICLE_SUBTYPE_PUSHBIKE)
				mat.pos.Z += 0.25f;

			SetMatrix(mat);
		}
	}
	else if((iType == TRAIN_PASSENGER_LOCO) ||
			(iType == TRAIN_FREIGHT_LOCO) ||
			(iType == TRAIN_TRAM))
	{
		// train locomotives
	}
	else if((iType == TRAIN_PASSENGER) ||
			iType == TRAIN_FREIGHT)
	{

	}

	m_byteObjectiveVehicle = 0;
	m_bSpecialMarkerEnabled = false;
	m_bDoorsLocked = false;
	m_dwMarkerID = 0;
	m_bIsInvulnerable = false;
}

CVehicle::~CVehicle()
{
	m_pVehicle = GamePool_Vehicle_GetAt(m_dwGTAId);

	if(m_pVehicle)
	{
		if(m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}

		RemoveEveryoneFromVehicle();

		if(m_pTrailer)
		{
			// detach trailer (допилить)
		}

		// тут еще какая-то интересная шняга

		if( m_pVehicle->entity.nModelIndex == TRAIN_PASSENGER_LOCO ||
			m_pVehicle->entity.nModelIndex == TRAIN_FREIGHT_LOCO )
		{
			ScriptCommand(&destroy_train, m_dwGTAId);
		}
		else
		{
			int nModelIndex = m_pVehicle->entity.nModelIndex;
			ScriptCommand(&destroy_car, m_dwGTAId);

			if( !GetModelReferenceCount(nModelIndex) &&
				//!m_bKeepModelLoaded &&
				//(pGame->GetVehicleModelsCount() > 80) &&
				pGame->IsModelLoaded(nModelIndex))
			{
				// CStreaming::RemoveModel
				(( void (*)(int))(g_libGTASA+0x290C4C+1))(nModelIndex);
			}
		}
	}
}

void CVehicle::LinkToInterior(int iInterior)
{
	if(GamePool_Vehicle_GetAt(m_dwGTAId)) 
	{
		ScriptCommand(&link_vehicle_to_interior, m_dwGTAId, iInterior);
	}
}

void CVehicle::SetColor(int iColor1, int iColor2)
{
	if(m_pVehicle)
	{
		if(GamePool_Vehicle_GetAt(m_dwGTAId))
		{
			m_pVehicle->byteColor1 = (uint8_t)iColor1;
			m_pVehicle->byteColor2 = (uint8_t)iColor2;
		}
	}

	m_byteColor1 = (uint8_t)iColor1;
	m_byteColor2 = (uint8_t)iColor2;
	m_bColorChanged = true;
}

void CVehicle::SetHealth(float fHealth)
{
	if(m_pVehicle)
	{
		m_pVehicle->fHealth = fHealth;
	}
}

float CVehicle::GetHealth()
{
	if(m_pVehicle) return m_pVehicle->fHealth;
	else return 0.0f;
}

// 0.3.7
void CVehicle::SetInvulnerable(bool bInv)
{
	if(!m_pVehicle) return;
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) return;
	if(m_pVehicle->entity.vtable == g_libGTASA+0x5C7358) return;

	if(bInv) 
	{
		ScriptCommand(&set_car_immunities, m_dwGTAId, 1,1,1,1,1);
		ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 0);
		m_bIsInvulnerable = true;
	} 
	else 
	{ 
		ScriptCommand(&set_car_immunities, m_dwGTAId, 0,0,0,0,0);
		ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 1);
		m_bIsInvulnerable = false;
	}
}

// 0.3.7
bool CVehicle::IsDriverLocalPlayer()
{
	if(m_pVehicle)
	{
		if((PED_TYPE*)m_pVehicle->pDriver == GamePool_FindPlayerPed())
			return true;
	}

	return false;
}

// 0.3.7
bool CVehicle::HasSunk()
{
	if(!m_pVehicle) return false;
	return ScriptCommand(&has_car_sunk, m_dwGTAId);
}

void CVehicle::RemoveEveryoneFromVehicle()
{
	if(!m_pVehicle) return;
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

	float fPosX = m_pVehicle->entity.mat->pos.X;
	float fPosY = m_pVehicle->entity.mat->pos.Y;
	float fPosZ = m_pVehicle->entity.mat->pos.Z;

	int iPlayerID = 0;
	if(m_pVehicle->pDriver)
	{
		iPlayerID = GamePool_Ped_GetIndex( m_pVehicle->pDriver );
		ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2.0f);
	}

	for(int i = 0; i<7; i++)
	{
		if(m_pVehicle->pPassengers[i] != nullptr)
		{
			iPlayerID = GamePool_Ped_GetIndex( m_pVehicle->pPassengers[i] );
			ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2.0f);
		}
	}
}

// 0.3.7
bool CVehicle::IsOccupied()
{
	if(m_pVehicle)
	{
		if(m_pVehicle->pDriver) return true;
		if(m_pVehicle->pPassengers[0]) return true;
		if(m_pVehicle->pPassengers[1]) return true;
		if(m_pVehicle->pPassengers[2]) return true;
		if(m_pVehicle->pPassengers[3]) return true;
		if(m_pVehicle->pPassengers[4]) return true;
		if(m_pVehicle->pPassengers[5]) return true;
		if(m_pVehicle->pPassengers[6]) return true;
	}

	return false;
}

void CVehicle::ProcessMarkers()
{
	if(!m_pVehicle) return;

	if(m_byteObjectiveVehicle)
	{
		if(!m_bSpecialMarkerEnabled)
		{
			if(m_dwMarkerID)
			{
				ScriptCommand(&disable_marker, m_dwMarkerID);
				m_dwMarkerID = 0;
			}

			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 3, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1006);
			ScriptCommand(&show_on_radar, m_dwMarkerID, 3);
			m_bSpecialMarkerEnabled = true;
		}

		return;
	}

	if(m_byteObjectiveVehicle && m_bSpecialMarkerEnabled)
	{
		if(m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_bSpecialMarkerEnabled = false;
			m_dwMarkerID = 0;
		}
	}

	if(GetDistanceFromLocalPlayerPed() < 200.0f && !IsOccupied())
	{
		if(!m_dwMarkerID)
		{
			// show
			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 2, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1004);
		}
	}

	else if(IsOccupied() || GetDistanceFromLocalPlayerPed() >= 200.0f)
	{
		// remove
		if(m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}
	}
}

void CVehicle::SetWheelPopped(uint8_t bytePopped)
{

}

void CVehicle::SetDoorState(int iState)
{
	if(iState) 
	{
		m_pVehicle->dwDoorsLocked = 2;
		m_bDoorsLocked = true;
	} 
	else 
	{
		m_pVehicle->dwDoorsLocked = 0;
		m_bDoorsLocked = false;
	}
}

void CVehicle::SetEngineState(int iState)
{
	/*uint8_t flag = *(uint8_t*)(m_pVehicle + 0x42D) & 2;

     if (flag)
    {
        *(uint8_t*)(m_pVehicle + 0x428) &= 0xEF;
      }
      else
      {
        if ( ScriptParams[1] )
          v149 = 1;
        *(_BYTE *)(this + 0x428) = *(_BYTE *)(this + 0x428) & 0xEF | 16 * (v149 & 1);
      }*/

	ScriptCommand(&turn_car_engine, m_dwGTAId, iState);
}

void CVehicle::UpdateDamageStatus(uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage)
{

}

unsigned int CVehicle::GetVehicleSubtype()
{
	if(m_pVehicle)
	{
		if(m_pVehicle->entity.vtable == g_libGTASA+0x5CC9F0) // 0x871120
		{
			return VEHICLE_SUBTYPE_CAR;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCD48) // 0x8721A0
		{
			return VEHICLE_SUBTYPE_BOAT;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCB18) // 0x871360
		{
			return VEHICLE_SUBTYPE_BIKE;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CD0B0) // 0x871948
		{
			return VEHICLE_SUBTYPE_PLANE;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCE60) // 0x871680
		{
			return VEHICLE_SUBTYPE_HELI;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCC30) // 0x871528
		{
			return VEHICLE_SUBTYPE_PUSHBIKE;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CD428) // 0x872370
		{
			return VEHICLE_SUBTYPE_TRAIN;
		}
	}

	return 0;
}