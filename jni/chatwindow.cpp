#include "main.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "keyboard.h"
#include "settings.h"
#include "game/game.h"
#include "net/netgame.h"

extern CGUI *pGUI;
extern CKeyBoard *pKeyBoard;
extern CSettings *pSettings;
extern CNetGame *pNetGame;

void ChatWindowInputHandler(const char* str)
{
	if(!str || *str == '\0') return;
	if(!pNetGame) return;

	if(*str == '/')
		pNetGame->SendChatCommand(str);
	else
		pNetGame->SendChatMessage(str);
	return;
}

CChatWindow::CChatWindow()
{
	Log("Initializng Chat Window..");
	m_fChatPosX = pGUI->ScaleX( pSettings->Get().fChatPosX );
	m_fChatPosY = pGUI->ScaleY( pSettings->Get().fChatPosY );
	m_fChatSizeX = pGUI->ScaleX( pSettings->Get().fChatSizeX );
	m_fChatSizeY = pGUI->ScaleY( pSettings->Get().fChatSizeY );
	m_iMaxMessages = pSettings->Get().iChatMaxMessages;
	Log("Chat pos: %f, %f, size: %f, %f", m_fChatPosX, m_fChatPosY, m_fChatSizeX, m_fChatSizeY);

	m_dwTextColor = 0xFFFFFFFF;
	m_dwInfoColor = 0x00C8C8FF;
	m_dwDebugColor = 0xBEBEBEFF;
}

CChatWindow::~CChatWindow()
{
}

bool CChatWindow::OnTouchEvent(int type, bool multi, int x, int y)
{
	static bool bWannaOpenChat = false;

	switch(type)
	{
		case TOUCH_PUSH:
			if (x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
				y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
				bWannaOpenChat = true;
		break;

		case TOUCH_POP:
			if(bWannaOpenChat &&
				x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
				y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
			{
				pKeyBoard->Open(&ChatWindowInputHandler);
			}
			bWannaOpenChat = false;
		break;

		case TOUCH_MOVE:
		break;
	}

	return true;
}

void CChatWindow::Render()
{
	if(pSettings->Get().bDebug)
	{
		ImGui::GetOverlayDrawList()->AddRect(
			ImVec2(m_fChatPosX, m_fChatPosY), 
			ImVec2(	m_fChatPosX + m_fChatSizeX, m_fChatPosY + m_fChatSizeY), 
			IM_COL32_BLACK);
	}

	ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY);

	for(auto entry : m_ChatWindowEntries)
	{
		switch(entry.eType)
		{
			case CHAT_TYPE_CHAT:
				if(entry.szNick[0] != 0)
				{
					RenderText(entry.szNick, pos.x, pos.y, entry.dwNickColor);
					pos.x += ImGui::CalcTextSize(entry.szNick).x + ImGui::CalcTextSize(" ").x; //+ pGUI->GetFontSize() * 0.4;
				}
				RenderText(entry.utf8Message, pos.x, pos.y, entry.dwTextColor);
			break;

			case CHAT_TYPE_INFO:
			case CHAT_TYPE_DEBUG:
				RenderText(entry.utf8Message, pos.x, pos.y, entry.dwTextColor);
			break;
		}

			pos.x = m_fChatPosX;
			pos.y += pGUI->GetFontSize();
	}
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color)
{
	const int hexCount = (int)(end-start);
	if(hexCount == 6 || hexCount == 8)
	{
		char hex[9];
		strncpy(hex, start, hexCount);
		hex[hexCount] = 0;

		unsigned int hexColor = 0;
		if(sscanf(hex, "%x", &hexColor)	> 0)
		{
			color.x = static_cast< float >((hexColor & 0x00FF0000) >> 16) / 255.0f;
			color.y = static_cast< float >((hexColor & 0x0000FF00) >> 8) / 255.0f;
			color.z = static_cast< float >((hexColor & 0x000000FF)) / 255.0f;
			color.w = 1.0f;

			if(hexCount == 8)
				color.w = static_cast< float >((hexColor & 0xFF000000) >> 24) / 255.0f;

			return true;
		}
	}

	return false;
}

void CChatWindow::RenderText(const char* u8Str, float posX, float posY, uint32_t dwColor)
{
	const char* textStart = u8Str;
	const char* textCur = u8Str;
	const char* textEnd = u8Str + strlen(u8Str);

	ImVec2 posCur = ImVec2(posX, posY);
	ImColor colorCur = ImColor(dwColor);
	ImVec4 col;

	while(*textCur)
	{
		// {BBCCDD}
		// '{' и '}' соответствуют ASCII кодировке
		if(textCur[0] == '{' && ((&textCur[7] < textEnd) && textCur[7] == '}'))
		{
			// Выводим текст до фигурной скобки
			if(textCur != textStart)
			{
				// Выводим до текущего символа
				pGUI->RenderText(posCur, colorCur, true, textStart, textCur);

				// Высчитываем новое смещение
				posCur.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// Получаем цвет
			if(ProcessInlineHexColor(textCur+1, textCur+7, col))
				colorCur = col;

			// Двигаем смещение
			textCur += 7;
			textStart = textCur + 1;
		}

		textCur++;
	}

	if(textCur != textStart)
		pGUI->RenderText(posCur, colorCur, true, textStart, textCur);

	return;
}

void CChatWindow::AddChatMessage(char* szNick, uint32_t dwNickColor, char* szMessage)
{
	FilterInvalidChars(szMessage);
	AddToChatWindowBuffer(CHAT_TYPE_CHAT, szMessage, szNick, m_dwTextColor, dwNickColor);
}

void CChatWindow::AddInfoMessage(char* szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, tmp_buf, nullptr, m_dwInfoColor, 0);
}

void CChatWindow::AddDebugMessage(char *szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_DEBUG, tmp_buf, nullptr, m_dwDebugColor, 0);
}

