// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GetCLMonitorComponentActor.h"
//#include "PawnMain.h"
//#include "Templates/SharedPointer.h" 
#include "GameModeMain.generated.h"

/**
 *
 */
UCLASS()
class EYETRACKINGVR_API AGameModeMain : public AGameModeBase
{
public:
	GENERATED_BODY()
	virtual void EndMatch();

	
protected:
	AGameModeMain();
	FVector debug_vect;

	APlayerController* PlayerControllerClassCast = nullptr;
	FVector InitialPlayerLocation;
	FRotator InitialPlayerRotation;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/* spawning player */
	void SpawnAndPossessPlayer(FVector spawn_location, FRotator spawn_rotation);
	//APawnMain* PlayerActor; 



	/* Spawns Sensors */
	void SpawnGetCLMonitorComponentActor();
	AGetCLMonitorComponentActor* GetCLMonitorComponentActor = nullptr;

	/* spawn everything */
	void SpawnAllLoggingActor(); 
	
	/* to store waypoint info  */
	virtual void InitGameState();
	virtual void StartPlay() override;

};
