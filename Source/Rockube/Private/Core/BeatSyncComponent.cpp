// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BeatSyncComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UBeatSyncComponent::UBeatSyncComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;

    // ...
}


// Called when the game starts
void UBeatSyncComponent::BeginPlay()
{
    Super::BeginPlay();

    auto Actor = UGameplayStatics::GetActorOfClass (GetWorld(), AGlobalBPMTickCounter::StaticClass());
    if (!Actor) {
        UE_LOG (LogTemp, Fatal, TEXT ("Expected to find Global Tick counter. Check level blueprint"))
        return;
    }
    GlobalBPMCounter = Cast <AGlobalBPMTickCounter> (Actor);
}

double UBeatSyncComponent::GetTimeUntilNextBeat() const
{
    return GlobalBPMCounter->GetEstimatedRemainingTimeNextBeat();

}

