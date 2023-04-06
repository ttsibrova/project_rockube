// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RockubeMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FDashOnBeatDelegate);

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
	UPROPERTY (EditDefaultsOnly) float DashImpulseOnGround = 2000.f;
	UPROPERTY (EditDefaultsOnly) float DashImpulseInAir = 2000.f;
	UPROPERTY (EditDefaultsOnly) float DashGravityForce = 2000.f;
	UPROPERTY (EditDefaultsOnly) float DashFriction = 5.f;
	UPROPERTY (EditDefaultsOnly) float BrakingDecelerationDash = 1000.f;
	UPROPERTY (EditDefaultsOnly) float DashExitSpeed = 400.f;



	//flags
	bool bWantsToDash = false;
	bool bHadAnimRootMotion;

	//Dash
	bool bAllowedToDash = true;
	float DashBlockTime = 0.4f;
	FTimerHandle DashCooldownTimer;
	bool bAirDash = false;
	bool bLastDashInBeat = false;
	bool bDashButtonPressed = false;
	//Dash New Phys
	FVector DashStartLocationPos;
	float DashStartTime;
	FVector InitialDashVelocity;
	UPROPERTY (EditDefaultsOnly) float DashSimulationRelativeTime = 0.9;
	UPROPERTY (EditDefaultsOnly) float DashTargetDistance = 2000;
	float DashSimulationTime = -1.f;
	float DashLastVelocity;


	//Jump
	bool bJumpButtonPressed = false;



	//Delegates
public:
	UPROPERTY (BlueprintAssignable) FDashOnBeatDelegate DashOnBeatDelegate;

public:
	URockubeMovementComponent() {};

protected:
	virtual void InitializeComponent();


private:


public:
	virtual void UpdateCharacterStateBeforeMovement (float DelatSeconds) override;
	virtual void UpdateCharacterStateAfterMovement (float DeltaSeconds) override;

protected:
	//virtual void OnMovementUpdated (float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void PhysCustom (float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged (EMovementMode PreviousMode, uint8 PreviousCustomMode) override;

	// Dash
private:
	void EnterDash (EMovementMode PrevMode, ECustomMovementMode PreviousCustomMode);
	void NewEnterDash (EMovementMode PrevMode, ECustomMovementMode PreviousCustomMode);
	void ExitDash();
	bool CanDash() const;
	void PhysDash (float deltaTime, int32 Iterations);
	void NewPhysDash (float deltaTime, int32 Iterations);
	void OnDashCooldown();

	//helpers
private:
	float ComputeDecelleratedSpeed (float RemainingDashDistance, float RemainingDashTime);

public:
	UFUNCTION (BlueprintCallable) void DashPressed();
	UFUNCTION (BlueprintCallable) void DashReleased();

	UFUNCTION (BlueprintCallable) void JumpPressed();
	UFUNCTION (BlueprintCallable) void JumpReleased();

	UFUNCTION (BlueprintPure) bool IsCustomMovementMode (ECustomMovementMode InCustomMovementMode) const;
	UFUNCTION (BlueprintPure) bool IsMovementMode (EMovementMode InMovementMode) const;
	UFUNCTION (BlueprintPure) bool IsDashing() const { return IsCustomMovementMode (CMOVE_Dash); }

};
