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
    if (bWantsToDash && CanDash()) {
        SetMovementMode (MOVE_Custom, CMOVE_Dash);
        bWantsToDash = false;
    }

    UCharacterMovementComponent::UpdateCharacterStateBeforeMovement (DeltaSeconds);
}

void URockubeMovementComponent::UpdateCharacterStateAfterMovement (float DeltaSeconds)
{

    UCharacterMovementComponent::UpdateCharacterStateAfterMovement (DeltaSeconds);
}


void URockubeMovementComponent::OnMovementModeChanged (EMovementMode PreviousMode, uint8 PreviousCustomMode)
{
    UCharacterMovementComponent::OnMovementModeChanged (PreviousMode, PreviousCustomMode);

    if (PreviousMode == MOVE_Custom && PreviousCustomMode == CMOVE_Dash) ExitDash();

    if (IsCustomMovementMode (CMOVE_Dash)) EnterDash (PreviousMode, static_cast<ECustomMovementMode>(PreviousCustomMode));


}


void URockubeMovementComponent::PhysCustom (float deltaTime, int32 Iterations)
{
    UCharacterMovementComponent::PhysCustom (deltaTime, Iterations);

    switch (CustomMovementMode)
    {
    case CMOVE_Dash:
        PhysDash (deltaTime, Iterations);
        break;
    default:
        UE_LOG (LogTemp, Fatal, TEXT ("Invalid Movement Mode"));
    }
}


void URockubeMovementComponent::EnterDash (EMovementMode PrevMode, ECustomMovementMode PreviousCustomMode)
{
    bAirDash = PrevMode == MOVE_Falling;
    bOrientRotationToMovement = false;
    DashStartLocationPos = UpdatedComponent->GetComponentLocation();
    DashStartTime = GetWorld()->GetTimeSeconds();
    if (!bAirDash) {
        FindFloor (DashStartLocationPos, CurrentFloor, true, NULL);
    }
    if (DashSimulationTime < 0.f) {
        DashSimulationTime = RockubePlayerOwner->GetBeatSyncComp()->GetBeatTime() * DashSimulationRelativeTime;
    }
}

void URockubeMovementComponent::ExitDash()
{
    bOrientRotationToMovement = true;
    auto aNewVelocity = UpdatedComponent->GetForwardVector();
    aNewVelocity.Normalize();
    Velocity += FVector::ZeroVector;
    SetMovementMode (MOVE_Falling);
    float DistancePassed = std::sqrt ((DashStartLocationPos - UpdatedComponent->GetComponentLocation()).SquaredLength());
    FString VelocityStat = TEXT ("Distance passed: ") + FString::SanitizeFloat (DistancePassed);
    GEngine->AddOnScreenDebugMessage (-1, 2.0f, FColor::Orange, VelocityStat);
}

bool URockubeMovementComponent::CanDash() const
{
    return IsWalking() && !IsCrouching() || IsFalling();
}


float URockubeMovementComponent::ComputeDecelleratedSpeed (float RemainingDashDistance, float RemainingDashTime)
{
    float CurrentTickDecelleration = (DashLastVelocity * RemainingDashTime - RemainingDashDistance) / std::pow (RemainingDashTime, 2);
    return DashLastVelocity + CurrentTickDecelleration;
}

