// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement/TrajectoryProviderComponent.h"

// Sets default values for this component's properties
UTrajectoryProviderComponent::UTrajectoryProviderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
    TrajectoryType = ETrajectoryType::TRAJECTORY_SINGLE;
    StartOffset = FVector (0., 0., -20.f);
    TargetLocation = FVector (0., 0., 100.f);
    TrajectoryDistance = 500.f;
	// ...
}

float UTrajectoryProviderComponent::GetDistanceToTarget() const
{
    float Distance = 0.f;
    Distance = std::sqrt ((GetTrajectoryEndLocation() - GetTrajectoryStartLocation()).SquaredLength());
    return Distance;
}

FVector UTrajectoryProviderComponent::GetDirectionToTarget() const
{
    FVector Direction;
    Direction = GetTrajectoryEndLocation() - GetTrajectoryStartLocation();
    Direction.Normalize();
    return Direction;
}

FVector UTrajectoryProviderComponent::GetTrajectoryStartLocation() const
{
    return GetOwner()->GetActorLocation() + StartOffset;
}

FVector UTrajectoryProviderComponent::GetTrajectoryEndLocation() const
{
    switch (TrajectoryType)
    {
    case ETrajectoryType::TRAJECTORY_SINGLE:
    case ETrajectoryType::TRAJECTORY_FIXED:
        return GetOwner()->GetActorLocation() + TargetLocation;
        break;
    case ETrajectoryType::TRAJECTORY_CHAIN:
        if (ChainActor) {
            return ChainActor->GetActorLocation();
        }
        else
            UE_LOG (LogTemp, Warning, TEXT ("Chain trajectory end requested, but no chain actor initialized"));
        break;
    default:
        UE_LOG (LogTemp, Warning, TEXT ("Impossiburu: trajectory provider"));
        break;
    }
    return FVector();
}

void UTrajectoryProviderComponent::SetTargetLocation (FVector UpdatedTargetLocation)
{
    switch (TrajectoryType)
    {
    case ETrajectoryType::TRAJECTORY_SINGLE:
        TargetLocation = UpdatedTargetLocation;
        break;
    case ETrajectoryType::TRAJECTORY_FIXED:
        FVector DistVect = (GetOwner ()->GetActorLocation() + UpdatedTargetLocation) - GetTrajectoryStartLocation();
        DistVect.Normalize();
        DistVect *= TrajectoryDistance;
        TargetLocation = DistVect;
        break;
    }
}

void UTrajectoryProviderComponent::SetTrajectoryDistance (float UpdatedTrajectoryDistance)
{
    TrajectoryDistance = UpdatedTrajectoryDistance;
    FVector DistVect = (GetOwner()->GetActorLocation() + TargetLocation) - GetTrajectoryStartLocation();
    DistVect.Normalize();
    DistVect *= TrajectoryDistance;
    TargetLocation = DistVect;
}

