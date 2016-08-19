#include "SDK.h"
#include "Client.h"
#include "Panels.h"

#include "Log.h"

#include <unistd.h>
#include <pthread.h>
#include <thread>

COffsets gOffsets;
CPlayerVariables gPlayerVars;
CInterfaces gInts;

CreateInterface_t EngineFactory = NULL;
CreateInterface_t ClientFactory = NULL;
CreateInterface_t VGUIFactory = NULL;
CreateInterface_t VGUI2Factory = NULL;

void mainThread()
{	
	if (gInts.Client == NULL) //Prevent repeat callings.
	{
		//Gottammove those factorys up.
		//Grab the factorys from their resptive module's EAT.

		//system("ls -l . > /home/josh/log.txt");
		
		ClientFactory = ( CreateInterfaceFn ) GetProcAddress( gSignatures.GetModuleHandleSafe( "./tf/bin/client.so" ), "CreateInterface" );
		gInts.Client = ( CHLClient* )ClientFactory( "VClient017", NULL);
		XASSERT(gInts.Client);
		gInts.EntList = ( CEntList* ) ClientFactory( "VClientEntityList003", NULL );
		XASSERT(gInts.EntList);
		EngineFactory = ( CreateInterfaceFn ) GetProcAddress( gSignatures.GetModuleHandleSafe( "./bin/engine.so" ), "CreateInterface" );
		gInts.Engine = ( EngineClient* ) EngineFactory( "VEngineClient013", NULL );
		XASSERT(gInts.Engine);
		VGUIFactory = ( CreateInterfaceFn ) GetProcAddress( gSignatures.GetModuleHandleSafe( "./bin/vguimatsurface.so" ), "CreateInterface" );
		gInts.Surface = ( ISurface* ) VGUIFactory( "VGUI_Surface030", NULL );
		XASSERT(gInts.Surface);

		//Setup the Panel hook so we can draw.
		if( !gInts.Panels )
		{
			VGUI2Factory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("./bin/vgui2.so"), "CreateInterface");
			gInts.Panels = ( IPanel* ) VGUI2Factory( "VGUI_Panel009", NULL );
			XASSERT( gInts.Panels );

			if( gInts.Panels )
			{
				VMTBaseManager* panelHook = new VMTBaseManager(); //Setup our VMTBaseManager for Panels.
				
				panelHook->Init(gInts.Panels);
				panelHook->HookMethod(&Hooked_PaintTraverse, gOffsets.iPaintTraverseOffset);
				panelHook->Rehook();

				//Log::Msg("Hooked_PaintTraverse=0x%X | Panels::PaintTraverse=0x%X",(PVOID)&Hooked_PaintTraverse, panelHook->GetMethod<PVOID>(gOffsets.iPaintTraverseOffset));
			}
		}

		
		//DWORD dwClientModeAddress = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 02 D9 05");
		//XASSERT(dwClientModeAddress);
		//gInts.ClientMode = **(ClientModeShared***)(dwClientModeAddress + 2);
		//LOGDEBUG("g_pClientModeShared_ptr client.dll+0x%X", (DWORD)gInts.ClientMode - dwClientBase);

		// That sig there wont work on linux.

		// because of how LD_PRELOAD works and the fact that g_pClientmodeshared is NULL roughly until the menu appears,
		// we should get g_pClientmodeshared in Intro()

		auto findCreateMove = [&]()
		{
			
			// CHLClient::HudProcessinput [vfunc 10] has the g_pClientModeShared pointer in it
			/*
			  .text:00D80490                               CHLClient__HudProcessInput proc near    ; DATA XREF: .rodata:017525F0o
			  .text:00D80490
			  .text:00D80490                               arg_0           = dword ptr  8
			  .text:00D80490                               arg_4           = dword ptr  0Ch
			  .text:00D80490
			  .text:00D80490 A1 1C A7 E9 01                                mov     eax, ds:g_pClientModeShared
			  .text:00D80495 55                                            push    ebp
			  .text:00D80496 89 E5                                         mov     ebp, esp
			  .text:00D80498 0F B6 4D 0C                                   movzx   ecx, byte ptr [ebp+arg_4]
			  .text:00D8049C 8B 10                                         mov     edx, [eax]
			  .text:00D8049E 89 45 08                                      mov     [ebp+arg_0], eax
			  .text:00D804A1 89 4D 0C                                      mov     [ebp+arg_4], ecx
			  .text:00D804A4 5D                                            pop     ebp
			  .text:00D804A5 8B 42 2C                                      mov     eax, [edx+2Ch]
			  .text:00D804A8 FF E0                                         jmp     eax
			  .text:00D804A8                               CHLClient__HudProcessInput endp
			*/

			// get the 10th function in the vtable
			DWORD dwHudProcessInput = (DWORD)((*(void ***)gInts.Client)[10]);
			XASSERT(dwHudProcessInput);
		
			/*
			// check we get the right function
			PBYTE pbHudFunc = (PBYTE)dwHudProcessInput;

			for(int i = 0; pbHudFunc[i-2] != 0xFF && pbHudFunc[i-1] != 0xE0; i++)
			{
			Log::Msg("%X",pbHudFunc[i]);
			}
			*/
		
			DWORD dwClientModeAddress = dwHudProcessInput + 1;
			XASSERT(dwClientModeAddress);

			while((gInts.ClientMode = **(ClientModeShared ***)(dwClientModeAddress)) == NULL)
			{
				// wait until clientModeShared is not NULL
			}

			//Log::Msg("Hooking clientMode!");
		
			VMTBaseManager* clientModeHook = new VMTBaseManager(); //Setup our VMTBaseManager for ClientMode.
			clientModeHook->Init(gInts.ClientMode);
			clientModeHook->HookMethod(&Hooked_CreateMove, gOffsets.iCreateMoveOffset); //ClientMode create move is called inside of CHLClient::CreateMove, and thus no need for hooking WriteUserCmdDelta.
			clientModeHook->Rehook();
		
		};

		std::thread findCM(findCreateMove);
		findCM.detach();
		
	}
	return; //The thread has been completed, and we do not need to call anything once we're done. The call to Hooked_PaintTraverse is now our main
}

void __attribute__((constructor)) base_main()
{
	//If you manually map, make sure you setup this function properly.
   	Log::Init(NULL);
   	//CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)dwMainThread, NULL, 0, NULL ); //CreateThread > _BeginThread. (For what we're doing, of course.)

	Log::Msg("DLLMAIN CALLED!!!!");

   	// create the threads
  	//std::thread init(mainThread);
	//init.detach();

	// sleep this thread for 5 seconds to allow the engine to set single thread mode

	mainThread();
	
	return;
}

void __attribute__((destructor))
endthisshit()
{
	// do unload stuff here
}
