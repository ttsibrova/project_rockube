// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "RockubeMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FDashStartDelegate);

UENUM (BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None			UMETA (Hidden),
	CMOVE_Dash			UMETA (DisplayName = "Dash"),
	CMOVE_MAX			UMETA (Hidden)
};

class APlayerRockubeCharacter;
/**
 * 
 */
UCLASS()
class ROCKUBE_API URockubeMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	UPROPERTY(Transient) APlayerRockubeCharacter* RockubePlayerOwner;

	//Parameters
	//Dash
	UPROPERTY (EditDefaultsOnly) UAnimMontage* DashMontage;
	UPROPERTY (EditDefaultsOnly) float DashImpulseOnGround = 6000.f;
	UPROPERTY (EditDefaultsOnly) float DashImpulseInAir = 2000.f;
	UPROPERTY (EditDefaultsOnly) float DashGravityForce = 2000.f;
	UPROPERTY (EditDefaultsOnly) float BrakingDecelerationDash = 1000.f;



	//flags
	bool Safe_bWantsToDash = false;
	bool bAllowedToDash = true;
	bool Safe_bHadAnimRootMotion;
	float DashBlockTime = 0.4f;
	float DashStartTime;

	//Delegates
public:
	UPROPERTY (BlueprintAssignable) FDashStartDelegate DashStartDelegate;

public:
	URockubeMovementComponent() {};

protected:
	virtual void InitializeComponent();


private:


public:
	virtual void UpdateCharacterStateBeforeMovement (float DelatSeconds) override;
	virtual void UpdateCharacterStateAfterMovement (float DeltaSeconds) override;

protected:
	virtual void OnMovementUpdated (float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void PhysCustom (float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged (EMovementMode PreviousMode, uint8 PreviousCustomMode) override;

	// Dash
private:
	void EnterDash (EMovementMode PrevMode, ECustomMovementMode PreviousCustomMode);
	void ExitDash();
	bool CanDash() const;
	void PhysDash (float deltaTime);
	void PerformDash ();

public:
	UFUNCTION(BlueprintCallable) void DashPressed();
	UFUNCTION(BlueprintCallable) void DashReleased();
};
