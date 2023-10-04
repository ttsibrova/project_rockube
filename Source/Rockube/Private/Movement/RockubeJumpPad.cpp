// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement/RockubeJumpPad.h"

#include "Characters/PlayerRockubeCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Movement/RockubeMovementComponent.h"
#include "Movement/TrajectoryProviderComponent.h"

// Sets default values
ARockubeJumpPad::ARockubeJumpPad()
{
    PrimaryActorTick.bCanEverTick = false;

    TrajectoryProvider = CreateDefaultSubobject <UTrajectoryProviderComponent> (TEXT("Trajectory Provider Component"));

    JumpPadCollider = CreateDefaultSubobject <UCapsuleComponent> (TEXT("JumpPad Collider"));
    JumpPadCollider->SetupAttachment (RootComponent);

    JumpPadMesh = CreateDefaultSubobject <UStaticMeshComponent> ("JumpPad Mesh");
    JumpPadMesh->SetupAttachment (JumpPadCollider);
}

void ARockubeJumpPad::OnOverlapBegin (UPrimitiveComponent* OverlappedComponent,
                                      AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex,
                                      bool bFromSweep,
                                      const FHitResult& SweepResult)
{
    APlayerRockubeCharacter* aPlayer = Cast <APlayerRockubeCharacter> (OtherActor);
    if (aPlayer) {
        aPlayer->GetRockubeCharacterMovement()->;
    }
}




