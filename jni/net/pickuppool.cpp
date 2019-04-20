#include "main.h"
#include "game/game.h"
#include "chatwindow.h"
#include "net/netgame.h"
#include "settings.h"

extern CGame *pGame;
extern CChatWindow *pChatWindow;
extern CNetGame *pNetGame;
extern CSettings *pSettings;

CPickupPool::CPickupPool()
{
	memset(&m_Pickups[0], 0, sizeof(PICKUP)*MAX_PICKUPS);
	m_iPickupCount = 0;
	for(int i=0; i<MAX_PICKUPS; i++)
	{
		m_dwHnd[i] = 0;
		m_iTimer[i] = 0;
		m_iPickupID[i] = -1;
	}
}

CPickupPool::~CPickupPool()
{
	for(int i=0; i<MAX_PICKUPS; i++)
	{
		if(m_dwHnd[i] != 0)
			ScriptCommand(&destroy_pickup, m_dwHnd[i]);
	}
}

void CPickupPool::New(PICKUP *pPickup, int iPickup)
{
	if(m_iPickupCount >= MAX_PICKUPS || iPickup < 0 || iPickup >= MAX_PICKUPS) return;
	if(m_dwHnd[iPickup] != 0) ScriptCommand(&destroy_pickup, m_dwHnd[iPickup]);
	m_Pickups[iPickup].iModel = pPickup->iModel;
	m_Pickups[iPickup].iType = pPickup->iType;
	m_Pickups[iPickup].fX = pPickup->fX;
	m_Pickups[iPickup].fY = pPickup->fY;
	m_Pickups[iPickup].fZ = pPickup->fZ;
	m_droppedWeapon[iPickup].bDroppedWeapon = false;

	int pickupid = 0;
	m_dwHnd[iPickup] = pGame->CreatePickup(pPickup->iModel, pPickup->iType, pPickup->fX, pPickup->fY, pPickup->fZ, &pickupid);
	m_iPickupID[iPickup] = pickupid;
	m_iPickupCount++;
}

void CPickupPool::Destroy(int iPickup)
{
	if(m_iPickupCount <= 0 || iPickup < 0 || iPickup >= MAX_PICKUPS) return;
	if(m_dwHnd[iPickup] != 0)
	{
		ScriptCommand(&destroy_pickup, m_dwHnd[iPickup]);
		m_dwHnd[iPickup] = 0;
		m_iTimer[iPickup] = 0;
		m_iPickupID[iPickup] = -1;
		m_iPickupCount--;
	}
}

int CPickupPool::GetNumberFromID(int iPickup)
{
	for(int i=0; i<MAX_PICKUPS; i++)
	{
		if(m_iPickupID[i] == iPickup)
			return i;
	}

	return -1;
}

void CPickupPool::PickedUp(int iPickup)
{
	int index = GetNumberFromID(iPickup);

	Log("CPickupPool::PickedUp index = %d", index);

	if(index < 0 || index >= MAX_PICKUPS) return;
	if(m_dwHnd[index] != 0 && m_iTimer[index] == 0)
	{
		if(m_droppedWeapon[index].bDroppedWeapon) return;

		RakNet::BitStream bsPickup;
		bsPickup.Write(index);
		if(pChatWindow && pSettings->Get().bDebug) pChatWindow->AddDebugMessage("RPC: Sending RPC_PickedUpPickup (PickedUp: %d)", index);
		pNetGame->GetRakClient()->RPC(&RPC_PickedUpPickup, &bsPickup, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
		m_iTimer[index] = 15;
	}
}

void CPickupPool::Process()
{
	for(int i=0; i < MAX_PICKUPS; i++)
	{
		if(m_dwHnd[i] != 0)
		{
			if(m_droppedWeapon[i].bDroppedWeapon || m_Pickups[i].iType == 14)
			{
				if (ScriptCommand(&is_pickup_picked_up, m_dwHnd[i]))
				{
					Log("Picked up %u", i);
					RakNet::BitStream bsPickup;
					if(m_droppedWeapon[i].bDroppedWeapon)
					{
						bsPickup.Write(m_droppedWeapon[i].fromPlayer);
						pNetGame->GetRakClient()->RPC(&RPC_PickedUpPickup, &bsPickup, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
					}
					else
					{
						bsPickup.Write(i);
						pNetGame->GetRakClient()->RPC(&RPC_PickedUpPickup, &bsPickup, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
					}

				}
			}
			else if(m_iTimer[i] > 0)
				m_iTimer[i]--;
		}
	}
}