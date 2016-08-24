#pragma once

typedef struct CScreenSize_t
{
	int iScreenHeight;
	int iScreenWidth;

} CScreenSize;

void Hooked_PaintTraverse(void *pPanels, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
void Intro();

extern CScreenSize gScreenSize;
