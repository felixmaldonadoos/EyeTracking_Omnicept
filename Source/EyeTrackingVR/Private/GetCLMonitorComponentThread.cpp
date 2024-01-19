#pragma once 
#include "GetCLMonitorComponentThread.h" 
#include "GameStateMain.h"
#include "GameModeMain.h"
#include "MouseKeyboardPlayerController.h"
#include "GetCLMonitorComponentActor.h"
#include "EyeTrackerFunctionLibrary.h"
#include "GameInstanceMain.h"
#include <iostream>
#include <sstream>
#include "TextFileManager.h"
#include "Misc/DateTime.h" 
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h" 
#include "Kismet/KismetStringLibrary.h" 


bool FGetCLMonitorComponentThread::ConnectToDevice() {
	const FString access_key = "F8OK38DWnRgqJgr5aaUhgcfBPHoEe5toBiDGGREkR2DWeZxgTKFpCF5YvAdnHd-S";
	const FString client_id = "25b17c6b-3386-45f8-9e1e-88d76259b5bf";
	UHPGliaClient::ConnectToGliaAsync(client_id, access_key, ELicensingModel::CORE);

	bIsConnected = UHPGliaClient::IsConnected();

	/* to do: check connection, returns false but I get data that matches the demo...? confusing...*/
	if (!bIsConnected) { UE_LOG(LogTemp, Error, TEXT("[FGetCLMonitorComponentThread::ConnectToDevice()] HPGlia Client connection: FAILED.")); }
	else { UE_LOG(LogTemp, Log, TEXT("[FGetCLMonitorComponentThread::ConnectToDevice()] HPGlia Client connection: SUCCESS.")); }

	return bIsConnected;
}

///* changes state of bIsConnected*/
//bool FGetCLMonitorComponentThread::ConnectToDevice() {
//
//	World = CurrentThreadActor->GetWorldReferenceFromActor();
//	if (!World) { return false; }
//	
//	UGameInstanceMain* GI = Cast<UGameInstanceMain>(UGameplayStatics::GetGameInstance(World));
//	if (!GI) { return false; }
//
//	/*const FString access_key = "F8OK38DWnRgqJgr5aaUhgcfBPHoEe5toBiDGGREkR2DWeZxgTKFpCF5YvAdnHd-S";
//	const FString client_id = "25b17c6b-3386-45f8-9e1e-88d76259b5bf";*/
//	UHPGliaClient::ConnectToGliaAsync(GI->client_id, GI->access_key, ELicensingModel::CORE);
//
//	if (UHPGliaClient::IsConnected()) { UE_LOG(LogTemp, Error, TEXT("[FGetCLMonitorComponentThread::ConnectToDevice()] HPGlia Client connection: FAILED.")); return false; }
//	else { UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::ConnectToDevice()] HPGlia Client connection: SUCCESS.")); }
//
//	//UHPGliaClient::ConnectToGliaAsync(HPGliaKeys::CLIENT_ID, HPGliaKeys::ACCESS_KEY, ELicensingModel::REV_SHARE);
//	return true;;
//}

bool FGetCLMonitorComponentThread::GetEyeTracking(FEyeTracking& OutEyeTracking) {
	return UHPGliaClient::GetEyeTracking(EyeData);
}

