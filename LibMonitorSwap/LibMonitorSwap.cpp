// LibMonitorSwap.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "LibMonitorSwap.h"
#include <stdlib.h>
#include <string.h>

// Library storage for error messages
// Callers can retrieve through GetLastLibraryErrorMessage
static TCHAR last_error[100];

//Non-Exported function declarations and definitions
BOOL WINAPI UpdateNonPrimaryDisplayCoordinates(LPCTSTR PrimaryDisplayName, LONG xOffset, LONG yOffset);

BOOL WINAPI SetLastLibraryErrorMsg(LPCTSTR msg)
{
	_tcscpy_s(last_error, 100, msg);
	return TRUE;
}

///// Exported Function Definitions /////

BOOL WINAPI GetLastLibraryErrorMsg(LPCTSTR* lastErrorMsg)
{
	*lastErrorMsg = last_error;
	return TRUE;
}

BOOL WINAPI GetDisplayCount(UINT* MonitorCount)
{
	DISPLAY_DEVICE DispDev;
	DispDev.cb = sizeof(DispDev);

	UINT count = 0;

	for (UINT id = 0; EnumDisplayDevices(NULL, id, &DispDev, NULL); id++)
	{
		if (DispDev.StateFlags & DISPLAY_DEVICE_ACTIVE)
			count++;
	}

	*MonitorCount = count;

	return TRUE;
}

