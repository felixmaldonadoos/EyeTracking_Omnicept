#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Containers/Array.h" 
#include "GameInstanceMain.generated.h"

/**
 * This file controls variables that need to be present from start to finish of entire experiment. 
 * Variables in UGameInstance are present (and accessible if public) during the whole runtime of 
 * the game/experiment executable. If you want variables that die after a game mode ends (or a trial 
 * for example) refer to GameState.
 */
UCLASS()
class EYETRACKINGVR_API UGameInstanceMain : public UGameInstance
{
	GENERATED_BODY()
public:

	UGameInstanceMain();

	/* override functions frmo UGameInstance */
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void StartGameInstance() override;

	/* file management */
	FString dir_project;
	FString dir_setup;
	FString dir_savedata;
	FString file_hp_client_keys;

	/* HP stuff */
	FString client_id; 
	FString access_key; 

	/* timer stuff */
	int32 InitialTimer_Instance;
	int32 AddTimer_Instance;
	int32 VRMode_Instance;

	/* can we go to next phase? terminates widget transition */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanTransition = 0;

	/* controller mode selection (VR or Mouse + Keyboard) */
	int ControllerMode = 0; // default to VR unless stated in config.txt
};