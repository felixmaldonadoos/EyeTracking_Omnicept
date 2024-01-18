#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Public/HAL/Runnable.h"
#include "Core/Public/HAL/RunnableThread.h"
#include "HeadMountedDisplayTypes.h" 
#include "IXRTrackingSystem.h"
#include "StereoRendering.h"
#include "IHeadMountedDisplay.h"
#include "GetCLMonitorComponentThread.h"
#include "GetCLMonitorComponentActor.generated.h"

UCLASS()
class EYETRACKINGVR_API AGetCLMonitorComponentActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGetCLMonitorComponentActor();

	bool Calibrate();

private:
	FString SaveDataDirectory;

protected:
	// Called when the game starts or when spawned
	void InitThread();

	UWorld* WorldRef = nullptr;
	UWorld* tempWorld = nullptr;

	class FGetCLMonitorComponentThread* WorkerThread = nullptr;
	FRunnableThread* CurrentRunningThread = nullptr;

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UWorld* GetWorldReferenceFromActor();

	/* to call end of thread */
	void Stop();



};