void URockubeMovementComponent::PhysDash (float deltaTime, int32 Iterations)
{

    auto DashDirection = UpdatedComponent->GetForwardVector();
    DashDirection.Normalize();

    float RemainingTime = deltaTime;
    while ((RemainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController))
    {
        //CMC
        const float timeTick = GetSimulationTimeStep (RemainingTime, Iterations);
        RemainingTime -= timeTick;
        Iterations++;

        //Simulation
        FVector CurrentPosition = UpdatedComponent->GetComponentLocation();
        float RemainingDashTime = DashSimulationTime - (GetWorld()->GetTimeSeconds() - DashStartTime);
        float DistancePassed = std::sqrt ((DashStartLocationPos - CurrentPosition).SquaredLength());
        float RemainingDashDistance = DashTargetDistance - DistancePassed;
        float T0 = (GetWorld()->GetTimeSeconds() - DashStartTime) / DashSimulationTime;
        float T1 = T0 + timeTick / DashSimulationTime;

        float S0 = DashMovementCurve->GetFloatValue (T0) * DashTargetDistance;
        if (std::abs (DistancePassed - S0) > 1.) {
            FString DistDiff = TEXT ("WARNING! Actual and expected distances are different: ") + FString::SanitizeFloat (DistancePassed - S0);
            GEngine->AddOnScreenDebugMessage (-1, 2.0f, FColor::Red, DistDiff);
        } else {
            S0 = DistancePassed;
        }

        if (RemainingDashTime < MIN_TICK_TIME) {
            RemainingTime = 0.f;
            ExitDash();
            continue;
        }
        if (RemainingDashTime < timeTick) {
            RemainingDashTime = timeTick;
        }

        // Computing speed
        float S1 = DashMovementCurve->GetFloatValue (T1) * DashTargetDistance;
        float SpeedCoeff = (S1 - S0) / timeTick;
        FVector Speed = DashDirection * SpeedCoeff;

        
        FString VelocityStat = TEXT ("Velocity - X: ") + FString::SanitizeFloat (Speed.X)
                             + TEXT (" Y: ")           + FString::SanitizeFloat (Speed.Y)
                             + TEXT (" Z: ")           + FString::SanitizeFloat (Speed.Z);
        if (GEngine)
            GEngine->AddOnScreenDebugMessage (-1, 2.0f, FColor::Red, VelocityStat);
        if (bAirDash) {
            FHitResult Hit;
            FQuat Rotation = UpdatedComponent->GetComponentRotation().Quaternion();
            FVector Adjusted = Speed * timeTick;
            SafeMoveUpdatedComponent (Adjusted, Rotation, true, Hit);

        } else {
            FStepDownResult StepDownResult;
            MoveAlongFloor (Speed, timeTick, &StepDownResult);
            FVector UpdatedPosition = UpdatedComponent->GetComponentLocation();
            if (IsFalling()) {
                StartNewPhysics (RemainingTime, Iterations);
            }
        }
    }

}

void URockubeMovementComponent::OnDashCooldown()
{
    bAllowedToDash = true;
}

// JumpPad jump

void URockubeMovementComponent::EnterJumpPadJump (EMovementMode PrevMode, ECustomMovementMode PreviousCustomMode)
{
    bAirDash = PrevMode == MOVE_Falling;
    bOrientRotationToMovement = false;
    JumpPadJumpStartLocationPos = UpdatedComponent->GetComponentLocation();
    JumpPadJumpStartTime = GetWorld()->GetTimeSeconds();
    if (JumpPadJumpSimulationTime < 0.f) {
        JumpPadJumpSimulationTime = RockubePlayerOwner->GetBeatSyncComp()->GetBeatTime() * DashSimulationRelativeTime;
    }
    
}

void URockubeMovementComponent::ExitJumpPadJump()
{
    bOrientRotationToMovement = true;
    auto aNewVelocity = UpdatedComponent->GetForwardVector();
    aNewVelocity.Normalize();
    Velocity += FVector::ZeroVector;
    SetMovementMode (MOVE_Falling);
    float DistancePassed = std::sqrt ((DashStartLocationPos - UpdatedComponent->GetComponentLocation ()).SquaredLength ());
}

bool URockubeMovementComponent::CanJumpPadJump() const
{
    return IsWalking() && !IsCrouching() || IsFalling();
}

