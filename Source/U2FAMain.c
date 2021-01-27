//
// File : U2FAMain.c
//

#include "U2FAMain.h"



UINTN
U2FASetCurDir(
	IN EFI_SHELL_PROTOCOL *EfiShellProtocol
)
{
	//
	// Check Usb key in the perticular File System
	//
	UsbFSCheck(EfiShellProtocol);

	return EFI_SUCCESS;
}

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
	EFI_SHELL_PROTOCOL		*EfiShellProtocol;
	EFI_STATUS				Status;

	Status = gBS->LocateProtocol(
					&gEfiShellProtocolGuid,
					NULL,
					(VOID**)&EfiShellProtocol
					);
	if (EFI_ERROR(Status)) {
		return Status;
	}

	//
	// Setting the current directory for USB key
	//
	U2FASetCurDir(EfiShellProtocol);

	return EFI_SUCCESS;
}