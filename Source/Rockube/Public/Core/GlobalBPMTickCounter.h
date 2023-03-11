// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobalBPMTickCounter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE (FOnBeatDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE (FOnBeatStartedDelegate);

UCLASS()
class ROCKUBE_API AGlobalBPMTickCounter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGlobalBPMTickCounter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION (BlueprintCallable) void StartBeatTick();
	UFUNCTION (BlueprintCallable) void StopBeatTick();

	UFUNCTION (BlueprintCallable) bool IsOnBeat (double GoodTolerance, double PerfectTolerance, bool& bIsPerfect, double& DeltaError);
	UFUNCTION (BlueprintCallable) double GetEstimatedRemainingTimeNextBeat();

	FOnBeatDelegate& GetBeatDelegate() { return BeatOnDelegate;}
	FOnBeatStartedDelegate& GetBeatStartedDelegate() { return BeatOnStartedDelegate;}

private:
	void RecomputeBeatTime();
	void PerformBeat();
	void StartBroadcast();


protected:
	UPROPERTY (BlueprintReadWrite, EditAnywhere, Category = "Synchronization")
		double BPM = 120.;
	UPROPERTY (BlueprintReadWrite, EditAnywhere, Category = "Synchronization")
		double StartOffset = 0.0;
	UPROPERTY (BlueprintReadOnly, Category = "Synchronization")
		double BeatTime = 0.5;

	UPROPERTY (BlueprintAssignable)
	FOnBeatDelegate BeatOnDelegate;

	UPROPERTY (BlueprintAssignable)
	FOnBeatStartedDelegate BeatOnStartedDelegate;

private:
	double			StartTime = 0.;
	double			LastBeatTime = 0.;
	double			ExpectedNextBeatTime = 0;
	bool			bBeatTickEnabled = false;
	int32			NumOfPerformedBeats = 0; // for debug purposes
	FTimerHandle	BeatTickTimer;
};
