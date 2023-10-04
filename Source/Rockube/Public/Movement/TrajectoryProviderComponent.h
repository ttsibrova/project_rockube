// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrajectoryProviderComponent.generated.h"

UENUM (BlueprintType)
enum class ETrajectoryType: uint8
{
    TRAJECTORY_None            UMETA (Hidden),
    TRAJECTORY_SINGLE          UMETA (DisplayName = "Single"),
    TRAJECTORY_CHAIN           UMETA (DisplayName = "Chain"),
    TRAJECTORY_FIXED           UMETA (DisplayName = "Fixed"),
    TRAJECTORY_MAX             UMETA (Hidden)
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROCKUBE_API UTrajectoryProviderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTrajectoryProviderComponent();

    UFUNCTION (BlueprintCallable) float GetDistanceToTarget() const;
    UFUNCTION (BlueprintCallable) FVector GetDirectionToTarget() const;

    FVector GetTrajectoryStartLocation() const;
    FVector GetTrajectoryEndLocation() const;

protected:
    UFUNCTION (CallInEditor)
    void SetTargetLocation (FVector UpdatedTargetLocation);
    UFUNCTION (CallInEditor)
    void SetTrajectoryDistance (float UpdatedTrajectoryDistance);


    UPROPERTY (EditAnywhere)
    ETrajectoryType TrajectoryType;

    UPROPERTY (EditAnywhere, meta = (EditCondition = "TrajectoryType == ETrajectoryType::TRAJECTORY_CHAIN"))
    AActor* ChainActor;

    UPROPERTY (EditAnywhere, Interp, meta = (EditCondition = "TrajectoryType == ETrajectoryType::TRAJECTORY_SINGLE || TrajectoryType == ETrajectoryType::TRAJECTORY_FIXED"))
    FVector TargetLocation;

    UPROPERTY (EditAnywhere)
    FVector StartOffset;

    UPROPERTY (EditAnywhere, Interp, meta = (EditCondition = "TrajectoryType == ETrajectoryType::TRAJECTORY_FIXED"))
    float TrajectoryDistance;

    friend class FTrajectoryProviderComponentVisualizer;

};
