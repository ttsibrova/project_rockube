// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/RocubeGameModeBase.h"
#include "Core/GlobalBPMTickCounter.h"

void ARocubeGameModeBase::InitGame (const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame (MapName, Options, ErrorMessage);
    auto CurrentWorld = GetWorld();
    if (!CurrentWorld) {
        UE_LOG (LogTemp, Fatal, TEXT ("Panik mode activated. No world is ready to spawn BPMTickCounter in"))
        return;
    }
    //I really need this to be loaded right at the start of the level
    CurrentWorld->SpawnActor<AGlobalBPMTickCounter>();
}
