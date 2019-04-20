#pragma once

#define MAX_TEXT_LABELS			1024
#define MAX_PLAYER_TEXT_LABELS	1024
#define INVALID_TEXT_LABEL		1025

#define MAX_LABELS_LENGTH		2048

#pragma pack(1)
typedef struct _TEXT_LABELS
{
	char text[2048+1];
	char textWithoutColors[2048+1];
	uint32_t color;
	VECTOR pos;
	float drawDistance;
	int virtualWorld;
	bool useLineOfSight;
	PLAYERID attachedToPlayerID;
	VEHICLEID attachedToVehicleID;
	VECTOR offsetCoords;
	float m_fTrueX;
} TEXT_LABELS;

class CText3DLabelsPool
{
private:
	TEXT_LABELS			*m_pTextLabels[MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2];
	bool				m_bSlotState[MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2];

public:
	CText3DLabelsPool();
	~CText3DLabelsPool();

	void CreateTextLabel(int labelID, char* text, uint32_t color, 
		float posX, float posY, float posZ, float drawDistance,
		 bool useLOS, PLAYERID attachedToPlayerID, VEHICLEID attachedToVehicleID);
	void Delete(int labelID);
	void AttachToPlayer(int labelID, PLAYERID playerID, VECTOR pos);
	void AttachToVehicle(int labelID, VEHICLEID vehicleID, VECTOR pos);
	void Update3DLabel(int labelID, uint32_t color, char* text);
	void Draw();
};