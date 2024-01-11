
#pragma once
#include "GetHMDLocationAndRotationActor.h"
#include "Engine/World.h" 
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "HeadMountedDisplayTypes.h" 
#include "GameFramework/Pawn.h" 
#include "GameFramework/Controller.h" 

#include "Engine/GameEngine.h"
#include "Engine/Engine.h"
#include "Engine/GameEngine.h"

#include "Async/Async.h"
#include "Async/TaskGraphInterfaces.h" 
#include "Stats/Stats.h" 

#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"
#include "UObject/Object.h" 
#include "GetHMDLocationAndRotation.h"

// Sets default values
AGetHMDLocationAndRotationActor::AGetHMDLocationAndRotationActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}


void AGetHMDLocationAndRotationActor::InitThread(int32 _Calculations)
{
	/* https://docs.unrealengine.com/5.1/en-US/API/Runtime/Engine/Engine/UEngine/XRSystem/
	https://github.com/stereolabs/zed-unreal-examples/blob/master/UE4_Examples/Source/ZEDSamples/Private/SamplesBlueprintFunctionLibrary.cpp
	*/
	UWorld* WorldRef = nullptr;
	if (GEngine) {
		WorldRef = GEngine->GetWorld();
		if (WorldRef == nullptr) {
			WorldRef = GetWorld();
		}
	}


	if (WorldRef == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[AGetHMDLocationAndRotationActor::InitThread] World ref from actor null."));
	}

	if (_Calculations > 0 && WorldRef != nullptr) {
		/* to do: pass pawn reference */
		WorkerThread = new FGetHMDLocationAndRotation(WorldRef, _Calculations, this);
		CurrentRunningThread = FRunnableThread::Create(WorkerThread, TEXT("[AGetHMDLocationAndRotationActor::InitThread] Worker Thread"));

		//FGraphEventRef BuildCompleteEvent = TGraphTask<FGetHMDLocationAndRotation>::CreateTask().ConstructAndDispatchWhenReady(WorldRef);

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("[AGetHMDLocationAndRotationActor::InitThread] Calculations must be greater than 0."));
	}
}

UWorld* AGetHMDLocationAndRotationActor::getWorldReferenceFromActor()
{
	return GetWorld();
}


// Called when the game starts or when spawned
void AGetHMDLocationAndRotationActor::BeginPlay()
{
	Super::BeginPlay();
}

void AGetHMDLocationAndRotationActor::printData()
{
	/*if (!WorkerDataQueue.IsEmpty() && WorkerDataQueue.Dequeue(ProcessedDataCount)) {
		UE_LOG(LogTemp, Warning, TEXT("Processed calculation: %d "), ProcessedDataCount);
	}*/

	/*if (!HMDDataQueue.IsEmpty() && HMDDataQueue.Dequeue(ProcessedHMDData)) {
		UE_LOG(LogTemp, Warning, TEXT("Processed HMD Data: (%f, %f, %f) "), ProcessedHMDData.X, ProcessedHMDData.Y, ProcessedHMDData.Z);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Processed HMD Data: EMPTY"));

	}*/
}

// Called every frame
void AGetHMDLocationAndRotationActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//printData();
	//UWorld* refWorld = GEngine->GetWorld(); 

	//this->WorldQueue.Enqueue(refWorld);
}

void AGetHMDLocationAndRotationActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// make sure if we have a valid current thread 
	if (CurrentRunningThread && WorkerThread) {
		// need to suspen thread THEN stop it. You can't stop and kill randomly
		CurrentRunningThread->Suspend(true);
		WorkerThread->bStopThread = true;
		CurrentRunningThread->Suspend(false);
		CurrentRunningThread->Kill(false);
		// here we wanna wait for other threads (not implemented rn)
		CurrentRunningThread->WaitForCompletion();
		delete WorkerThread;
	}
}

