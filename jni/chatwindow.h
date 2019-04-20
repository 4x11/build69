#pragma once

#define MAX_MESSAGE_LENGTH	144
#define MAX_LINE_LENGTH		MAX_MESSAGE_LENGTH / 2

enum eChatMessageType
{
	CHAT_TYPE_NONE = 0,
	CHAT_TYPE_CHAT,
	CHAT_TYPE_INFO,
	CHAT_TYPE_DEBUG
};

typedef struct
{
	eChatMessageType	eType;
	char 				utf8Message[MAX_MESSAGE_LENGTH * 3 + 1];
	char 				szNick[MAX_PLAYER_NAME + 1];
	uint32_t			dwTextColor;
	uint32_t 			dwNickColor;
} CHAT_WINDOW_ENTRY;

class CChatWindow
{
	friend class CGUI;
public:
	CChatWindow();
	~CChatWindow();

	void AddChatMessage(char* szNick, uint32_t dwNickColor, char* szMessage);
	void AddInfoMessage(char* szFormat, ...);
	void AddDebugMessage(char* szFormat, ...);
	void AddClientMessage(uint32_t dwColor, char* szStr);

protected:
	void Render();
	bool OnTouchEvent(int type, bool multi, int x, int y);

public:
	void RenderText(const char* u8Str, float posX, float posY, uint32_t dwColor);
private:
	void FilterInvalidChars(char* szString);
	void AddToChatWindowBuffer(eChatMessageType eType, char* szString, char* szNick, 
		uint32_t dwTextColor, uint32_t dwNickColor);
	void PushBack(CHAT_WINDOW_ENTRY &entry);

private:
	float m_fChatPosX, m_fChatPosY;
	float m_fChatSizeX, m_fChatSizeY;

	int m_iMaxMessages;
	std::list<CHAT_WINDOW_ENTRY> m_ChatWindowEntries;

	uint32_t m_dwTextColor;
	uint32_t m_dwInfoColor;
	uint32_t m_dwDebugColor;
};