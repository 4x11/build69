#pragma once

class CPlayerTags
{
public:
	CPlayerTags();
	~CPlayerTags();

	void Render();

private:
	ImVec2 HealthBarBDR1;
	ImVec2 HealthBarBDR2;
	ImVec2 HealthBarBG1;
	ImVec2 HealthBarBG2;
	ImVec2 HealthBar1;
	ImVec2 HealthBar2;

	ImColor HealthBarColor;
	ImColor HealthBarBGColor;
	ImColor HealthBarBDRColor;

	RwTexture* m_pAfk_icon; 

	void Draw(VECTOR* vec, char* szNick, uint32_t dwColor, 
	float fDist, float fHealth, float fArmour, bool bAfk);
};