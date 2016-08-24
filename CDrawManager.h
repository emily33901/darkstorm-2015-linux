#pragma once
//===================================================================================
#include "Panels.h"
#include "SDK.h"

#define RED(COLORCODE) ((int)(COLORCODE >> 24))
#define BLUE(COLORCODE) ((int)(COLORCODE >> 8) & 0xFF)
#define GREEN(COLORCODE) ((int)(COLORCODE >> 16) & 0xFF)
#define ALPHA(COLORCODE) ((int)COLORCODE & 0xFF)
#define COLORCODE(r, g, b, a) ((uint32_t)((((r)&0xff) << 24) | (((g)&0xff) << 16) | (((b)&0xff) << 8) | ((a)&0xff)))
//===================================================================================
class CDrawManager
{
public:
	void Initialize();
	void DrawString(int x, int y, uint32_t dwColor, const wchar_t *pszText);
	void DrawString(int x, int y, uint32_t dwColor, const char *pszText, ...);
	int GetESPHeight();
	int GetPixelTextSize(const char *pszText);
	int GetPixelTextSize(wchar_t *pszText);
	void DrawBox(Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius);
	void DrawRect(int x, int y, int w, int h, uint32_t dwColor);
	void OutlineRect(int x, int y, int w, int h, uint32_t dwColor);
	bool WorldToScreen(Vector &vOrigin, Vector &vScreen);
	uint32_t dwGetTeamColor(int iIndex)
	{
		static uint32_t dwColors[] = {
			0,			//Dummy
			0,			// 1 Teamone (UNUSED)
			0xFF8000FF, // 2 Teamtwo (RED)
			0x0080FFFF, // 3 teamthree (BLUE)
			0			// 4 Teamfour (UNUSED)
		};
		return dwColors[iIndex];
	}

private:
	unsigned long m_Font;
};
//===================================================================================
extern CDrawManager gDrawManager;
//===================================================================================
