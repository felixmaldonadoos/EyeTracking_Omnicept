// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeMain.h"
#include "PawnMain.h"
#include "GameStateMain.h"
#include "MouseKeyboardPlayerController.h"

AGameModeMain::AGameModeMain()
{
	/* Get PawnMain_BP to spawn */
	DefaultPawnClass = APawnMain::StaticClass();

	/* Assign default game state */
	GameStateClass = AGameStateMain::StaticClass();

	/* assing default (for now) player controller */
	PlayerControllerClass = AMouseKeyboardPlayerController::StaticClass(); 

	/* standard defaults */
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = false;

}

void AGameModeMain::EndMatch()
{
	UE_LOG(LogTemp, Warning, TEXT("[ AGameModeMain::EndMatch()] Force quit."));
	FGenericPlatformMisc::RequestExit(false);
}

void AGameModeMain::SpawnAndPossessPlayer(FVector spawn_location, FRotator spawn_rotation)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	APawnMain* TempChar = Cast<APawnMain>(GetWorld()->SpawnActor(DefaultPawnClass, &spawn_location, &spawn_rotation, SpawnInfo));
	EAutoReceiveInput::Type::Player0;
	EAutoPossessAI::PlacedInWorldOrSpawned;
	GetWorld()->GetFirstPlayerController()->Possess(TempChar);
}

void AGameModeMain::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void AGameModeMain::InitGameState()
{
	Super::InitGameState();
}

void AGameModeMain::StartPlay()
{
	Super::StartPlay();
	UE_LOG(LogTemp, Warning, TEXT("[AGameModeMain::StartPlay()] Starting game!\n"));
}
