// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlobalBPMTickCounter.h"
#include "BeatSyncComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROCKUBE_API UBeatSyncComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    // Sets default values for this component's properties
    UBeatSyncComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:    
    UFUNCTION (BlueprintCallable)
    double GetTimeUntilNextBeat() const;
    UFUNCTION (BlueprintCallable)
    double GetBeatTime() const { return GlobalBPMCounter->GetBeatTime(); }


    UFUNCTION (BlueprintCallable)
    AGlobalBPMTickCounter* GetBPMCounter() { return GlobalBPMCounter; }

    FOnBeatDelegate& GetBeatDelegate() { return GlobalBPMCounter->GetBeatDelegate(); }

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Synchronizaiton")
    AGlobalBPMTickCounter* GlobalBPMCounter;

private:

};
