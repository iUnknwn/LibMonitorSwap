extern "C"
{

	__declspec(dllexport) BOOL WINAPI GetDisplayCount(UINT* MonitorCount);

	__declspec(dllexport) BOOL WINAPI GetPrimaryDisplayName(LPTSTR PrimaryMonitorName);

	__declspec(dllexport) BOOL WINAPI GetDisplayNameArray(LPTSTR** ArrayOfDisplayNames, UINT* DisplaysInArray);

	__declspec(dllexport) BOOL WINAPI ReleaseDisplayNameArray(LPTSTR* ArrayOfDisplayNames, UINT DisplaysInArray);

	__declspec(dllexport) BOOL WINAPI GetDisplayPosition(LPCTSTR DisplayName, LONG* xPosition, LONG* yPosition);

	__declspec(dllexport) BOOL WINAPI SetPrimaryDisplay(LPCTSTR PrimaryDisplayName);

	__declspec(dllexport) BOOL WINAPI GetMonitorDeviceID(LPCTSTR DisplayName, LPTSTR DeviceID);

	__declspec(dllexport) BOOL WINAPI GetMonitorNameFromDisplayName(LPCTSTR DisplayName, LPTSTR MonitorName);

	__declspec(dllexport) BOOL WINAPI GetLastLibraryErrorMsg(LPCTSTR* lastErrorMsg);

}