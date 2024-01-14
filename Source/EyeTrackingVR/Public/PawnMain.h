#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h" 
#include "Components/WidgetInteractionComponent.h" 
#include "Components/WidgetComponent.h"
#include "GameFramework/Pawn.h"
#include "PawnMainMovementComponent.h"
#include "CoreMinimal.h"
#include "HPGliaClient.h"
#include "HeadMountedDisplay.h"
#include "Containers/Array.h" 
#include "Kismet/GameplayStatics.h" 
#include "GameFramework/FloatingPawnMovement.h" 
#include "GameFramework/CharacterMovementComponent.h" // test 
//#include "Runtime/Steam/SteamVR/Source/SteamVR/Classes/SteamVRChaperoneComponent.h"
#include "MotionControllerComponent.h"
#include "GameModeMain.h"
#include "PawnMain.generated.h"

UCLASS()
class EYETRACKINGVR_API APawnMain : public APawn
{
	GENERATED_BODY()

public:

	// Sets default values for this pawn's properties
	APawnMain();
	//virtual void SetupPlayerInputComponent(class UInputComponent* InInputComponent) override;

	UPROPERTY() UCharacterMovementComponent* OurMovementComponentChar;

	virtual UPawnMovementComponent* GetMovementComponent() const override;
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void LookUp(float AxisValue);
	void QuitGame();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	AGameModeMain* GameMode = nullptr; 

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/* === properties === */
	//UPROPERTY(VisibleDefaultsOnly, meta = (Category = "Default"))
	class UCapsuleComponent* CollisionCylinder;
	class USkeletalMeshComponent* CharacterMeshComponent;
	class UCharacterMovementComponent* CharacterMoveComponent;
	class UCameraComponent* Camera;
	class UArrowComponent* Arrow;
	class USkeletalMeshComponent* CharacterMesh;
	class UCharacterMovementComponent* CharMoveComp;
	//UPROPERTY(EditAnywhere, Category = "MotionController")
	class UMotionControllerComponent* MotionControllerLeft;
	class UMotionControllerComponent* MotionControllerLeftAim;
	//UPROPERTY(EditAnywhere, Category = "MotionController")
	class UMotionControllerComponent* MotionControllerRight;
	class UMotionControllerComponent* MotionControllerRightAim;
	class UWidgetInteractionComponent* WidgetInteractionLeft;
	class UWidgetInteractionComponent* WidgetInteractionRight;
	class UWidgetInteractionComponent* WidgetInteraction;
	
	
	void SetupPlayerInputComponent(class UInputComponent* InInputComponent);

	/* overlap events */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// to store overlap characters
	UPrimitiveComponent* OverlappedComponent;

	/* Movement Component */
	FHitResult OutHit;
	ETeleportType TeleportType = ETeleportType::None;
	void UpdateMovementComponent(FVector InputVector, bool bForce);

};
