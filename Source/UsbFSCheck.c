//
// File : UsbFSCheck.c
//

#include "UsbFSCheck.h"

//
// Check Usb key in the perticular File System
//
UINTN
UsbFSCheck(
	IN EFI_SHELL_PROTOCOL *EfiShellProtocol
)
{
	EFI_STATUS			Status;
	UINT8				UsbNumber;
	CHAR16				FSStr[] = L"FS1:";
	UINT16				FSnum = 0x0030;
	SHELL_FILE_HANDLE	Handle;

	for (UsbNumber = 1; UsbNumber < 7; UsbNumber++) {
		Status = EfiShellProtocol->SetCurDir(
										NULL,
										(L"%s", FSStr)
										);
		if (EFI_ERROR(Status)) {
			return Status;
		}

		Status = EfiShellProtocol->OpenFileByName(
										L"u2fa.key",
										&Handle,
										EFI_FILE_MODE_READ
										);
		if (EFI_ERROR(Status)) {
			Print(L"%s - File not found u2fa.key\n", FSStr);
			FSStr[2] = ++FSnum;
		}
		else {
			Print(L"File found u2fa.key\n");
			EfiShellProtocol->CloseFile(Handle);
			break;
		}
	}
	return EFI_SUCCESS;
}