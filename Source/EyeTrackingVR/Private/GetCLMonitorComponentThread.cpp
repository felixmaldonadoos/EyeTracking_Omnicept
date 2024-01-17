#pragma once 
#include "GetCLMonitorComponentThread.h" 
#include "GameStateMain.h"
#include "GameModeMain.h"
#include "MouseKeyboardPlayerController.h"
#include "GetCLMonitorComponentActor.h"
#include "EyeTrackerFunctionLibrary.h"
#include "ToolDataVisualizer.h"
#include "TextFileManager.h"
#include "Misc/DateTime.h" 
#include "Keys.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h" 
#include "Kismet/KismetStringLibrary.h" 

/* changes state of bIsConnected*/
bool FGetCLMonitorComponentThread::ConnectToDevice() {
	const FString access_key = "F8OK38DWnRgqJgr5aaUhgcfBPHoEe5toBiDGGREkR2DWeZxgTKFpCF5YvAdnHd-S";
	const FString client_id = "25b17c6b-3386-45f8-9e1e-88d76259b5bf";
	UHPGliaClient::ConnectToGliaAsync(client_id, access_key, ELicensingModel::CORE);

	bIsConnected = UHPGliaClient::IsConnected();

	if (bIsConnected) { UE_LOG(LogTemp, Error, TEXT("[FGetCLMonitorComponentThread::ConnectToDevice()] HPGlia Client connection: FAILED.")); }
	else { UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::ConnectToDevice()] HPGlia Client connection: SUCCESS.")); }

	//UHPGliaClient::ConnectToGliaAsync(HPGliaKeys::CLIENT_ID, HPGliaKeys::ACCESS_KEY, ELicensingModel::REV_SHARE);
	return bIsConnected;
}

bool FGetCLMonitorComponentThread::GetEyeTracking(FEyeTracking& OutEyeTracking) {
	return UHPGliaClient::GetEyeTracking(EyeData);
}

FGetCLMonitorComponentThread::FGetCLMonitorComponentThread(AGetCLMonitorComponentActor* funActor, FString SaveDataDirectory)
{
	CurrentThreadActor = funActor; 
	/* init eye-tracker */

}

FGetCLMonitorComponentThread::~FGetCLMonitorComponentThread()
{
	// Destructor
	if (Thread != nullptr)
	{
		Thread->Kill(true);
		delete Thread;
	}
}

uint32 FGetCLMonitorComponentThread::Run()
{

	FGetCLMonitorComponentThread::ConnectToDevice();

	/* check HMD */
	//IStereoRendering RenderingPtr = nullptr; 
	if (!GEngine->XRSystem->GetHMDDevice()->IsHMDConnected()) {
		UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::Run()] HMD NOT connected."));
	}

	/* to do: change bStopThread to GS */
	int i = 0; 
	while (!bStopThread) {

		/* print eye data */
		
		FEyeTracking HPEye;
		if (!UHPGliaClient::GetEyeTracking(HPEye)) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Eye data: Not valid"));
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Eye data: Valid"));
		}

		const FString ProjectDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

		FString save_line = FString::SanitizeFloat(HPEye.LeftGaze.X) + "," + FString::SanitizeFloat(HPEye.LeftGaze.Y) + "," + FString::SanitizeFloat(HPEye.LeftGaze.Z) + "\n";
		UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::Run()] HPGlia::GetEyeTracking() saving line -> %s"), *save_line);
		FString filename = ProjectDirectory + "Data/data.txt";
		IFileManager *FileManager = nullptr;

		//if (!FFileHelper::SaveStringToFile(*save_line, *filename, FFileHelper::EEncodingOptions::AutoDetect, FileManager, EFileWrite::FILEWRITE_Append)) {
		//	UE_DEBUG_BREAK();
		//}

		if (!UTextFileManager::SaveTxt(*save_line, *filename)) {
			UE_DEBUG_BREAK();
		}

		i += 1; 
		FPlatformProcess::Sleep(0.1f); // sleep 
	}

	UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::Run()] Ending thread."));
	return 0;
}


void FGetCLMonitorComponentThread::Stop()
{
	bStopThread = true;
}

void FGetCLMonitorComponentThread::Exit()
{
}
