#include "GetCLMonitorComponentActor.h"

AGetCLMonitorComponentActor::AGetCLMonitorComponentActor()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bTickEvenWhenPaused   = true;
}

bool AGetCLMonitorComponentActor::InitializeHPKeys() {
	TArray<FString> hp_client_info;
	UWorld* World = nullptr;
	UGameInstanceMain* GI = nullptr;

	if (GEngine) { World = GEngine->GetWorld(); }
	else { return false; }

	if (World) { GI = Cast<UGameInstanceMain>(UGameplayStatics::GetGameInstance(World)); }
	else { return false; }

	if (!GI) { return false; }
	bool res = UConfigManager::LoadHPClientKeys(GI, hp_client_info);

	return res; 
} 

bool AGetCLMonitorComponentActor::Calibrate()
{
	if (GEngine) {
		GEngine->XRSystem->GetStereoRenderingDevice()->EnableStereo(false);
		GEngine->XRSystem->GetHMDDevice()->EnableHMD(false);
	}
	FString ENV_VAR = TEXT("HP_OMNICEPT_INSTALL");
	FString eyePath = FWindowsPlatformMisc::GetEnvironmentVariable(*ENV_VAR);
	eyePath = eyePath.Append(TEXT("\\..\\..\\HP Omnicept\\HP Omnicept Eye Tracking Calibration\\ETCal\\Binaries\\Win64\\ETCal-Win64-Shipping.exe"));
	FProcHandle handle = FPlatformProcess::CreateProc(*eyePath, nullptr, true, false, false, nullptr, 0, nullptr, nullptr);

	if (handle.IsValid())
	{
		FPlatformProcess::WaitForProc(handle);


		if (GEngine) {
			GEngine->XRSystem->GetStereoRenderingDevice()->EnableStereo(true);
			GEngine->XRSystem->GetHMDDevice()->EnableHMD(true);
		}

		FString fileContent = "famal";
		FString userName = UKismetSystemLibrary::GetPlatformUserName();
		FString filePath = "C:\\Users\\";
		filePath = filePath.Append(userName);
		filePath = filePath.Append("\\AppData\\Local\\ETCal\\Saved\\etcallog.log");

		FFileHelper::LoadFileToString(fileContent, *filePath);

		return fileContent.Contains("Tobii Eye Calibration finished successfully");
	}

	if (GEngine) {
		GEngine->XRSystem->GetStereoRenderingDevice()->EnableStereo(true);
		GEngine->XRSystem->GetHMDDevice()->EnableHMD(true);
	}

	return false;

}

void AGetCLMonitorComponentActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[AGetCLMonitorComponentActor::BeginPlay] Start."));

	/* get keys */
	//if (!AGetCLMonitorComponentActor::InitializeHPKeys()) {
	//	UE_DEBUG_BREAK();
	//}

	/* run calibration */
	//Calibrate();
	InitThread();
}

void AGetCLMonitorComponentActor::InitThread()
{
	UE_LOG(LogTemp, Warning, TEXT("[AGetCLMonitorComponentActor::InitThread] Start."));

	WorldRef = nullptr;
	if (GEngine) {
		WorldRef = GEngine->GetWorld();
		if (WorldRef == nullptr) {
			WorldRef = GetWorld();
		}
	}

	if (WorldRef == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[AGetPlayerTraversePathActor::InitThread] World ref from actor null. Aborting."));
		return;
	}

	/* to do: pass pawn reference */
	UE_LOG(LogTemp, Error, TEXT("[AGetPlayerTraversePathActor::InitThread] Before Worker Thread."));

	/* create thread object */
	WorkerThread = new FGetCLMonitorComponentThread(this, SaveDataDirectory);
	CurrentRunningThread = FRunnableThread::Create(WorkerThread, TEXT("t_GetCLMonitorComponentThread"));
}

