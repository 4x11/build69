#include "main.h"
#include "game/game.h"
#include "netgame.h"

extern CGame *pGame;

CGangZonePool::CGangZonePool()
{
	for(uint16_t wZone = 0; wZone < MAX_GANG_ZONES; wZone++)
	{
		m_pGangZone[wZone] = nullptr;
		m_bSlotState[wZone] = false;
	}
}

CGangZonePool::~CGangZonePool()
{
	for(uint16_t wZone = 0; wZone < MAX_GANG_ZONES; wZone++)
	{
		if(m_pGangZone[wZone])
		{
			delete m_pGangZone[wZone];
			m_pGangZone[wZone] = nullptr;
		}
	}
}

void CGangZonePool::New(uint16_t wZone, float fMinX, float fMinY, float fMaxX, float fMaxY, uint32_t dwColor)
{
	if(m_pGangZone[wZone])
	{
		delete m_pGangZone[wZone];
		m_pGangZone[wZone] = nullptr;
		m_bSlotState[wZone] = false;
	}

	GANG_ZONE *pGangZone = new GANG_ZONE;
	if(pGangZone)
	{
		pGangZone->fPos[0] = fMinX;
		pGangZone->fPos[1] = fMaxY;
		pGangZone->fPos[2] = fMaxX;
		pGangZone->fPos[3] = fMinY;
		pGangZone->dwColor = dwColor;
		pGangZone->dwAltColor = dwColor;
		m_pGangZone[wZone] = pGangZone;
		m_bSlotState[wZone] = true;
	}
}

void CGangZonePool::Delete(uint16_t wZone)
{
	if(m_pGangZone[wZone])
	{
		delete m_pGangZone[wZone];
		m_pGangZone[wZone] = nullptr;
		m_bSlotState[wZone] = false;
	}
}

void CGangZonePool::Draw()
{
	static uint32_t dwLastTick = 0;
	uint32_t dwTick = GetTickCount();
	static uint8_t alt = 0;

	if(dwTick - dwLastTick >= 500)
	{
		alt = ~alt;
		dwLastTick = dwTick;
	}

	for(uint16_t wZone = 0; wZone < MAX_GANG_ZONES; wZone++)
	{
		if(m_bSlotState[wZone])
		{
			pGame->DrawGangZone(m_pGangZone[wZone]->fPos, 
				alt ? m_pGangZone[wZone]->dwAltColor : m_pGangZone[wZone]->dwColor);
		}
	}
}

void CGangZonePool::Flash(uint16_t wZone, uint32_t dwColor)
{
	if(m_pGangZone[wZone])
		m_pGangZone[wZone]->dwAltColor = dwColor;
}

void CGangZonePool::StopFlash(uint16_t wZone)
{
	if(m_pGangZone[wZone])
		m_pGangZone[wZone]->dwAltColor = m_pGangZone[wZone]->dwColor;
}