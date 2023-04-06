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

//    if (!HasAnimRootMotion() && bHadAnimRootMotion && IsFlying())
//    {
//        UE_LOG (LogTemp, Warning, TEXT ("Ending Anim Root Motion"))
//        SetMovementMode (MOVE_Walking);
//    }
//    bHadAnimRootMotion = HasAnimRootMotion();
}


void URockubeMovementComponent::OnMovementModeChanged (EMovementMode PreviousMode, uint8 PreviousCustomMode)
{
    UCharacterMovementComponent::OnMovementModeChanged (PreviousMode, PreviousCustomMode);

    if (PreviousMode == MOVE_Custom && PreviousCustomMode == CMOVE_Dash) ExitDash();

    if (IsCustomMovementMode (CMOVE_Dash)) NewEnterDash (PreviousMode, static_cast<ECustomMovementMode>(PreviousCustomMode));


}


void URockubeMovementComponent::PhysCustom (float deltaTime, int32 Iterations)
{
    UCharacterMovementComponent::PhysCustom (deltaTime, Iterations);

    switch (CustomMovementMode)
    {
    case CMOVE_Dash:
        NewPhysDash (deltaTime, Iterations);
        break;
    default:
        UE_LOG (LogTemp, Fatal, TEXT ("Invalid Movement Mode"));
    }
}





void URockubeMovementComponent::EnterDash (EMovementMode PrevMode, ECustomMovementMode PreviousCustomMode)
{
    bAirDash = PrevMode == MOVE_Falling;
    bOrientRotationToMovement = false;
    auto aNewVelocity = UpdatedComponent->GetForwardVector();
    aNewVelocity.Normalize();
    Velocity += aNewVelocity.GetSafeNormal2D() * DashImpulseOnGround;
    InitialDashVelocity = Velocity;
    if (!bAirDash) {
        FindFloor (UpdatedComponent->GetComponentLocation(), CurrentFloor, true, NULL);
    }
}

void URockubeMovementComponent::NewEnterDash (EMovementMode PrevMode, ECustomMovementMode PreviousCustomMode)
{
    bAirDash = PrevMode == MOVE_Falling;
    bOrientRotationToMovement = false;
    DashStartLocationPos = UpdatedComponent->GetComponentLocation ();
    DashStartTime = GetWorld ()->GetTimeSeconds ();
    if (!bAirDash) {
        FindFloor (DashStartLocationPos, CurrentFloor, true, NULL);
    }
    if (DashSimulationTime < 0.f) {
        DashSimulationTime = RockubePlayerOwner->GetBeatSyncComp ()->GetBeatTime () * DashSimulationRelativeTime;
    }
}

