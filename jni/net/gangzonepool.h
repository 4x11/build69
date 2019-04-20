#pragma once

#define MAX_GANG_ZONES	1024

typedef struct _GANG_ZONE
{
	float 		fPos[4];
	uint32_t	dwColor;
	uint32_t 	dwAltColor;
} GANG_ZONE;

class CGangZonePool
{
public:
	CGangZonePool();
	~CGangZonePool();

	void New(uint16_t wZone, float fMinX, float fMinY, float fMaxX, float fMaxY, uint32_t dwColor);
	void Delete(uint16_t wZone);

	void Draw();

	void Flash(uint16_t wZone, uint32_t dwColor);
	void StopFlash(uint16_t wZone);

private:
	GANG_ZONE 	*m_pGangZone[MAX_GANG_ZONES];
	bool 		m_bSlotState[MAX_GANG_ZONES];
};