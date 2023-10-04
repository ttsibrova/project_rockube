// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/BeatSyncComponent.h"
#include "RockubeBaseCharacter.generated.h"

UCLASS()
class ROCKUBE_API ARockubeBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ARockubeBaseCharacter (const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get ());
    UFUNCTION()
    UBeatSyncComponent* GetBeatSyncComp() const { return BeatSync.Get(); };

protected:
    UPROPERTY (Category = "Synchronization", VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBeatSyncComponent> BeatSync;
};
