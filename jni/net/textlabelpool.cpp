#include "main.h"
#include "gui/gui.h"
#include "game/game.h"
#include "chatwindow.h"
#include "netgame.h"

extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
extern CGUI *pGUI;

CText3DLabelsPool::CText3DLabelsPool()
{
	for(int x = 0; x < MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2; x++)
	{
		m_pTextLabels[x] = nullptr;
		m_bSlotState[x] = false;
	}
}

CText3DLabelsPool::~CText3DLabelsPool()
{
	for (int x = 0; x < MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2; x++)
	{
		if (m_pTextLabels[x])
		{
			m_pTextLabels[x]->text[0] = '\0';
			delete m_pTextLabels[x];
			m_pTextLabels[x] = nullptr;
		}
	}
}

void FilterColors(char* szStr)
{
	if(!szStr) return;

	char szNonColored[2048+1];
	int iNonColoredMsgLen = 0;

	for(int pos = 0; pos < strlen(szStr) && szStr[pos] != '\0'; pos++)
	{
		if(pos+7 < strlen(szStr))
		{
			if(szStr[pos] == '{' && szStr[pos+7] == '}')
			{
				pos += 7;
				continue;
			}
		}

		szNonColored[iNonColoredMsgLen] = szStr[pos];
		iNonColoredMsgLen++;
	}

	szNonColored[iNonColoredMsgLen] = 0;
	strcpy(szStr, szNonColored);
}

void CText3DLabelsPool::CreateTextLabel(int labelID, char* text, uint32_t color, 
	float posX, float posY, float posZ, float drawDistance, bool useLOS, PLAYERID attachedToPlayerID, VEHICLEID attachedToVehicleID)
{
	if (m_pTextLabels[labelID])
	{
		delete m_pTextLabels[labelID];
		m_pTextLabels[labelID] = nullptr;
		m_bSlotState[labelID] = false;
	}

	TEXT_LABELS* pTextLabel = new TEXT_LABELS;

	if (pTextLabel)
	{
		//pTextLabel->text = text;
		cp1251_to_utf8(pTextLabel->text, text);
		cp1251_to_utf8(pTextLabel->textWithoutColors, text);
		FilterColors(pTextLabel->textWithoutColors);

		pTextLabel->color = color;
		pTextLabel->pos.X = posX;
		pTextLabel->pos.Y = posY;
		pTextLabel->pos.Z = posZ;
		pTextLabel->drawDistance = drawDistance;
		pTextLabel->useLineOfSight = useLOS;
		pTextLabel->attachedToPlayerID = attachedToPlayerID;
		pTextLabel->attachedToVehicleID = attachedToVehicleID;

		pTextLabel->m_fTrueX = -1;

		if (attachedToVehicleID != INVALID_VEHICLE_ID || attachedToPlayerID != INVALID_PLAYER_ID)
		{
			pTextLabel->offsetCoords.X = posX;
			pTextLabel->offsetCoords.Y = posY;
			pTextLabel->offsetCoords.Z = posZ;
		}

		m_pTextLabels[labelID] = pTextLabel;
		m_bSlotState[labelID] = true;
	}
}

void CText3DLabelsPool::Delete(int labelID)
{
	if (m_pTextLabels[labelID])
	{
		delete m_pTextLabels[labelID];
		m_pTextLabels[labelID] = nullptr;
		m_bSlotState[labelID] = false;
	}
}

void CText3DLabelsPool::AttachToPlayer(int labelID, PLAYERID playerID, VECTOR pos)
{
	if (m_bSlotState[labelID] == true)
	{
		//tempPlayerID = playerID;
		m_pTextLabels[labelID]->attachedToPlayerID = playerID;
		m_pTextLabels[labelID]->pos = pos;
		m_pTextLabels[labelID]->offsetCoords = pos;
	}
}

void CText3DLabelsPool::AttachToVehicle(int labelID, VEHICLEID vehicleID, VECTOR pos)
{
	if (m_bSlotState[labelID] == true)
	{
		m_pTextLabels[labelID]->attachedToVehicleID = vehicleID;
		m_pTextLabels[labelID]->pos = pos;
		m_pTextLabels[labelID]->offsetCoords = pos;
	}
}

