//
// File : U2FAMain.h
//

#ifndef __U2F_A_MAIN_H__
#define __U2F_A_MAIN_H__

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Shell.h>

#include "UsbFSCheck.h"

UINTN
U2FASetCurDir(
	IN EFI_SHELL_PROTOCOL *EfiShellProtocol
);

#endif