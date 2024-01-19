#include "GetCLMonitorComponentActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

AGetCLMonitorComponentActor::AGetCLMonitorComponentActor()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
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

bool AGetCLMonitorComponentActor::IsVectorAllZeros(const FVector Vec)
{
	return Vec.IsZero(); // true if zeros
}

bool AGetCLMonitorComponentActor::IsVectorAllNegativeOnes(const FVector Vec)
{
	FVector NegOnes = { -1.0f,1.0f,-1.0f };
	return Vec == NegOnes;
}

bool AGetCLMonitorComponentActor::IsEyeDataValid(FVector Vec) {
	if (AGetCLMonitorComponentActor::IsVectorAllNegativeOnes(eye_combined_gaze) ||
		AGetCLMonitorComponentActor::IsVectorAllZeros(eye_combined_gaze)) {
		UE_LOG(LogTemp, Error, TEXT("[AGetCLMonitorComponentActor::DrawEyeTraceOnPlayer()] eye_combined_gaze not valid."));
		return false;
	}
	else {
		return true;
	}
}
bool AGetCLMonitorComponentActor::IsWorldValid(UWorld*& World) {
	
	if (GetWorld()) {
		World = GetWorld();
		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("[AGetCLMonitorComponentActor::IsWorldValid] GEngine->GetWorld() not valid."));
	return false;
}

bool AGetCLMonitorComponentActor::GetPlayerCameraComponent(UCameraComponent*& Camera) {
	/* get player controller */
	MouseKeyboardPlayerController = Cast<AMouseKeyboardPlayerController>(UGameplayStatics::GetPlayerController(WorldRef, player_index));
	if (!MouseKeyboardPlayerController) {
		UE_LOG(LogTemp, Error, TEXT("[AGetCLMonitorComponentActor::GetPlayerCameraComponent] PlayerController == nullptr ."));
		return false;
	}

	/* get pawn*/
	Pawn = Cast<APawnMain>(MouseKeyboardPlayerController->GetPawn());
	if (Pawn == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[AGetCLMonitorComponentActor::GetPlayerCameraComponent] PawnMain == nullptr ."));
		return false;
	}

	/* new test */

	Camera = Pawn->GetCameraComponent();
	if (Camera == nullptr) {
		return false;
	}
	/*
	* 
	* old 
	* 
	UE_LOG(LogTemp, Warning, TEXT("[AGetCLMonitorComponentActor::GetPlayerCameraComponent] PawnMain id: %s."), *Pawn->GetName());
	CameraComponent = Cast<UCameraComponent>(GetComponentByClass(UCameraComponent::StaticClass()));
	UActorComponent* ActorComponent = GetComponentByClass(UCameraComponent::StaticClass());
	if (CameraComponent == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[AGetCLMonitorComponentActor::GetPlayerCameraComponent] CameraComponent == nullptr ."));
		return false;
	}
	*/

	return true;
}

bool AGetCLMonitorComponentActor::DrawEyeTraceOnPlayer()
{
	/* make sure that the eye data array isn't empty */
	if (!AGetCLMonitorComponentActor::IsEyeDataValid(eye_combined_gaze)) {
		return false;
	}

	/* check if game is running and world is valid*/
	if (!AGetCLMonitorComponentActor::IsWorldValid(WorldRef)) {
		return false;
	}

	/* get camera component */
	if (!AGetCLMonitorComponentActor::GetPlayerCameraComponent(CameraComponent)) {
		return false;
	}

	/* start tracing */
	const FVector trace_start = CameraComponent->GetComponentLocation();
	const FVector trace_end = trace_start + UKismetMathLibrary::TransformDirection(CameraComponent->GetComponentTransform(), eye_combined_gaze)*250;
	//const FVector trace_end = UKismetMathLibrary::TransformDirection(CameraComponent->GetComponentTransform(), eye_combined_gaze);
	FHitResult hit_result;
	FCollisionQueryParams collision_params;
	GetWorld()->LineTraceSingleByChannel(hit_result, trace_start, trace_end, ECollisionChannel::ECC_Visibility, collision_params);
	DrawDebugLine(GetWorld(), trace_start, trace_end, FColor::Red, false, 0.2);

	/*psuedo:
	* 
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