BOOL WINAPI GetPrimaryDisplayName(TCHAR AdapterName[32])
{
	// AdapterName should be a TCHAR[32]
	DISPLAY_DEVICE DispDev;
	DispDev.cb = sizeof(DispDev);

	for (UINT id = 0; EnumDisplayDevices(NULL, id, &DispDev, NULL); id++)
	{
		if (DispDev.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		{
			_tcscpy_s(AdapterName, 32, DispDev.DeviceName);
			return TRUE;
		}
	}

	SetLastLibraryErrorMsg(_T("Could not find primary adapter"));
	return FALSE;
}

BOOL WINAPI GetDisplayPosition(LPCTSTR DisplayName, LONG* xPosition, LONG* yPosition)
{
	BOOL status = FALSE;
	DEVMODE	DevMode;
	DevMode.dmSize = sizeof(DevMode);

	status = EnumDisplaySettings(DisplayName, ENUM_CURRENT_SETTINGS, &DevMode);
	if (!status)
	{
		SetLastLibraryErrorMsg(_T("Failed to enumerate display settings"));
		return FALSE;
	}

	*xPosition = DevMode.dmPosition.x;
	*yPosition = DevMode.dmPosition.y;

	return TRUE;
}

BOOL WINAPI SetPrimaryDisplay(LPCTSTR PrimaryDisplayName)
{
	BOOL status = FALSE;
	LONG DisplaySettingResult = DISP_CHANGE_FAILED;

	// The primary display is always the one at 0,0. To change the primary,
	// we need to adjust the coordinates of all displays to reflect this.
	LONG x_offset = 0; 
	LONG y_offset = 0;

	DISPLAY_DEVICE DispDev;
	DispDev.cb = sizeof(DispDev);

	DEVMODE	DevMode;
	DevMode.dmSize = sizeof(DevMode);

	// Set the new monitor to be the primary
	status = EnumDisplaySettings(PrimaryDisplayName, ENUM_CURRENT_SETTINGS, &DevMode);
	if (!status) {
		SetLastLibraryErrorMsg(_T("Failed to enumerate display settings"));
		return FALSE;
	}

	x_offset = DevMode.dmPosition.x;
	y_offset = DevMode.dmPosition.y;

	DevMode.dmPosition.x = 0;
	DevMode.dmPosition.y = 0;

	// Change registry, but don't update
	DisplaySettingResult = ChangeDisplaySettingsEx(PrimaryDisplayName, &DevMode, NULL, CDS_SET_PRIMARY | CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
	if (DisplaySettingResult != DISP_CHANGE_SUCCESSFUL)
	{
		SetLastLibraryErrorMsg(_T("Failed to update registry with new monitor positions"));
		return FALSE;
	}

	status = UpdateNonPrimaryDisplayCoordinates(PrimaryDisplayName, x_offset, y_offset);
	if (!status)
	{
		// don't set last error here - the callee will have set it
		return FALSE;
	}

	DisplaySettingResult = ChangeDisplaySettingsEx(NULL, NULL, NULL, NULL, NULL);
	if (DisplaySettingResult != DISP_CHANGE_SUCCESSFUL)
	{
		SetLastLibraryErrorMsg(_T("Failed to apply in-registry monitor changes"));
		return FALSE;
	}

	return TRUE;
}

BOOL WINAPI GetDisplayNameArray(LPCTSTR** ArrayOfDisplayNames, UINT* DisplaysInArray)
{
	BOOL status = FALSE;
	DISPLAY_DEVICE DispDev;
	DispDev.cb = sizeof(DispDev);

	status = GetDisplayCount(DisplaysInArray);
	if (!status)
	{
		SetLastLibraryErrorMsg(_T("Failed to get display count"));
		return FALSE;
	}

	// allocate the array itself
	*ArrayOfDisplayNames = (LPCTSTR*)malloc(sizeof(TCHAR*) * (*DisplaysInArray));
	if ((*ArrayOfDisplayNames) == NULL)
	{
		SetLastLibraryErrorMsg(_T("Failed to allocate memory to hold display names array"));
		return FALSE;
	}

	LPCTSTR *currentName = *ArrayOfDisplayNames;
	LPTSTR tmpBuffer = NULL;

	for (UINT id = 0; EnumDisplayDevices(NULL, id, &DispDev, NULL); id++)
	{
		if (DispDev.StateFlags & DISPLAY_DEVICE_ACTIVE)
		{
			UINT DeviceNameLength = _tcslen(DispDev.DeviceName) + 1;

			tmpBuffer = (LPTSTR)malloc(sizeof(TCHAR) * DeviceNameLength);
			if ((tmpBuffer) == NULL)
			{
				SetLastLibraryErrorMsg(_T("Failed to allocate memory for monitor name string"));
				return FALSE;
			}

			_tcscpy_s(tmpBuffer, DeviceNameLength, DispDev.DeviceName);
			*currentName = tmpBuffer;
			currentName++;
		}
	}

	return TRUE;
}

BOOL WINAPI ReleaseDisplayNameArray(LPCTSTR* ArrayOfDisplayNames, UINT DisplaysInArray)
{
	for (UINT index = 0; index < DisplaysInArray; index++)
	{
		// Free the string
		free((void*) (ArrayOfDisplayNames[index]));
	}

	// Free the array itself
	free(ArrayOfDisplayNames);

	return TRUE;
}

BOOL WINAPI GetMonitorDeviceID(LPCTSTR DisplayName, TCHAR DeviceID[128])
{
	// DeviceID should be a TCHAR[128]
	BOOL status = FALSE;
	DISPLAY_DEVICE DispDev;
	DispDev.cb = sizeof(DispDev);

	status = EnumDisplayDevices(DisplayName, 0, &DispDev, EDD_GET_DEVICE_INTERFACE_NAME);
	if (!status)
	{
		SetLastLibraryErrorMsg(_T("Failed to enumerate monitors"));
		return FALSE;
	}

	// Get the deviceID - this will not be in standard DeviceID form - we need to clean it up
	// Move to temp buffer so we can make changes
	TCHAR tmp_buffer[128];
	// copy only from the 5th character onwards (remove leading \\?\)
	_tcscpy_s(tmp_buffer, 128, DispDev.DeviceID + 4);

	// split by '#' signs in returned string
	TCHAR* token = NULL;
	TCHAR* context = NULL;
	UINT position_in_DeviceID = 0;
	UINT cnt = 0; // track how many strings we've moved
	token = _tcstok_s(tmp_buffer, L"#", &context);

	while ((token != NULL) && (cnt < 3))
	{
		// write [split string result] + '\' to the string (offset by how much we've already wrote)
		_stprintf_s(DeviceID + position_in_DeviceID, 128 - position_in_DeviceID, _T("%s\\"), token);
		position_in_DeviceID = _tcslen(DeviceID);

		// Get next token
		token = _tcstok_s(NULL, L"#", &context);
		cnt++;
	}

	// remove the final slash (write a null terminator there instead)
	DeviceID[position_in_DeviceID - 1] = _T('\0');


	return TRUE;
}

BOOL WINAPI GetMonitorNameFromDisplayName(LPCTSTR DisplayName, TCHAR MonitorName[128])
{
	// MonitorName should be a TCHAR[128]
	BOOL status = FALSE;
	DISPLAY_DEVICE DispDev;
	DispDev.cb = sizeof(DispDev);

	status = EnumDisplayDevices(DisplayName, 0, &DispDev, EDD_GET_DEVICE_INTERFACE_NAME);
	if (!status)
	{
		SetLastLibraryErrorMsg(_T("Failed to enumerate monitors"));
		return FALSE;
	}

	_tcscpy_s(MonitorName, 128, DispDev.DeviceString);

	return TRUE;
}

/////// Non-Exported functions below this line //////

BOOL WINAPI UpdateNonPrimaryDisplayCoordinates(LPCTSTR PrimaryDisplayName, LONG xOffset, LONG yOffset)
{
	BOOL status = FALSE;
	LONG DisplaySettingResult = DISP_CHANGE_FAILED;
	DISPLAY_DEVICE DispDev;
	DispDev.cb = sizeof(DispDev);
	DEVMODE	DevMode;
	DevMode.dmSize = sizeof(DevMode);

	for (UINT id = 0; EnumDisplayDevices(NULL, id, &DispDev, NULL); id++)
	{
		// only update Active, Non-Primary Monitors
		if ((DispDev.StateFlags & DISPLAY_DEVICE_ACTIVE)
			&& (_tcscmp(PrimaryDisplayName, DispDev.DeviceName) != 0))
		{
			// Get current dev mode structure
			status = EnumDisplaySettings(DispDev.DeviceName, ENUM_CURRENT_SETTINGS, &DevMode);
			if (!status)
			{
				SetLastLibraryErrorMsg(_T("Failed to enumerate display settings"));
				return FALSE;
			}

			// Adjust position
			DevMode.dmPosition.x -= xOffset;
			DevMode.dmPosition.y -= yOffset;

			// Update registry
			DisplaySettingResult = ChangeDisplaySettingsEx(DispDev.DeviceName, &DevMode, NULL, CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
			if (DisplaySettingResult != DISP_CHANGE_SUCCESSFUL)
			{
				SetLastLibraryErrorMsg(_T("Failed to update registry with new monitor positions"));
				return FALSE;
			}
		}
	}

	return TRUE;
}