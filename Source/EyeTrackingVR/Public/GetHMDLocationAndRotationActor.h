#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GetHMDLocationAndRotation.h"
#include "Core/Public/HAL/Runnable.h"
#include "Core/Public/HAL/RunnableThread.h"
#include "GetHMDLocationAndRotationActor.generated.h"

UCLASS()
class EYETRACKINGVR_API AGetHMDLocationAndRotationActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGetHMDLocationAndRotationActor();

	UFUNCTION(BlueprintCallable, Category = "HMDInformation")
	void InitThread(int32 _Calculations);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void printData();
	int32 ProcessedDataCount;
	FVector ProcessedHMDData;

	class FGetHMDLocationAndRotation* WorkerThread = nullptr;
	FRunnableThread* CurrentRunningThread = nullptr;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	TQueue<int32> WorkerDataQueue;
	TQueue<FVector> HMDDataQueue;
	TQueue<UWorld*> WorldQueue;
	UWorld* ProcessedWorld;

	UWorld* getWorldReferenceFromActor();

};
