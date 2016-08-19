#include "SDK.h"
#include "Panels.h"
#include "CDrawManager.h"

CScreenSize gScreenSize;
//===================================================================================
void Hooked_PaintTraverse( PVOID pPanels, unsigned int vguiPanel, bool forceRepaint, bool allowForce )
{
	try
	{
		VMTManager& hook = VMTManager::GetHook(pPanels); //Get a pointer to the instance of your VMTManager with the function GetHook.
		hook.GetMethod<void(__thiscall*)(PVOID, unsigned int, bool, bool)>(gOffsets.iPaintTraverseOffset)(pPanels, vguiPanel, forceRepaint, allowForce); //Call the original.

		static unsigned int vguiMatSystemTopPanel;

		if (vguiMatSystemTopPanel == 0)
		{
			const char* szName = gInts.Panels->GetName(vguiPanel);
			if( szName[0] == 'M' && szName[3] == 'S') //Look for MatSystemTopPanel without using slow operations like strcmp or strstr.
			{
				vguiMatSystemTopPanel = vguiPanel;
				Intro();
			}
		}
		
		if ( vguiMatSystemTopPanel == vguiPanel ) //If we're on MatSystemTopPanel, call our drawing code.
		{
			if( gInts.Engine->IsDrawingLoadingImage() || !gInts.Engine->IsInGame( ) || !gInts.Engine->IsConnected() || gInts.Engine->Con_IsVisible( ) || gInts.Engine->IsTakingScreenshot( ) )
			{
				return; //We don't want to draw at the menu.
			}

			//This section will be called when the player is not at the menu game and can see the screen or not taking a screenshot.
			gDrawManager.DrawString( 200, 200, gDrawManager.dwGetTeamColor(3), "Welcome to Darkstorm"); //Remove this if you want.

			/*
			//Test ESP code.

			CBaseEntity* pBaseLocalEnt = gInts.EntList->GetClientEntity(me);  //Grab the local player's entity.

			if (pBaseLocalEnt == NULL) //Always check for null pointers.
				return;

			Vector vecWorld, vecScreen; //Setup the Vectors.

			pBaseLocalEnt->GetWorldSpaceCenter(vecWorld); //Get the center of the player.

			if ( gDrawManager.WorldToScreen(vecWorld, vecScreen) ) //If the player is visble.
			{
				gDrawManager.DrawString( vecScreen.x, vecScreen.y, 0xFFFFFFFF, "You" ); //Draw on the player.
			}
			*/
		}
	}
	catch(...)
	{
		//Log::Fatal("Failed PaintTraverse");
	}
}
//===================================================================================
void Intro( void )
{
	try
	{
		gDrawManager.Initialize(); //Initalize the drawing class.

		Log::Msg("Injection Successful"); //If the module got here without crashing, it is good day.

		Log::Debug("Height=%d, Width=%d",gScreenSize.iScreenHeight, gScreenSize.iScreenWidth);
	}
	catch(...)
	{
		//Log::Fatal("Failed Intro");
	}
}
