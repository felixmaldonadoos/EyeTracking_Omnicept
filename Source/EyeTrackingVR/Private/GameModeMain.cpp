// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeMain.h"
#include "PawnMain.h" 
#include "GameStateMain.h"
#include "HPGlia.h"
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
	PrimaryActorTick.bCanEverTick = true;

}

void AGameModeMain::EndMatch()
{
	UE_LOG(LogTemp, Warning, TEXT("[ AGameModeMain::EndMatch()] Force quit."));
	FGenericPlatformMisc::RequestExit(false);
}

/* 
* Updates GameInstance with HP keys. Will use variables inside GameInstanceMain.h 
* to find, load, and process the HP keys. 
*/
bool AGameModeMain::InitializeHPKeys() {
	TArray<FString> hp_client_info; 
	UWorld* World = nullptr;
	UGameInstanceMain* GI = nullptr; 

	if (GEngine) { World = GEngine->GetWorld(); }
	else { return false;  }
	if (World) { GI = Cast<UGameInstanceMain>(UGameplayStatics::GetGameInstance(World)); }
	else { return false;  }

	return UConfigManager::LoadHPClientKeys(GI, hp_client_info);
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

void AGameModeMain::SpawnGetCLMonitorComponentActor()
{
	FTransform tSpawnTransform;
	FVector TempLoc = { 0.0f, 0.0f, 0.0f };
	FRotator TempRot = tSpawnTransform.GetRotation().Rotator();

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetCLMonitorComponentActor = Cast<AGetCLMonitorComponentActor>(GetWorld()->SpawnActor(AGetCLMonitorComponentActor::StaticClass(), &TempLoc, &TempRot, SpawnInfo));
}

void AGameModeMain::SpawnAllLoggingActor()
{
	/* eye-tracker */
	AGameModeMain::SpawnGetCLMonitorComponentActor();

	/* player path */
}


void AGameModeMain::StartPlay()
{
	Super::StartPlay();
	UE_LOG(LogTemp, Warning, TEXT("[AGameModeMain::StartPlay()] Starting game!\n"));

	/* spawn player */
	AGameModeMain::SpawnAndPossessPlayer(spawn_location_player, spawn_rotation_player);

	/* spawn eyetracker monitor */
	AGameModeMain::SpawnGetCLMonitorComponentActor();

	//if (AGameModeMain::InitializeHPKeys()) {
	//	UE_DEBUG_BREAK();
	//}

	//const FString access_key = "F8OK38DWnRgqJgr5aaUhgcfBPHoEe5toBiDGGREkR2DWeZxgTKFpCF5YvAdnHd-S";
	//const FString client_id = "25b17c6b-3386-45f8-9e1e-88d76259b5bf";
	//UHPGliaClient::ConnectToGliaAsync(client_id, access_key, ELicensingModel::CORE);

}

void AGameModeMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
