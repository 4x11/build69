#include "main.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "playertags.h"
#include "settings.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;
extern CSettings *pSettings;

CPlayerTags::CPlayerTags()
{
	Log("Loading afk_icon..");
	m_pAfk_icon = (RwTexture*)LoadTextureFromDB("samp", "afk_icon");

	HealthBarBDRColor = ImColor( 0x00, 0x00, 0x00, 0xFF );
}

CPlayerTags::~CPlayerTags() {}

// допилить
void CPlayerTags::Render()
{
	VECTOR VecPos;
	MATRIX4X4 matLocal, matPlayer;
	int dwHitEntity;
	char szNickBuf[50];

	if(pNetGame && pNetGame->m_bShowPlayerTags)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		pGame->FindPlayerPed()->GetMatrix(&matLocal);

		for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
		{
			if(pPlayerPool->GetSlotState(playerId) == true)
			{
				CRemotePlayer* pPlayer = pPlayerPool->GetAt(playerId);

				if(pPlayer && pPlayer->IsActive() && pPlayer->m_bShowNameTag)
				{
					CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();

					if(pPlayerPed->GetDistanceFromCamera() <= pNetGame->m_fNameTagDrawDistance)
					{
						/*if(	pPlayer->GetState() == PLAYER_STATE_DRIVER && 
							pPlayer->m_pCurrentVehicle &&
							pPlayer->m_pCurrentVehicle->IsRCVehicle())
						{
							pPlayer->m_pCurrentVehicle->GetMatrix(matPlayer);
							VecPos.X = matPlayer.pos.X;
							VecPos.Y = matPlayer.pos.Y;
							VecPos.Z = matPlayer.pos.Z;
						}
						else*/
						{
							if(!pPlayerPed->IsAdded()) continue;
							VecPos.X = 0.0f;
							VecPos.Y = 0.0f;
							VecPos.Z = 0.0f;
							pPlayerPed->GetBonePosition(8, &VecPos);
						}

						CAMERA_AIM *pCam = GameGetInternalAim();
						dwHitEntity = 0;

						if(pNetGame->m_bNameTagLOS)
						{
							dwHitEntity = ScriptCommand(&get_line_of_sight, 
								VecPos.X, VecPos.Y, VecPos.Z,
								pCam->pos1x, pCam->pos1y, pCam->pos1z,
								1, 0, 0, 1, 0);
						}

						if(!pNetGame->m_bNameTagLOS || dwHitEntity)
						{
							sprintf(szNickBuf, "%s (%d)", pPlayerPool->GetPlayerName(playerId), playerId);
							Draw(&VecPos, szNickBuf,
								pPlayer->GetPlayerColor(),
								pPlayerPed->GetDistanceFromCamera(),
								pPlayer->m_fReportedHealth,
								pPlayer->m_fReportedArmour,
								pPlayer->IsAFK());
						}
					}
				}
			}
		}
	}
}


#pragma pack(1)
typedef struct _RECT
{
	float x; // +0
	float y; // +4
	float x1; // +8
	float y1; // +12
} RECT, *PRECT;

void CPlayerTags::Draw(VECTOR* vec, char* szName, uint32_t dwColor, 
	float fDist, float fHealth, float fArmour, bool bAfk)
{
	VECTOR TagPos;

	TagPos.X = vec->X;
	TagPos.Y = vec->Y;
	TagPos.Z = vec->Z;
	TagPos.Z += 0.25f + (fDist * 0.0475f);

	VECTOR Out;
	// CSprite::CalcScreenCoors
	(( void (*)(VECTOR*, VECTOR*, float*, float*, bool, bool))(g_libGTASA+0x54EEC0+1))(&TagPos, &Out, 0, 0, 0, 0);

	if(Out.Z < 1.0f)
		return;

	// name (id)
	ImVec2 pos = ImVec2(Out.X, Out.Y);
	pos.x -= ImGui::CalcTextSize(szName).x/2;
	pGUI->RenderText(pos, __builtin_bswap32(dwColor | (0x000000FF)), true, szName);

	// Health Bar
	if(fHealth < 0.0f) return;

	// округляем
	Out.X = (float)((int)Out.X);
	Out.Y = (float)((int)Out.Y);

	HealthBarColor = ImColor( 0xB9, 0x22, 0x28, 0xFF );
	HealthBarBGColor = ImColor( 0x4B, 0x0B, 0x14, 0xFF );

	float fWidth = pGUI->ScaleX( pSettings->Get().fHealthBarWidth );
	float fHeight = pGUI->ScaleY( pSettings->Get().fHealthBarHeight );
	float fOutline = (float)pSettings->Get().iFontOutline;

	// top left
	HealthBarBDR1.x = Out.X - ((fWidth/2) + fOutline);
	HealthBarBDR1.y = Out.Y + (pGUI->GetFontSize()*1.2f);//35.0f;
	// bottom right
	HealthBarBDR2.x = Out.X + ((fWidth/2) + fOutline);
	HealthBarBDR2.y = Out.Y + (pGUI->GetFontSize()*1.2f) + fHeight;//48.0f;

	// top left
	HealthBarBG1.x = HealthBarBDR1.x + fOutline;//Out.X - 40.0f;
	HealthBarBG1.y = HealthBarBDR1.y + fOutline;//Out.Y + 37.0f;
	// bottom right
	HealthBarBG2.x = HealthBarBDR2.x - fOutline;//Out.X + 40.0f;
	HealthBarBG2.y = HealthBarBDR2.y - fOutline;//Out.Y + 46.0f;

	// top left
	HealthBar1.x = HealthBarBG1.x;//Out.X - 40.0f;
	HealthBar1.y = HealthBarBG1.y;//Out.Y + 37.0f;
	// bottom right
	HealthBar2.y = HealthBarBG2.y;//Out.Y + 46.0f;

	if (fHealth > 100.0f)
		fHealth = 100.0f;

	fHealth *= fWidth/100.0f;
	fHealth -= (fWidth/2);
	HealthBar2.x = Out.X + fHealth;

	if(fArmour > 0.0f)
	{
		HealthBarBDR1.y += 13.0f;
		HealthBarBDR2.y += 13.0f;
		HealthBarBG1.y += 13.0f;
		HealthBarBG2.y += 13.0f;
		HealthBar1.y += 13.0f;
		HealthBar2.y += 13.0f;
	}

	ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
	ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
	ImGui::GetOverlayDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);

	// Armour Bar
	if(fArmour > 0.0f)
	{
		HealthBarBDR1.y -= 13.0f;
		HealthBarBDR2.y -= 13.0f;
		HealthBarBG1.y -= 13.0f;
		HealthBarBG2.y -= 13.0f;
		HealthBar1.y -= 13.0f;
		HealthBar2.y -= 13.0f;

		HealthBarColor = ImColor(200, 200, 200, 255);
		HealthBarBGColor = ImColor(40, 40, 40, 255);

		if(fArmour > 100.0f)
			fArmour = 100.0f;

		fArmour *= fWidth/100.0f;
		fArmour -= (fWidth/2);
		HealthBar2.x = Out.X + fArmour;
		ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
		ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
		ImGui::GetOverlayDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);
	}

	// AFK Icon
	if(bAfk)
	{
		ImVec2 a = ImVec2(HealthBarBDR1.x - (pGUI->GetFontSize()*1.4f), HealthBarBDR1.y);
		ImVec2 b = ImVec2(a.x + (pGUI->GetFontSize()*1.3f), a.y + (pGUI->GetFontSize()*1.3f));
		ImGui::GetOverlayDrawList()->AddImage((ImTextureID)m_pAfk_icon->raster, a, b);
	}
}