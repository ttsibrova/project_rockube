// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RockubeBaseCharacter.h"

// Sets default values
ARockubeBaseCharacter::ARockubeBaseCharacter (const FObjectInitializer& ObjectInitializer)
    : Super (ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

    BeatSync = CreateDefaultSubobject <UBeatSyncComponent> (TEXT ("CharacterBeatSyncComponent"));
}