void CText3DLabelsPool::Update3DLabel(int labelID, uint32_t color, char* text)
{
	if (m_bSlotState[labelID] == true)
	{
		m_pTextLabels[labelID]->color = color;
		//m_pTextLabels[labelID]->text = text;
		cp1251_to_utf8(m_pTextLabels[labelID]->text, text);
	}
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);

void TextWithColors(ImVec2 pos, ImColor col, const char* szStr)
{
	char tempStr[4096];

	//va_list argPtr;
	//va_start(argPtr, fmt);
	//vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
	//va_end(argPtr);

	ImVec2 vecPos = pos;

	strcpy(tempStr, szStr);
	tempStr[sizeof(tempStr) - 1] = '\0';

	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	while(textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
	{
		if (*textCur == '{')
		{
			// Print accumulated text
			if (textCur != textStart)
			{
				//ImGui::TextUnformatted(textStart, textCur);
				pGUI->RenderText(vecPos, col, true, textStart, textCur);
				//ImGui::SameLine(0.0f, 0.0f);
				vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// Process color code
			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			} while (*textCur != '\0' && *textCur != '}');

			// Change color
			if (pushedColorStyle)
			{
				//ImGui::PopStyleColor();
				pushedColorStyle = false;
			}

			ImVec4 textColor;
			if (ProcessInlineHexColor(colorStart, textCur, textColor))
			{
				//ImGui::PushStyleColor(ImGuiCol_Text, textColor);
				col = textColor;
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if (*textCur == '\n')
		{
			// Print accumulated text an go to next line
			//ImGui::TextUnformatted(textStart, textCur);
			pGUI->RenderText(vecPos, col, true, textStart, textCur);
			vecPos.x = pos.x;//+= ImGui::CalcTextSize(textStart, textCur).x;
			vecPos.y += pGUI->GetFontSize();
			textStart = textCur + 1;
		}

		++textCur;
	}

	if (textCur != textStart)
		//ImGui::TextUnformatted(textStart, textCur);
	{
		pGUI->RenderText(vecPos, col, true, textStart, textCur);
		vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
	}
	else
		//ImGui::NewLine();
		vecPos.y += pGUI->GetFontSize();

	//if(pushedColorStyle)
	//	ImGui::PopStyleColor();
}


void CText3DLabelsPool::Draw()
{
	int hitEntity = 0;

	for (int x = 0; x < MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2; x++)
	{
		if (x == INVALID_TEXT_LABEL) continue;
		if (m_bSlotState[x])
		{
			CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
			CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

			//D3DXVECTOR3 textPos;
			VECTOR textPos;
			if (m_pTextLabels[x]->attachedToPlayerID != INVALID_PLAYER_ID)
			{
				if (m_pTextLabels[x]->attachedToPlayerID == pPlayerPool->GetLocalPlayerID())
				{
					//3d label for localplayer
					MATRIX4X4 matLocalPlayer;
					pPlayerPool->GetLocalPlayer()->GetPlayerPed()->GetMatrix(&matLocalPlayer);
						
					textPos.X = matLocalPlayer.pos.X + m_pTextLabels[x]->offsetCoords.X;
					textPos.Y = matLocalPlayer.pos.Y + m_pTextLabels[x]->offsetCoords.Y;
					textPos.Z = matLocalPlayer.pos.Z + m_pTextLabels[x]->offsetCoords.Z;
				}
				if (pPlayerPool->GetSlotState(m_pTextLabels[x]->attachedToPlayerID) == true)
				{
					//3d label for player (not localplayer)
					MATRIX4X4 matPlayer;
					pPlayerPool->GetAt(m_pTextLabels[x]->attachedToPlayerID)->GetPlayerPed()->GetMatrix(&matPlayer);

					textPos.X = matPlayer.pos.X + m_pTextLabels[x]->offsetCoords.X;
					textPos.Y = matPlayer.pos.Y + m_pTextLabels[x]->offsetCoords.Y;
					textPos.Z = matPlayer.pos.Z + m_pTextLabels[x]->offsetCoords.Z;
				}
			}
			else if (m_pTextLabels[x]->attachedToVehicleID != INVALID_VEHICLE_ID)
			{
				if (pVehiclePool->GetSlotState(m_pTextLabels[x]->attachedToVehicleID) == true)
				{
					MATRIX4X4 matVehicle;
					pVehiclePool->GetAt(m_pTextLabels[x]->attachedToVehicleID)->GetMatrix(&matVehicle);

					textPos.X = matVehicle.pos.X + m_pTextLabels[x]->offsetCoords.X;
					textPos.Y = matVehicle.pos.Y + m_pTextLabels[x]->offsetCoords.Y;
					textPos.Z = matVehicle.pos.Z + m_pTextLabels[x]->offsetCoords.Z;
				}
			}
			else
			{
				textPos.X = m_pTextLabels[x]->pos.X;
				textPos.Y = m_pTextLabels[x]->pos.Y;
				textPos.Z = m_pTextLabels[x]->pos.Z;
			}

			if (m_pTextLabels[x]->useLineOfSight)
			{
				MATRIX4X4 mat;
				VECTOR playerPosition;

				CAMERA_AIM *pCam = GameGetInternalAim();
				pPlayerPool->GetLocalPlayer()->GetPlayerPed()->GetMatrix(&mat);

				playerPosition.X = mat.pos.X;
				playerPosition.Y = mat.pos.Y;
				playerPosition.Z = mat.pos.Z;

				if (m_pTextLabels[x]->useLineOfSight)
					hitEntity = ScriptCommand(&get_line_of_sight,
					playerPosition.X, playerPosition.Y, playerPosition.Z,
					pCam->pos1x, pCam->pos1y, pCam->pos1z,
					1, 0, 0, 0, 0);
			}
				
			m_pTextLabels[x]->pos.X = textPos.X;
			m_pTextLabels[x]->pos.Y = textPos.Y;
			m_pTextLabels[x]->pos.Z = textPos.Z;

			if (!m_pTextLabels[x]->useLineOfSight || hitEntity)
			{
				if (pPlayerPool->GetLocalPlayer()->GetPlayerPed()->GetDistanceFromPoint(m_pTextLabels[x]->pos.X, m_pTextLabels[x]->pos.Y, m_pTextLabels[x]->pos.Z) <= m_pTextLabels[x]->drawDistance)
				{
					// pLabel->Draw(&textPos, (char *)m_pTextLabels[x]->text.c_str(), m_pTextLabels[x]->color);
					VECTOR Out;

					// CSprite::CalcScreenCoors
					(( void (*)(VECTOR*, VECTOR*, float*, float*, bool, bool))(g_libGTASA+0x54EEC0+1))(&textPos, &Out, 0, 0, 0, 0);
					if(Out.Z < 1.0f) return;

					ImVec2 pos = ImVec2(Out.X, Out.Y);

					if(m_pTextLabels[x]->m_fTrueX < 0)
					{
						char* curBegin = m_pTextLabels[x]->textWithoutColors;
						char* curPos = m_pTextLabels[x]->textWithoutColors;
						while(*curPos != '\0')
						{
							if(*curPos == '\n')
							{
								float width = ImGui::CalcTextSize(curBegin, (char*)(curPos-1)).x;
								if(width > m_pTextLabels[x]->m_fTrueX) 
								{
									m_pTextLabels[x]->m_fTrueX = width;
									Log("m_fTrueX = %f", m_pTextLabels[x]->m_fTrueX);
								}

								curBegin = curPos+1;
							}

							curPos++;
						}

						if(m_pTextLabels[x]->m_fTrueX < 0) 
						{
							m_pTextLabels[x]->m_fTrueX = ImGui::CalcTextSize(m_pTextLabels[x]->textWithoutColors).x;
						}

						//Log("m_fTrueX = %f", m_pTextLabels[x]->m_fTrueX);
					}

					pos.x -= (m_pTextLabels[x]->m_fTrueX/2);
					//pos.x -= ImGui::CalcTextSize(m_pTextLabels[x]->text).x;
					//pGUI->RenderText(pos, __builtin_bswap32(m_pTextLabels[x]->color), true, m_pTextLabels[x]->text);
					TextWithColors( pos, __builtin_bswap32(m_pTextLabels[x]->color), m_pTextLabels[x]->text );
				}	
			}
		}
	}
}