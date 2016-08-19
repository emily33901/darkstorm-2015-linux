#include "SDK.h"

class CPlayerVariables
{
public:
	int iWeaponID;
	bool bHasMeleeWeapon;
	bool bHasFlameThrower;
	int iClass;
	int iWeaponSlot;
	int iPlayerCond;
	int iPlayerCondEx;
	int iFlags;
	char chName[34];
};

class COffsets
{
public:
	int iCreateMoveOffset = 22, iPaintTraverseOffset = 42;
};
