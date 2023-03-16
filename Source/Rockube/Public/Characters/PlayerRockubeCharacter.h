// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/RockubeBaseCharacter.h"
#include "InputActionValue.h"
#include "PlayerRockubeCharacter.generated.h"

class UCameraComponent;
class URockubeMovementComponent;

UCLASS()
class ROCKUBE_API APlayerRockubeCharacter : public ARockubeBaseCharacter
{
	GENERATED_BODY()

public:
	APlayerRockubeCharacter (const FObjectInitializer& ObjectInitializer);

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	//virtual void Tick (float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent (class UInputComponent* PlayerInputComponent) override;


protected:
	/** MappingContext */
	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
		class UInputAction* InteractAction;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
		class UInputAction* DashAction;

	//Camera
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class USpringArmComponent* SpringArmComponent;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* CameraComponent;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		URockubeMovementComponent* RockubeMovmentComponent;


protected:
	void Move (const FInputActionValue& Value);
	void Look (const FInputActionValue& Value);
	void Interact (const FInputActionValue& Value);
	void Dash (const FInputActionValue& Value);
	void DashReleased (const FInputActionValue& Value);
};
