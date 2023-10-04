// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RockubeJumpPad.generated.h"

class UTrajectoryProviderComponent;
class UCapsuleComponent;

UCLASS()
class ROCKUBE_API ARockubeJumpPad : public AActor
{
    GENERATED_BODY()
    
public:    
    // Sets default values for this actor's properties
    ARockubeJumpPad();

    void OnOverlapBegin (UPrimitiveComponent* OverlappedComponent,
                         AActor* OtherActor,
                         UPrimitiveComponent* OtherComp,
                         int32 OtherBodyIndex,
                         bool bFromSweep,
                         const FHitResult& SweepResult);

    //const TObjectPtr<UTrajectoryProviderComponent>& GetTrajectoryProvider() { return TrajectoryProvider; };

protected:
    UPROPERTY(EditAnywhere) TObjectPtr<UTrajectoryProviderComponent> TrajectoryProvider;
    UPROPERTY(BluePrintReadWrite) TObjectPtr<UStaticMeshComponent>   JumpPadMesh;
    UPROPERTY(BluePrintReadWrite) TObjectPtr<UCapsuleComponent>      JumpPadCollider;


public:

};