void URockubeMovementComponent::ExitDash()
{
    bOrientRotationToMovement = true;
    auto aNewVelocity = UpdatedComponent->GetForwardVector ();
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



void URockubeMovementComponent::PhysDash (float deltaTime, int32 Iterations)
{
    if (deltaTime < MIN_TICK_TIME) {
        return;
    }

    //if (!CanDash()) {
    //    SetMovementMode (MOVE_Falling);
    //    StartNewPhysics (deltaTime, Iterations);
    //    return;
    //}


    // Save current values
    const FVector OldLocation = UpdatedComponent->GetComponentLocation();
    FFindFloorResult OldFloor = CurrentFloor;
    if (!bAirDash) {
        MaintainHorizontalGroundVelocity();
    }
    Velocity = InitialDashVelocity;

    //CalcVelocity (deltaTime, DashFriction, false, BrakingDecelerationDash);
    FVector CalcVelocity = InitialDashVelocity;
    InitialDashVelocity = Velocity;
    //ApplyRootMotionToVelocity (deltaTime);


    // Perform move
    Iterations++;
    bJustTeleported = false;

    if (bAirDash) {
        CalcVelocity /= (DashFriction + 1.);
        FHitResult Hit (0.4f);
        FVector Adjusted = CalcVelocity * deltaTime;
        FString VelocityStat = TEXT ("Velocity - X: ") + FString::SanitizeFloat (CalcVelocity.X) + TEXT (" Y: ") + FString::SanitizeFloat (CalcVelocity.Y) + TEXT(" Z: ") + FString::SanitizeFloat (CalcVelocity.Z);
        //if (GEngine)
            //GEngine->AddOnScreenDebugMessage (-1, 2.0f, FColor::Red, VelocityStat);
        FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
        SafeMoveUpdatedComponent (Adjusted, OldRotation, true, Hit);

        if (Hit.Time < .4f) {
            HandleImpact (Hit, deltaTime, Adjusted);
            SlideAlongSurface (Adjusted, (0.4f - Hit.Time), Hit.Normal, Hit, true);
        }
    } else {
        CalcVelocity /= DashFriction;
        FStepDownResult StepDownResult;
        const float timeTick = GetSimulationTimeStep (deltaTime, Iterations);
        FString VelocityStat = TEXT ("Velocity - X: ") + FString::SanitizeFloat (Velocity.X) + TEXT (" Y: ") + FString::SanitizeFloat (Velocity.Y) + TEXT (" Z: ") + FString::SanitizeFloat (Velocity.Z);
        //if (GEngine)
            //GEngine->AddOnScreenDebugMessage (-1, 2.0f, FColor::Red, VelocityStat);
        MoveAlongFloor (CalcVelocity, timeTick, &StepDownResult);
        if (IsFalling()) {
            StartNewPhysics (deltaTime, Iterations);
        }
    }

    if (CalcVelocity.SizeSquared() < std::pow (DashExitSpeed, 2)) {
        ExitDash();
    }
    else if (bAirDash && (CalcVelocity.SizeSquared() < std::pow (DashExitSpeed * 2, 2))) {
        ExitDash();
    }
    InitialDashVelocity = CalcVelocity;
}

float URockubeMovementComponent::ComputeDecelleratedSpeed (float RemainingDashDistance, float RemainingDashTime)
{
    float CurrentTickDecelleration = (DashLastVelocity * RemainingDashTime - RemainingDashDistance) / std::pow (RemainingDashTime, 2);
    return DashLastVelocity + CurrentTickDecelleration;
}

void URockubeMovementComponent::NewPhysDash (float deltaTime, int32 Iterations)
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
        float RemainingDashDistance = DashTargetDistance - std::sqrt ((DashStartLocationPos - CurrentPosition).SquaredLength());

        if (RemainingDashTime < MIN_TICK_TIME) {
            RemainingTime = 0.f;
            ExitDash();
            continue;
        }
        float CurrentTickVelocity = 0.;
        //Dash Stages
        if (RemainingDashDistance > 0.2 * DashTargetDistance) {
            //Stage 1: Full speed
            CurrentTickVelocity = 4 * DashTargetDistance / DashSimulationTime;
        }
        if (RemainingDashDistance <= 0.2 * DashTargetDistance && RemainingDashDistance > 0.05 * DashTargetDistance) {
            RemainingDashTime = RemainingDashTime > timeTick ? RemainingDashTime : timeTick;
            float CurrentTickDecelleration = ((std::pow (DashLastVelocity, 1.367)) * RemainingDashTime - RemainingDashDistance) / std::pow (RemainingDashTime, 2);
            GEngine->AddOnScreenDebugMessage (-1, 2.0f, FColor::Blue, FString::SanitizeFloat (CurrentTickDecelleration));
            CurrentTickVelocity = DashLastVelocity - CurrentTickDecelleration * timeTick;
            if (CurrentTickVelocity < 0) {
                CurrentTickVelocity = 0.f;
            }
        }
        if (RemainingDashDistance <= 0.05 * DashTargetDistance) {
            RemainingDashTime = RemainingDashTime > timeTick ? RemainingDashTime : timeTick;
            CurrentTickVelocity = RemainingDashDistance / RemainingDashTime;
        }

        FVector Speed = DashDirection * CurrentTickVelocity;
        DashLastVelocity = CurrentTickVelocity;
        FString VelocityStat = TEXT ("Velocity - X: ") + FString::SanitizeFloat (Speed.X) + TEXT (" Y: ") + FString::SanitizeFloat (Speed.Y) + TEXT (" Z: ") + FString::SanitizeFloat (Speed.Z);
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

