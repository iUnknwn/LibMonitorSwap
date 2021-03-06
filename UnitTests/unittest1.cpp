#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\LibMonitorSwap\LibMonitorSwap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMonitorCount)
		{
			// Only run when two monitors are present
			UINT MonitorCount = 0;
			BOOL status = FALSE;

			status = GetDisplayCount(&MonitorCount);

			Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

			Assert::AreEqual(MonitorCount, (UINT)2);
		}

		TEST_METHOD(TestGetPrimaryDisplayName)
		{
			BOOL status = FALSE;
			TCHAR PrimaryMonitorName[32];
			memset(PrimaryMonitorName, 0, 32 * sizeof(TCHAR));

			status = GetPrimaryDisplayName((LPTSTR)PrimaryMonitorName);
			Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

			TCHAR buffer[100];
			_stprintf_s(buffer, 100, _T("Primary Monitor is: %s\r\n"), PrimaryMonitorName);
			Logger::WriteMessage(buffer);

			Assert::IsTrue(status == TRUE, GetLastMsgWrapper());
		}

		TEST_METHOD(TestGetAndReleaseDisplayNameArray)
		{
			BOOL status = FALSE;
			LPCTSTR* DisplayNames;
			UINT DisplayCount;
			
			status = GetDisplayNameArray(&DisplayNames, &DisplayCount);
			Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

			TCHAR buffer[100];
			for (UINT index = 0; index < DisplayCount; index++)
			{
				_stprintf_s(buffer, 100, _T("Display %d is: %s\r\n"), index, DisplayNames[index]);
				Logger::WriteMessage(buffer);
			}

			status = ReleaseDisplayNameArray(DisplayNames, DisplayCount);
			Assert::IsTrue(status == TRUE, GetLastMsgWrapper());
		}

		TEST_METHOD(TestGetDisplayPosition_PrimaryMonitor)
		{
			// we know the primary monitor should always be 0,0
			// use that to test
			BOOL status = FALSE;
			TCHAR PrimaryMonitorName[32];
			memset(PrimaryMonitorName, 0, sizeof(TCHAR) * 32);

			status = GetPrimaryDisplayName(PrimaryMonitorName);
			Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

			LONG xPos = -1;
			LONG yPos = -1;

			status = GetDisplayPosition(PrimaryMonitorName, &xPos, &yPos);
			Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

			Assert::IsTrue(xPos == 0, _T("Primary Display X coordinate <> 0"));
			Assert::IsTrue(yPos == 0, _T("Primary Display Y coordinate <> 0"));

		}

		TEST_METHOD(TestGetDisplayPosition_AlternateMonitor)
		{
			BOOL status;
			UINT DisplayCount;

			status = GetDisplayCount(&DisplayCount);
			Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

			if (DisplayCount > 1)
			{
				TCHAR PrimaryDisplayName[32];
				status = GetPrimaryDisplayName(PrimaryDisplayName);
				Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

				// find the name of a display that is NOT the primary
				LPCTSTR* DisplayNameArray;
				status = GetDisplayNameArray(&DisplayNameArray, &DisplayCount);
				Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

				for (UINT index = 0; index < DisplayCount; index++)
				{
					LONG xPos;
					LONG yPos;
					
					if (_tcscmp(DisplayNameArray[index], PrimaryDisplayName) != 0)
					{
						status = GetDisplayPosition(DisplayNameArray[index], &xPos, &yPos);
						Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

						Assert::IsTrue((xPos != 0) || (yPos != 0), _T("Non-Primary Monitor cannot have 0,0 Coordinate"));
					}
				}

				status = ReleaseDisplayNameArray(DisplayNameArray, DisplayCount);
				Assert::IsTrue(status == TRUE, GetLastMsgWrapper());
			}
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(TestSetPrimaryDisplay)
			TEST_IGNORE()
			END_TEST_METHOD_ATTRIBUTE()
		TEST_METHOD(TestSetPrimaryDisplay)
		{
			BOOL status;
			UINT DisplayCount;

			status = GetDisplayCount(&DisplayCount);
			Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

			if (DisplayCount > 1)
			{
				TCHAR CurrentPrimaryDisplayName[32];
				TCHAR NewPrimaryDisplayName[32];
				status = GetPrimaryDisplayName(CurrentPrimaryDisplayName);
				Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

				// find the name of a display that is NOT the primary
				LPCTSTR* DisplayNameArray;
				status = GetDisplayNameArray(&DisplayNameArray, &DisplayCount);
				Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

				for (UINT index = 0; index < DisplayCount; index++)
				{

					if (_tcscmp(DisplayNameArray[index], CurrentPrimaryDisplayName) != 0)
					{
						// Swap the primary monitor
						status = SetPrimaryDisplay(DisplayNameArray[index]);
						Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

						// Copy the new primary name
						_tcscpy_s(NewPrimaryDisplayName, 32, DisplayNameArray[index]);
						break;
					}
				}

				// Monitor should now be switched
				memset(CurrentPrimaryDisplayName, 0, sizeof(TCHAR) * 32);
				status = GetPrimaryDisplayName(CurrentPrimaryDisplayName);
				Assert::IsTrue(status == TRUE, GetLastMsgWrapper());

				Assert::IsTrue(_tcscmp(CurrentPrimaryDisplayName, NewPrimaryDisplayName) == 0);

				status = ReleaseDisplayNameArray(DisplayNameArray, DisplayCount);
				Assert::IsTrue(status == TRUE, GetLastMsgWrapper());
			}
		}

		TEST_METHOD(TestGetDisplayDeviceID_Success)
		{
			TCHAR PrimaryDisplayName[32];
			TCHAR PrimaryMonitorID[128];
			BOOL status;

			status = GetPrimaryDisplayName(PrimaryDisplayName);
			Assert::IsTrue(status == TRUE);

			status = GetMonitorDeviceID(PrimaryDisplayName, PrimaryMonitorID);
			Assert::IsTrue(status == TRUE);

		}

		TEST_METHOD(TestGetDisplayDeviceID_FAIL)
		{
			TCHAR PrimaryMonitorID[128];
			BOOL status = GetMonitorDeviceID(_T("BAD NAME"), PrimaryMonitorID);

			Assert::IsFalse(status == TRUE, _T("Call to GetMonitorDeviceID succeeded - it should have failed."));

			if (!status)
			{
				Logger::WriteMessage(_T("Call to GetMonitorDeviceID failed (as expected) with error:"));
				Logger::WriteMessage(GetLastMsgWrapper());
				Logger::WriteMessage(_T("\r\n"));
			}
			

		}

		TEST_METHOD(TestGetMonitorNameFromDisplayName)
		{
			TCHAR PrimaryDisplayName[32];
			TCHAR PrimaryMonitorName[128];
			BOOL status;

			status = GetPrimaryDisplayName(PrimaryDisplayName);
			Assert::IsTrue(status == TRUE);

			status = GetMonitorNameFromDisplayName(PrimaryDisplayName, PrimaryMonitorName);
			Assert::IsTrue(status == TRUE);


		}

		// wraps the "GetLastLibraryErrorMsg to return an LPCTSTR instead of BOOL
		LPCTSTR GetLastMsgWrapper()
		{
			LPCTSTR msg = NULL;
			BOOL status = GetLastLibraryErrorMsg(&msg);
			if (!status)
			{
				Assert::Fail(_T("Getting Error Message Failed"));
				return NULL;
			}
			else
			{
				return msg;
			}
		}
	};
}