#pragma once 
#include "GetCLMonitorComponentThread.h" 
#include "GameStateMain.h"
#include "GameModeMain.h"
#include "GetCLMonitorComponentActor.h"
#include "Misc/DateTime.h" 
#include "Kismet/GameplayStatics.h" 
#include "Kismet/KismetStringLibrary.h" 

FGetCLMonitorComponentThread::FGetCLMonitorComponentThread(AGetCLMonitorComponentActor* funActor, FString SaveDataDirectory)
{
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

	/* to do: change bStopThread to GS */
	while (!bStopThread) {

		/* main loop */
		FPlatformProcess::Sleep(0.01f); // sleep 
	}

	UE_LOG(LogTemp, Warning, TEXT("[FGetPlayerTraversePath::Run()] Ending thread."));
	return 0;
}


void FGetCLMonitorComponentThread::Stop()
{
	bStopThread = true;
}

void FGetCLMonitorComponentThread::Exit()
{
}