bool AGetCLMonitorComponentActor::IsVectorAllZeros(const FVector& Vec)
{
	return Vec.IsZero();
}

bool AGetCLMonitorComponentActor::IsVectorAllNegativeOnes(const FVector& Vec)
{
	return FMath::IsNearlyEqual(Vec.X, -1.0f) && FMath::IsNearlyEqual(Vec.Y, -1.0f) && FMath::IsNearlyEqual(Vec.Z, -1.0f);
}

bool AGetCLMonitorComponentActor::DrawEyeTraceOnPlayer()
{
	/* make sure that the eye data array isn't empty */
	if (!AGetCLMonitorComponentActor::IsVectorAllNegativeOnes(eye_combined_gaze) ||
		!AGetCLMonitorComponentActor::IsVectorAllZeros(eye_combined_gaze)) {
		UE_LOG(LogTemp, Error, TEXT("[AGetCLMonitorComponentActor::DrawEyeTraceOnPlayer()] eye_combined_gaze not valid."));
		return false;
	}

	/* check if game is running and world is valid*/
	if (GEngine->GetWorld()) {
		WorldRef = GEngine->GetWorld();
	}

	/* get world reference */
	if (!WorldRef) {
		UE_LOG(LogTemp, Error, TEXT("[AGetCLMonitorComponentActor::DrawEyeTraceOnPlayer()] WorldRef == nullptr ."));
		return false;
	}

	/* get player controller */
	MouseKeyboardPlayerController = Cast<AMouseKeyboardPlayerController>(UGameplayStatics::GetPlayerController(WorldRef, player_index));
	if (!MouseKeyboardPlayerController) {
		UE_LOG(LogTemp, Error, TEXT("[AGetCLMonitorComponentActor::DrawEyeTraceOnPlayer()] PlayerController == nullptr ."));
		return false;
	}

	/* get pawn*/
	Pawn = Cast<APawnMain>(MouseKeyboardPlayerController->GetPawn());
	if (!Pawn) {
		UE_LOG(LogTemp, Error, TEXT("[AGetCLMonitorComponentActor::DrawEyeTraceOnPlayer()] PawnMain == nullptr ."));
		return false;
	}

	/* get camera component */
	CameraComponent = Cast<UCameraComponent>(GetComponentByClass(UCameraComponent::StaticClass()));
	if (!CameraComponent) {
		UE_LOG(LogTemp, Error, TEXT("[AGetCLMonitorComponentActor::DrawEyeTraceOnPlayer()] CameraComponent == nullptr ."));
		return false;
	}

	/*psuedo:
	* get combined gaze vector
	* get camera world transform
	* start_location = GetWorldLocation(PlayerCamera)
	* end_location   = start_location + TransformDirection(T,CombinedGazeVec)*magnitude(250)
	*
	*/

	/* remove the first sample */
	return true;
}

void AGetCLMonitorComponentActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("[AGetCLMonitorComponentActor::Tick()]"));
	AGetCLMonitorComponentActor::DrawEyeTraceOnPlayer();
}


void AGetCLMonitorComponentActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (CurrentRunningThread && WorkerThread) {
		// need to suspen thread THEN stop it. You can't stop and kill randomly
		CurrentRunningThread->Suspend(true);
		WorkerThread->bStopThread = true;
		CurrentRunningThread->Suspend(false);
		CurrentRunningThread->Kill(false); // was false
		CurrentRunningThread->WaitForCompletion();
	}

	/* destroy this actor */
	UE_LOG(LogTemp, Log, TEXT("[AGetCLMonitorComponentActor::EndPlay()] Calling Destroy()"));

}


UWorld* AGetCLMonitorComponentActor::GetWorldReferenceFromActor()
{
	return GEngine->GetWorld();
}

void AGetCLMonitorComponentActor::Stop()
{
	UE_LOG(LogTemp, Log, TEXT("[AGetPlayerTraversePathActor::Stop()]"));
}

