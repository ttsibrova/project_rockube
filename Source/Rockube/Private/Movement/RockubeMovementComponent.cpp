// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement/RockubeMovementComponent.h"

#include "Characters/PlayerRockubeCharacter.h"


void URockubeMovementComponent::InitializeComponent()
{
    UCharacterMovementComponent::InitializeComponent();
    RockubePlayerOwner = Cast<APlayerRockubeCharacter> (GetOwner());
}

void URockubeMovementComponent::UpdateCharacterStateBeforeMovement (float DeltaSeconds)
{
    if (Safe_bWantsToDash && CanDash()) {
        PerformDash();
        Safe_bWantsToDash = false;
    }

    UCharacterMovementComponent::UpdateCharacterStateBeforeMovement (DeltaSeconds);
}

void URockubeMovementComponent::UpdateCharacterStateAfterMovement (float DeltaSeconds)
{

    Super::UpdateCharacterStateAfterMovement (DeltaSeconds);

//    if (!HasAnimRootMotion() && Safe_bHadAnimRootMotion && IsFlying())
//    {
//        UE_LOG (LogTemp, Warning, TEXT ("Ending Anim Root Motion"))
//        SetMovementMode (MOVE_Walking);
//    }
//    Safe_bHadAnimRootMotion = HasAnimRootMotion();
}

void URockubeMovementComponent::OnMovementUpdated (float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
    Super::OnMovementUpdated (DeltaSeconds, OldLocation, OldVelocity);

    if (IsFlying() && !HasRootMotionSources()) {
        SetMovementMode(MOVE_Walking);
    }
}

bool URockubeMovementComponent::CanDash() const
{
    return IsWalking() && !IsCrouching() || IsFalling();
}

void URockubeMovementComponent::PerformDash()
{
    DashStartTime = GetWorld()->GetTimeSeconds();
    FVector DashDirection = (Acceleration.IsNearlyZero() ? UpdatedComponent->GetForwardVector() : Acceleration).GetSafeNormal2D();
    //DashDirection += FVector::UpVector * 0.1f;
    if (IsFalling()) {
        Velocity = DashImpulseInAir * DashDirection;
    } else {
        Velocity = DashImpulseOnGround * DashDirection;
    }

    FQuat NewRotation = FRotationMatrix::MakeFromXZ (DashDirection, FVector::UpVector).ToQuat();
    FHitResult Hit;
    SafeMoveUpdatedComponent (FVector::ZeroVector, NewRotation, false, Hit);
    SetMovementMode (MOVE_Falling);

    DashStartDelegate.Broadcast();
}

//Inputs
void URockubeMovementComponent::DashPressed() 
{
    if (bAllowedToDash) {
        Safe_bWantsToDash = true;
        bAllowedToDash = false;
    } else {
        float DeltaTime = GetWorld()->GetTimeSeconds() - DashStartTime;
        if (DeltaTime >= DashBlockTime) {
            Safe_bWantsToDash = true;
        }
    }
    //Dash cooldown check
    //timer start?
}

void URockubeMovementComponent::DashReleased()
{
    //clearing timer
    if (GEngine)
    	GEngine->AddOnScreenDebugMessage (-1, 2.0f, FColor::Red, TEXT ("Dash release registered"));
    Safe_bWantsToDash = false;
}

