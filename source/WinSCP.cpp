//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

USEFORM("forms\NonVisual.cpp", NonVisualDataModule); /* TDataModule: File Type */
USEFORM("forms\CustomScpExplorer.cpp", CustomScpExplorerForm);
USEFORM("forms\ScpExplorer.cpp", ScpExplorerForm);
USEFORM("forms\ScpCommander.cpp", ScpCommanderForm);
//---------------------------------------------------------------------------
#include <CoreMain.h>
#include <WinInterface.h>
#include <ProgParams.h>
#include <VCLCommon.h>
#include <Setup.h>
#include <PuttyTools.h>
#include <GUITools.h>
//---------------------------------------------------------------------------
WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
  int Result = 0;
  try
  {
	TProgramParams * Params = TProgramParams::Instance();
    ApplicationLog = new TApplicationLog();
    UnicodeString AppLogPath;
    if (Params->FindSwitch(L"applog", AppLogPath))
    {
      ApplicationLog->Enable(AppLogPath);
    }
	AppLog(L"Starting...");

    AddStartupSequence(L"M");
    DllHijackingProtection();
    AddStartupSequence(L"T");

	WinInitialize();
    Application->Initialize();
    Application->MainFormOnTaskBar = true;
    Application->ModalPopupMode = pmAuto;
    SetEnvironmentVariable(L"WINSCP_PATH",
      ExcludeTrailingBackslash(ExtractFilePath(Application->ExeName)).c_str());
    CoreInitialize();
    ApplicationLog->AddStartupInfo(); // Needs Configuration
    InitializeWinHelp();
    InitializeSystemSettings();
    AddStartupSequence(L"S");

    try
    {
      try
      {
        ConfigureInterface();
        SetupInitialize();

        Application->Title = AppName;
        AppLog(L"Executing...");
        Result = Execute();
        AppLog(L"Execution done");
      }
      catch (Exception & E)
      {
        // Capture most errors before Usage class is released,
        // so that we can count them
		Configuration->Usage->Inc(L"GlobalFailures");
        // After we get WM_QUIT (posted by Application->Terminate()), i.e once Application->Run() exits,
        // the message just blinks
		ShowExtendedException(&E);
      }
    }
    __finally
    {
	  AppLog(L"Finalizing");
      GUIFinalize();
      FinalizeSystemSettings();
      FinalizeWinHelp();
      CoreFinalize();
      WinFinalize();
      AppLog(L"Finalizing done");
      SAFE_DESTROY_EX(TApplicationLog, ApplicationLog);
    }
  }
  catch (Exception &E)
  {
	ShowExtendedException(&E);
  }
  return Result;
}
//---------------------------------------------------------------------------
