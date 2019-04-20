#pragma once

#define DIALOG_STYLE_MSGBOX		0
#define DIALOG_STYLE_INPUT		1
#define DIALOG_STYLE_LIST		2
#define DIALOG_STYLE_PASSWORD	3

class CDialogWindow
{
public:
	CDialogWindow();
	~CDialogWindow();

	void Render();
	void Clear();
	void Show(bool bShow);
	void SetInfo(char* szInfo, int length);

	void DrawFormattedText();

public:
	bool		m_bIsActive;
	uint8_t 	m_byteDialogStyle;
	uint16_t	m_wDialogID;
	char		m_utf8Title[64*3 + 1];
	char*		m_putf8Info;
	char* 		m_pszInfo;
	char		m_utf8Button1[64*3 + 1];
	char		m_utf8Button2[64*3 + 1];

	float		m_fSizeX;
	float 		m_fSizeY;
};