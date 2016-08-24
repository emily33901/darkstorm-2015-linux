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
			}
		}
		
		// This sig here wont work on linux (a replacement one is further down)
		//DWORD dwClientModeAddress = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 02 D9 05");
		//XASSERT(dwClientModeAddress);
		//gInts.ClientMode = **(ClientModeShared***)(dwClientModeAddress + 2);
		//LOGDEBUG("g_pClientModeShared_ptr client.dll+0x%X", (DWORD)gInts.ClientMode - dwClientBase);

		// because of how LD_PRELOAD works and the fact that g_pClientmodeshared is NULL roughly until the menu appears,
		// we need to wait for the pointer to not be NULL before we try to hook this function.
		// since this mainThread() function is not called from a thread, we need to make another thread that we can block until
		// clientmodeshared is no longer NULL
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
			uintptr_t dwHudProcessInput = (uintptr_t)((*(void ***)gInts.Client)[10]);
			XASSERT(dwHudProcessInput);
		
			/*
			// check we get the right function
			PBYTE pbHudFunc = (PBYTE)dwHudProcessInput;

			for(int i = 0; pbHudFunc[i-2] != 0xFF && pbHudFunc[i-1] != 0xE0; i++)
			{
			Log::Msg("%X",pbHudFunc[i]);
			}
			*/
		
			uintptr_t dwClientModeAddress = dwHudProcessInput + 1;
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

			// if you really want to use a sig, here is one.
			// its far to long (the whole function, but go ahead if you really feel like it.
			// it was mainly to test that my signature scanning was working.
			//uintptr_t dwAddr = gSignatures.GetClientSignature("A1 ? ? ? ? 55 89 E5 0F B6 4D 0C 8B 10 89 45 08 89 4D 0C 5D 8B 42 2C FF E0");		
		};

		std::thread findCM(findCreateMove);
		findCM.detach();
		
	}
	return; //The thread has been completed, and we do not need to call anything once we're done. The call to Hooked_PaintTraverse is now our main
}

void __attribute__((constructor)) base_main()
{
   	// create the threads
  	//std::thread init(mainThread);
	//init.detach();

	mainThread();
	
	return;
}

void __attribute__((destructor))
endthisshit()
{
	// do unload stuff here
}
