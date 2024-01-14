#pragma once 
#include "GetCLMonitorComponentThread.h" 
#include "GameStateMain.h"
#include "GameModeMain.h"
#include "MouseKeyboardPlayerController.h"
#include "GetCLMonitorComponentActor.h"
#include "EyeTrackerFunctionLibrary.h"
#include "Misc/DateTime.h" 
#include "Keys.h"
#include "Kismet/GameplayStatics.h" 
#include "Kismet/KismetStringLibrary.h" 


/* changes state of bIsConnected*/
bool FGetCLMonitorComponentThread::ConnectToDevice() {
	UHPGliaClient::ConnectToGliaAsync(HPGliaKeys::CLIENT_ID, HPGliaKeys::ACCESS_KEY, requestedLicense);
	bIsConnected = UHPGliaClient::IsConnected();
	return bIsConnected;
}

bool FGetCLMonitorComponentThread::GetEyeTracking(FEyeTracking& OutEyeTracking) {
	return  UHPGliaClient::GetEyeTracking(EyeData);
}


FGetCLMonitorComponentThread::FGetCLMonitorComponentThread(AGetCLMonitorComponentActor* funActor, FString SaveDataDirectory)
{
	CurrentThreadActor = funActor; 
	/* init eye-tracker */
	bIsConnected = false;
	FGetCLMonitorComponentThread::ConnectToDevice();
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

	/* check HMD */
	//IStereoRendering RenderingPtr = nullptr; 
	if (!GEngine->XRSystem->GetHMDDevice()->IsHMDConnected()) {
		UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::Run()] HMD NOT connected."));
	}

	/* initialize CL monitor */
	bool bIsEyeTrackerConnected = UEyeTrackerFunctionLibrary::IsEyeTrackerConnected();
	int n_attempts = 10; 
	bIsEyeTrackerConnected = true; // debug (currently returns false).
	if (!bIsEyeTrackerConnected) {
		for (int i = 0; i < n_attempts; ++i) {
			UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::Run()] Eyetracker not connected. Attempting to connect (%i/%i)."), i + 1, n_attempts);
			bIsEyeTrackerConnected = UEyeTrackerFunctionLibrary::IsEyeTrackerConnected();
			FPlatformProcess::Sleep(1); // seconds
			if (bIsEyeTrackerConnected) { break; }
		}
		if (!bIsEyeTrackerConnected) { UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::Run()] HMD NOT found. Stopping."));  this->Stop(); }
	}

	UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::Run()] Eyetracker found. Nice :o"));

	/* get query counter */
	 
	/* set up file system */

	/* set up local params (if not done globally or input) */

	/* to do: change bStopThread to GS */
	int i = 0; 
	while (!bStopThread) {

		/* set player to track (if valid) */
		APlayerController* PlayerController = nullptr; 
		PlayerController = CurrentThreadActor->GetWorld()->GetFirstPlayerController();
		if (PlayerController != nullptr) {
			UEyeTrackerFunctionLibrary::SetEyeTrackedPlayer(CurrentThreadActor->GetWorld()->GetFirstPlayerController());
			UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::Run()] found player controller"));
		}
		/* main loop */

		World = CurrentThreadActor->GetWorld();
		
		/* grab sample  */

		/* save sample or add to buffer */


		/* test (works, 1/13/2024*/
		i += 1; 
		UE_LOG(LogTemp, Warning, TEXT("[FGetCLMonitorComponentThread::Run()] In main thread loop., %i"), i);
		FPlatformProcess::Sleep(0.1f); // sleep 

		if (bStopThread) {
			UE_LOG(LogTemp, Warning, TEXT("break"));
		}
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