void URockubeMovementComponent::PhysJumpPadJump (float deltaTime, int32 Iterations)
{

    auto DashDirection = UpdatedComponent->GetForwardVector();
    DashDirection.Normalize();

    float RemainingTime = deltaTime;
    while ((RemainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController))
    {
        //CMC
        const float timeTick = GetSimulationTimeStep (RemainingTime, Iterations);
        RemainingTime -= timeTick;
        Iterations++;

        //Simulation
        FVector CurrentPosition = UpdatedComponent->GetComponentLocation();
        float RemainingDashTime = DashSimulationTime - (GetWorld()->GetTimeSeconds() - DashStartTime);
        float DistancePassed = std::sqrt ((DashStartLocationPos - CurrentPosition).SquaredLength());
        float RemainingDashDistance = DashTargetDistance - DistancePassed;
        float T0 = (GetWorld()->GetTimeSeconds() - DashStartTime) / DashSimulationTime;
        float T1 = T0 + timeTick / DashSimulationTime;

        float S0 = DashMovementCurve->GetFloatValue (T0) * DashTargetDistance;
        if (std::abs (DistancePassed - S0) > 1.) {
            FString DistDiff = TEXT ("WARNING! Actual and expected distances are different: ") + FString::SanitizeFloat (DistancePassed - S0);
            GEngine->AddOnScreenDebugMessage (-1, 2.0f, FColor::Red, DistDiff);
        } else {
            S0 = DistancePassed;
        }

        if (RemainingDashTime < MIN_TICK_TIME) {
            RemainingTime = 0.f;
            ExitDash();
            continue;
        }
        if (RemainingDashTime < timeTick) {
            RemainingDashTime = timeTick;
        }

        // Computing speed
        float S1 = DashMovementCurve->GetFloatValue (T1) * DashTargetDistance;
        float SpeedCoeff = (S1 - S0) / timeTick;
        FVector Speed = DashDirection * SpeedCoeff;

        
        FString VelocityStat = TEXT ("Velocity - X: ") + FString::SanitizeFloat (Speed.X)
                             + TEXT (" Y: ")           + FString::SanitizeFloat (Speed.Y)
                             + TEXT (" Z: ")           + FString::SanitizeFloat (Speed.Z);
        if (GEngine)
            GEngine->AddOnScreenDebugMessage (-1, 2.0f, FColor::Red, VelocityStat);
        if (bAirDash) {
            FHitResult Hit;
            FQuat Rotation = UpdatedComponent->GetComponentRotation().Quaternion();
            FVector Adjusted = Speed * timeTick;
            SafeMoveUpdatedComponent (Adjusted, Rotation, true, Hit);

        } else {
            FStepDownResult StepDownResult;
            MoveAlongFloor (Speed, timeTick, &StepDownResult);
            FVector UpdatedPosition = UpdatedComponent->GetComponentLocation();
            if (IsFalling()) {
                StartNewPhysics (RemainingTime, Iterations);
            }
        }
    }

}


//Inputs
void URockubeMovementComponent::DashPressed() 
{
    FString Time = FString::SanitizeFloat (GetWorld()->GetTimeSeconds());
    GEngine->AddOnScreenDebugMessage (-1, 10.0f, FColor::Black, Time);
    if (bAllowedToDash) {
        bWantsToDash = true;
        bAllowedToDash = false;
    } else {
        return;
    }
    double PerfTolerance = 0.1;
    double GoodTolerance = 0.2;
    double DeltaError;
    bool bIsPerfect;
    if (RockubePlayerOwner->GetBeatSyncComp()->GetBPMCounter()->IsOnBeat(GoodTolerance, PerfTolerance, bIsPerfect, DeltaError)) {
        bLastDashInBeat = true;
        DashOnBeatDelegate.Broadcast();
    } else {
        bLastDashInBeat = false;
    }
}

void URockubeMovementComponent::DashReleased()
{
    bWantsToDash = false;
    if (bLastDashInBeat) {
        bAllowedToDash = true;
    } else {
        bAllowedToDash = false;
        float TimeCooldown = RockubePlayerOwner->GetBeatSyncComp()->GetBeatTime() * 2;
        GetWorld()->GetTimerManager().SetTimer (DashCooldownTimer,this, &URockubeMovementComponent::OnDashCooldown, TimeCooldown, false);
    }
}

void URockubeMovementComponent::JumpPressed()
{
    if (!bJumpButtonPressed) {
        CharacterOwner->Jump();
    }
    bJumpButtonPressed = true;
}

void URockubeMovementComponent::JumpReleased()
{
    CharacterOwner->StopJumping();
    bJumpButtonPressed = false;
}

bool URockubeMovementComponent::IsCustomMovementMode (ECustomMovementMode InCustomMovementMode) const
{
    return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

bool URockubeMovementComponent::IsMovementMode (EMovementMode InMovementMode) const
{
    return InMovementMode == MovementMode;
}