void FGetCLMonitorComponentThread::ConvertCombinedGazeToLocation()
{
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

FString FGetCLMonitorComponentThread::GetVariableHeader() 
{
	const FString H1 = "SystemTime,CombinedGaze_x,CombinedGaze_y,CombinedGaze_z,CombinedGaze_Confidence,LeftGaze_x,LeftGaze_y, LeftGaze_z,LeftGaze_Confidence,RightGaze_x,RightGaze_y,RightGaze_z,RightGaze_Confidence,";
	const FString H2 = "LeftPupilPosition_x,LeftPupilPosition_y,LeftPupilPosition_z,LeftPupilPosition_Confidence,RightPupilPosition_x,RightPupilPosition_y,RightPupilPosition_z,RightPupilPosition_Confidence,";
	const FString H3 = "LeftPupilDilation,LeftPupilDilation_Confidence,RightPupilDilation,RightPupilDilation_Confidence,LeftEyeOpenness,LeftEyeOpenness_Confidence,RightEyeOpenness,RightEyeOpenness_Confidence\n";
	return H1 + H2 + H3;
}

uint32 FGetCLMonitorComponentThread::Run()
{
	const FString ProjectDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FString filename = ProjectDirectory + "Data/data.csv";
	FString save_line = FGetCLMonitorComponentThread::GetVariableHeader();
		
	UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::Run()] HPGlia::GetEyeTracking() saving line -> %s"), *save_line);

	/* check omnicept runtime connection */
	FGetCLMonitorComponentThread::ConnectToDevice();

	/* check HMD */
	if (!GEngine->XRSystem->GetHMDDevice()->IsHMDConnected()) {
		UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::Run()] HMD NOT connected."));
		UE_DEBUG_BREAK();
	}

	/* to do: change bStopThread to GS */
	int i = 0; 
	while (!bStopThread) {

		/* collect sample */
		FEyeTracking HPEye;
		UHPGliaClient::GetEyeTracking(HPEye);

		/* push sample GetCLMonitorActor*/
		CurrentThreadActor->eye_combined_gaze = HPEye.CombinedGaze;

		/* save the data */
		save_line = HPEye.SystemTime.ToString() + "," + 
			FString::SanitizeFloat(HPEye.CombinedGaze.X) + "," + FString::SanitizeFloat(HPEye.CombinedGaze.Y) + "," + FString::SanitizeFloat(HPEye.CombinedGaze.Z) + "," +
			FString::SanitizeFloat(HPEye.LeftGaze.X) + "," + FString::SanitizeFloat(HPEye.LeftGaze.Y) + "," + FString::SanitizeFloat(HPEye.LeftGaze.Z) + "," + FString::SanitizeFloat(HPEye.LeftGazeConfidence) + "," +
			FString::SanitizeFloat(HPEye.RightGaze.X) + "," + FString::SanitizeFloat(HPEye.RightGaze.Y) + "," + FString::SanitizeFloat(HPEye.RightGaze.Z) + "," + FString::SanitizeFloat(HPEye.RightGazeConfidence) + "," +
			FString::SanitizeFloat(HPEye.LeftPupilPosition.X) + "," + FString::SanitizeFloat(HPEye.LeftPupilPosition.Y) + "," + FString::SanitizeFloat(HPEye.LeftPupilPosition.Z) + "," + FString::SanitizeFloat(HPEye.LeftPupilPositionConfidence) + "," +
			FString::SanitizeFloat(HPEye.RightPupilPosition.X) + "," + FString::SanitizeFloat(HPEye.RightPupilPosition.Y) + "," + FString::SanitizeFloat(HPEye.RightPupilPosition.Z) + "," + FString::SanitizeFloat(HPEye.RightPupilPositionConfidence) + "," +
			FString::SanitizeFloat(HPEye.LeftPupilDilation) + "," + FString::SanitizeFloat(HPEye.LeftPupilDilationConfidence) + "," + FString::SanitizeFloat(HPEye.RightPupilDilation) + "," + FString::SanitizeFloat(HPEye.RightPupilDilationConfidence) + "," +
			FString::SanitizeFloat(HPEye.LeftEyeOpenness) + "," + FString::SanitizeFloat(HPEye.LeftEyeOpennessConfidence) + "," + FString::SanitizeFloat(HPEye.RightEyeOpenness) + "," + FString::SanitizeFloat(HPEye.RightEyeOpennessConfidence) + "\n";
		
		/* print eye data */
		UE_LOG(LogTemp, Log, TEXT("[FGetCLMonitorComponentThread::Run()] HPGlia::GetEyeTracking() saving line -> %s"), *save_line);
		IFileManager *FileManager = nullptr;

		if (!UTextFileManager::SaveTxt(*save_line, *filename)) { UE_DEBUG_BREAK(); }

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
