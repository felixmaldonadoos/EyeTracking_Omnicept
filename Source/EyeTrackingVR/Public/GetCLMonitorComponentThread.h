#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h" 
#include "Core/Public/HAL/RunnableThread.h"
#include "Core/Public/HAL/Runnable.h"
#include "GameFramework/Actor.h"
//#include "GetPlayerTraversePath.generated.h"

class FRunnableThread;
class AGetCLMonitorComponentActor;

class EYETRACKINGVR_API FGetCLMonitorComponentThread : public FRunnable
{

public:
	// Sets default values for this actor's properties
	FGetCLMonitorComponentThread(AGetCLMonitorComponentActor* funActor, FString SaveDataDirectory);
	virtual ~FGetCLMonitorComponentThread();

	// FRunnable functions 
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	bool bStopThread;
private:
	AGetCLMonitorComponentActor* CurrentThreadActor;

protected:
	// Called when the game starts or when spawned
	FRunnableThread* Thread = nullptr;

	// to save data 
	int32 DataCount = 0;
	TArray<FVector> DataBuffer;

	// temp vars
	UWorld* tempWorld;
	APawn* tempPawn;

};
