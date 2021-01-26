//
// File : U2FAMain.c
//

#include "U2FAMain.h"

//
// U2F Authentication main entry point
//
EFI_STATUS
EFIAPI
U2FAMain(
	IN EFI_HANDLE			ImageHandle,
	IN EFI_SYSTEM_TABLE		*SystemTable
)
{
	Print(L"Hello, Shhivva Raj !\n");

	return EFI_SUCCESS;
}