// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "RockubeMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FDashStartDelegate);

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
	UPROPERTY (EditDefaultsOnly) float DashImpulse = 2000.f;



	//flags
	bool Safe_bWantsToDash = false;
	bool bAllowedToDash;

	bool Safe_bHadAnimRootMotion;

	//Delegates
public:
	UPROPERTY (BluprintAssignable) FDashStartDelegate DashStartDelegate;

public:
	URockubeMovementComponent() {};

protected:
	virtual void InitializeComponent();


private:
	bool CanDash() const;
	void PerformDash();

public:
	virtual void UpdateCharacterStateBeforeMovement (float DelatSeconds) override;
	virtual void UpdateCharacterStateAfterMovement (float DeltaSeconds) override;

protected:
	virtual void OnMovementUpdated (float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

public:
	UFUNCTION(BlueprintCallable) void DashPressed();
	UFUNCTION(BlueprintCallable) void DashReleased();
};
