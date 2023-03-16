// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RocubeGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ROCKUBE_API ARocubeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	virtual void InitGame (const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};
