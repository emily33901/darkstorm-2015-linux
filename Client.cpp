#include "SDK.h"
#include "Client.h"

//============================================================================================
bool __fastcall Hooked_CreateMove(PVOID ClientMode, float input_sample_frametime, CUserCmd *pCommand)
{
	
	//If you want taunt slide, you will need to hook CHLClient::CreateMove and do it there. If you do it here, you'll just shimmy forward.
	VMTManager& hook = VMTManager::GetHook(ClientMode); //Get a pointer to the instance of your VMTManager with the function GetHook.
	bool bReturn = hook.GetMethod<bool(__thiscall*)(PVOID, float, CUserCmd*)>(gOffsets.iCreateMoveOffset)(ClientMode, input_sample_frametime, pCommand); //Call the original.
	try
	{
		CBaseEntity* pBaseEntity = GetBaseEntity(me); //Grab the local player's entity pointer.

		if (pBaseEntity == NULL) //This should never happen, but never say never. 0xC0000005 is no laughing matter.
			return bReturn;

		//Do your client hook stuff here. This function is called once per tick. For time-critical functions, run your code in PaintTraverse.
		// For move specific functions, run them here.
		if (pCommand->buttons & IN_JUMP) //To prove we have control over the CUserCmd, write the IN_ATTACK bit every time we jump.
		{
			pCommand->buttons |= IN_ATTACK; //Set the IN_ATTACK flag.
		}
	}
	catch(...)
	{
		// Log::Fatal("Failed Hooked_CreateMove");
	}
	return bReturn; //If bReturn is true, CInput::CreateMove will call CEngine::SetViewAngles(pCommand->viewangles). If you want silent aim, return false, but make sure to call SetViewAngles manually.
	
	return true;
}
//============================================================================================
