
#pragma once
#include "CoreMinimal.h"
#include "Engine/World.h" 
#include "Kismet/GameplayStatics.h" 
#include "Core/Public/HAL/RunnableThread.h"
#include "Core/Public/HAL/Runnable.h"

/* forward declarations */
class FRunnableThread;
class AGetHMDLocationAndRotationActor;

class FGetHMDLocationAndRotation : public FRunnable
{
public:
	/* constructor */
	FGetHMDLocationAndRotation(UWorld* refWorld, int32 _Calculation, AGetHMDLocationAndRotationActor* FunActor);

	// The boolean that acts as the main switch
	// When this is false, inputs and outputs can be safely read from game thread
	bool bInputReady = false;
	bool bRunThread = false;
	bool bStopThread;

	// Declare the variables that are the inputs and outputs here.
	// You can have as many as you want. Remember not to use pointers, this must be
	// plain old data
	// For example:
	int ExampleIntInput = 0;
	float ExampleFloatOutput = 0.0f;

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent);
	//TStatId GetStatId() const;
	/* thread safe */
	TQueue<int32> ThreadQueue;
	//AGameState_Main* const GS; 

	/* output data */
	FVector resDirection;
	FRotator resRotation;
private:
	int32 Calculations;
	int32 DataCount; // iterator 
	//UWorld* World; 

	AGetHMDLocationAndRotationActor* CurrentThreadActor;
	int32 CurrentCalculation; // to store current value
	UWorld* ProcessedWorld;
};