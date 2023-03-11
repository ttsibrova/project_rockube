// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RockubeBaseCharacter.h"

// Sets default values
ARockubeBaseCharacter::ARockubeBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BeatSync = CreateDefaultSubobject <UBeatSyncComponent> (TEXT ("CharacterBeatSyncComponent"));
}