void CChatWindow::AddClientMessage(uint32_t dwColor, char* szStr)
{
	FilterInvalidChars(szStr);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, szStr, nullptr, dwColor, 0);
}

void CChatWindow::PushBack(CHAT_WINDOW_ENTRY &entry)
{
	if(m_ChatWindowEntries.size() >= m_iMaxMessages)
		m_ChatWindowEntries.pop_front();

	m_ChatWindowEntries.push_back(entry);
	return;
}

void CChatWindow::AddToChatWindowBuffer(eChatMessageType type, char* szString, char* szNick, 
	uint32_t dwTextColor, uint32_t dwNickColor)
{
	int iBestLineLength = 0;
	CHAT_WINDOW_ENTRY entry;
	entry.eType = type;
	entry.dwNickColor = __builtin_bswap32(dwNickColor | 0x000000FF);
	entry.dwTextColor = __builtin_bswap32(dwTextColor | 0x000000FF);

	if(szNick)
	{
		strcpy(entry.szNick, szNick);
		strcat(entry.szNick, ":");
	}
	else 
		entry.szNick[0] = '\0';

	if(type == CHAT_TYPE_CHAT && strlen(szString) > MAX_LINE_LENGTH)
	{
		iBestLineLength = MAX_LINE_LENGTH;
		// Находим первый пробел с конца
		while(szString[iBestLineLength] != ' ' && iBestLineLength)
			iBestLineLength--;

		// Если последнее слово больше 12 символов
		if((MAX_LINE_LENGTH - iBestLineLength) > 12)
		{
			// Выводим до MAX_MESSAGE_LENGTH/2
			cp1251_to_utf8(entry.utf8Message, szString, MAX_LINE_LENGTH);
			PushBack(entry);

			// Выводим после MAX_MESSAGE_LENGTH/2
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+MAX_LINE_LENGTH);
			PushBack(entry);
		}
		else
		{
			// Выводим до пробела
			cp1251_to_utf8(entry.utf8Message, szString, iBestLineLength);
			PushBack(entry);

			// Выводим после пробела
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+(iBestLineLength+1));
			PushBack(entry);
		}
	}
	else
	{
		cp1251_to_utf8(entry.utf8Message, szString, MAX_MESSAGE_LENGTH);
		PushBack(entry);
	}

	return;
}

void CChatWindow::FilterInvalidChars(char *szString)
{
	while(*szString)
	{
		if(*szString > 0 && *szString < ' ')
			*szString = ' ';

		szString++;
	}
}