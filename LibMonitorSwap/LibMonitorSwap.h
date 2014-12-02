extern "C"
{

	__declspec(dllexport) BOOL WINAPI GetDisplayCount(UINT* MonitorCount);

	__declspec(dllexport) BOOL WINAPI GetPrimaryDisplayName(TCHAR AdapterName[32]);

	__declspec(dllexport) BOOL WINAPI GetDisplayNameArray(LPCTSTR** ArrayOfDisplayNames, UINT* DisplaysInArray);

	__declspec(dllexport) BOOL WINAPI ReleaseDisplayNameArray(LPCTSTR* ArrayOfDisplayNames, UINT DisplaysInArray);

	__declspec(dllexport) BOOL WINAPI GetDisplayPosition(LPCTSTR DisplayName, LONG* xPosition, LONG* yPosition);

	__declspec(dllexport) BOOL WINAPI SetPrimaryDisplay(LPCTSTR PrimaryDisplayName);

	__declspec(dllexport) BOOL WINAPI GetMonitorDeviceID(LPCTSTR DisplayName, TCHAR DeviceID[128]);

	__declspec(dllexport) BOOL WINAPI GetMonitorNameFromDisplayName(LPCTSTR DisplayName, TCHAR MonitorName[128]);

	__declspec(dllexport) BOOL WINAPI GetLastLibraryErrorMsg(LPCTSTR* lastErrorMsg);

}