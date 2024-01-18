#include "GetCLMonitorComponentActor.h"

AGetCLMonitorComponentActor::AGetCLMonitorComponentActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
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

void AGetCLMonitorComponentActor::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("[AGetCLMonitorComponentActor::Tick()]"));
	Super::Tick(DeltaTime);
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
