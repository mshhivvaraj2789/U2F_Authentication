//
// File : U2FAMain.c
//

#include "U2FAMain.h"

EFI_GUID gEfiSimpleFileSystemProtocolGuid
				= EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

EFI_STATUS
EFIAPI PerFileFunc(
	IN EFI_FILE_HANDLE Dir,
	IN EFI_DEVICE_PATH *DirDp,
	IN EFI_FILE_INFO *FileInfo,
	IN EFI_DEVICE_PATH *Dp
)
{
	EFI_STATUS Status;
	EFI_FILE_HANDLE File;

	Print(L"Path = %s FileName = %s\n", ConvertDevicePathToText(DirDp, TRUE,
		TRUE), FileInfo->FileName);

	// read the file into a buffer
	Status = Dir->Open(Dir, &File, FileInfo->FileName, EFI_FILE_MODE_READ, 0);
	if (EFI_ERROR(Status)) {
		return Status;
	}

	// reset position just in case
	File->SetPosition(File, 0);

	// ****Do stuff on the file here****

	Dir->Close(File);

	return EFI_SUCCESS;
}

//
// Recurse through directory, calling a user defined function for each file.
//
EFI_STATUS
EFIAPI
ProcessFilesInDir(
	IN EFI_FILE_HANDLE Dir,
	IN EFI_DEVICE_PATH *DirDp
)
{
	EFI_STATUS				Status;
	EFI_FILE_INFO			*FileInfo;
	UINTN					FileInfoSize;
	CHAR16					*FileName;
	EFI_DEVICE_PATH			*Dp;

	//
	// Big enough to hold EFI_FILE_INFO struct and the whole file path.
	//
	FileInfo = AllocatePool(MAX_FILE_INFO_SIZE);
	if (FileInfo == NULL) {
		return EFI_OUT_OF_RESOURCES;
	}

	for (;;) {
		//
		// Get the next file's info. There's an internal position that gets incremented when
		// you read from a directory so that subsequent reads gets the next fils's info.
		//
		FileInfoSize = MAX_FILE_INFO_SIZE;
		Status = Dir->Read(Dir, &FileInfoSize, (VOID*)FileInfo);
		//
		// This is how we eventually exit this function when we run out of files.
		//
		if (EFI_ERROR(Status) || FileInfoSize == 0) {
			if (Status == EFI_BUFFER_TOO_SMALL) {
				Print(L"EFI_FILE_INFO > MAX_FILE_INFO_SIZE. Increase the size\n");
			}
			FreePool(FileInfo);
			return Status;
		}

		FileName = FileInfo->FileName;

		//
		// skip files named . or ..
		//
		if (StrCmp(FileName, L".") == 0 || StrCmp(FileName, L"..") == 0) {
			continue;
		}
		//
		// Searching for u2fa.key file in USB Mass Storage
		//
		else if (StrCmp(FileName, L"u2fa.key") == 0) {
			Print(L"Detected file u2fa.key\n");
		}

		//
		// So we have absolute device path to child file/dir.
		//
		Dp = FileDevicePath(DirDp, FileName);
		if (Dp == NULL) {
			FreePool(FileInfo);
			return EFI_OUT_OF_RESOURCES;
		}

		//
		// Do whatever processing on the file.
		//
		PerFileFunc(Dir, DirDp, FileInfo, Dp);

		if (FileInfo->Attribute & EFI_FILE_DIRECTORY) {
			//
			// recurse
			//

			EFI_FILE_HANDLE NewDir;

			Status = Dir->Open(Dir, &NewDir, FileName, EFI_FILE_MODE_READ, 0);
			if (Status != EFI_SUCCESS) {
				FreePool(FileInfo);
				FreePool(Dp);
				return Status;
			}
			NewDir->SetPosition(NewDir, 0);

			Status = ProcessFilesInDir(NewDir, Dp);
			Dir->Close(NewDir);
			if (Status != EFI_SUCCESS) {
				FreePool(FileInfo);
				FreePool(Dp);
				return Status;
			}
		}
		FreePool(Dp);
	}
}

//
// U2F Authentication main entry point.
//
EFI_STATUS
EFIAPI
U2FAMain(
	IN EFI_HANDLE			ImageHandle,
	IN EFI_SYSTEM_TABLE		*SystemTable
)
{
	EFI_STATUS			Status;
	UINTN				NumHandles;
	EFI_HANDLE			*Handles;
	UINTN				Index;
	EFI_HANDLE			AgentHandle;
	VOID				*Context;

	AgentHandle = ImageHandle;


	//
	// Get all handles with file system installed.
	//
	Status = gBS->LocateHandleBuffer(
					ByProtocol,
					&gEfiSimpleFileSystemProtocolGuid,
					NULL,
					&NumHandles,
					&Handles
					);
	if (EFI_ERROR(Status)) {
		return Status;
	}

	//
	// Loop through all handles we just got.
	//
	for (Index = 0; Index < NumHandles; Index++) {
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs;
		EFI_DEVICE_PATH *Dp;
		EFI_FILE_HANDLE Root;
		//
		// Get file system protocol instance from current handle.
		//
		Status = gBS->OpenProtocol(
						Handles[Index],
						&gEfiSimpleFileSystemProtocolGuid,
						&Fs,
						NULL,
						AgentHandle,
						EFI_OPEN_PROTOCOL_GET_PROTOCOL
						);
		if (EFI_ERROR(Status)) {
			DEBUG((EFI_D_ERROR, "pError: Missing EFI_SIMPLE_FILE_SYSTEM_PROTOCOL on handle.\n"));
			continue;
		}

		//
		// Get device path instance from current handle.
		//
		Status = gBS->OpenProtocol(
						Handles[Index],
						&gEfiDevicePathProtocolGuid,
						&Dp,
						NULL,
						AgentHandle,
						EFI_OPEN_PROTOCOL_GET_PROTOCOL
						);
		if (EFI_ERROR(Status)) {
			DEBUG((EFI_D_ERROR, "pError: Missing EFI_DEVICE_PATH_PROTOCOL on handle.\n"));
			continue;
		}

		//
		// Open root dir from current file system.
		//
		Status = Fs->OpenVolume(Fs, &Root);
		if (EFI_ERROR(Status)) {
			DEBUG((EFI_D_ERROR, "pError: Unable to open volume.\n"));
			continue;
		}

		//
		// Recursively process files in root dir.
		//
		Context = NULL;
		Status = ProcessFilesInDir(Root, Dp);
		Root->Close(Root);
		if (EFI_ERROR(Status)) {
			DEBUG((EFI_D_ERROR, "pError: ProcessFilesInDir. Continuing with next volume...\n"));
			continue;
		}
	}

	return EFI_SUCCESS;
}