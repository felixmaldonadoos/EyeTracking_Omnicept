#pragma once
#include "GetHMDLocationAndRotation.h"
#include "GetHMDLocationAndRotationActor.h"
//#include "MyGameInstance.h"
#include "HeadMountedDisplayTypes.h" 
#include "GenericPlatform/GenericPlatformProcess.h" 
#include "Math/Rotator.h" 
#include "Math/Vector.h" 
#include "IXRTrackingSystem.h"
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "IHeadMountedDisplay.h"
#include "UObject/UObjectBaseUtility.h" 
#include "Engine/Engine.h"
#include "Engine/World.h" 
#include "Async/TaskGraphInterfaces.h" 
#include "Stats/Stats.h" 
#include "Engine/GameEngine.h"

/*APawn Player,*/
FGetHMDLocationAndRotation::FGetHMDLocationAndRotation(UWorld* refWorld, int32 _Calculation, AGetHMDLocationAndRotationActor* FunActor)
{
	if (_Calculation > 0 && FunActor) {
		Calculations = _Calculation;
		CurrentThreadActor = FunActor;
		//World = refWorld; // passed as world context to GetHMDData(). 
	}
}

bool FGetHMDLocationAndRotation::Init()
{
	bStopThread = false;
	DataCount = 0;
	UE_LOG(LogTemp, Error, TEXT("[FGetHMDLocationAndRotation::Init()]."));

	return true;
}

void FGetHMDLocationAndRotation::DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) {
	FGetHMDLocationAndRotation::Run();
}

//TStatId FGetHMDLocationAndRotation::GetStatId() const 
//{
//	return UObject::GetStatID();
//}

uint32 FGetHMDLocationAndRotation::Run()
{

	while (!bStopThread) {

		//if (DataCount < Calculations) {
		/* main task start */
		FVector Loc;
		FQuat Quat;
		//UObject* WorldContext = nullptr;
		FXRHMDData* HMDData = new FXRHMDData();
		UWorld* tempWorld = nullptr;
		tempWorld = CurrentThreadActor->getWorldReferenceFromActor();
		if (tempWorld) {
			UE_LOG(LogTemp, Warning, TEXT("[FGetHMDLocationAndRotation::Run()] Got Game State."));
			int c = 30;
		}


		if (GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->StereoRenderingDevice.IsValid()) {
			//UE_LOG(LogTemp, Warning, TEXT("HMD found and valid."))
			tempWorld = CurrentThreadActor->getWorldReferenceFromActor();
			if (tempWorld && tempWorld->IsValidLowLevelFast()) {
				UE_LOG(LogTemp, Warning, TEXT("[FGetHMDLocationAndRotation::Run()] tempWorld->IsValidLowLevelFast() == True. "));

				if (tempWorld) {
					GEngine->XRSystem->GetHMDData(tempWorld->GetWorld(), *HMDData);
					////CurrentThreadActor->HMDDataQueue.Enqueue(Loc); // 21:46
				}
				if (HMDData && HMDData->bValid) {
					UE_LOG(LogTemp, Warning, TEXT("HMDData valid"));
					Loc = HMDData->Position;
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("HMDData invalid"));
				}
				UE_LOG(LogTemp, Warning, TEXT("HMD valid and found.-> (%f, %f, %f)"), Loc.X, Loc.Y, Loc.Z);
			}
			else if (tempWorld == nullptr) {
				UE_LOG(LogTemp, Error, TEXT("World == nullptr"));
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("else"));

			}
		}

		FPlatformProcess::Sleep(0.1f);
		DataCount = DataCount + 1;
		CurrentThreadActor->WorkerDataQueue.Enqueue(DataCount); // 21:46
	}

	UE_LOG(LogTemp, Warning, TEXT("Ending thread."));

	return 0;
}

void FGetHMDLocationAndRotation::Stop()
{
	/* call stop thread */
	bStopThread = true;
}


