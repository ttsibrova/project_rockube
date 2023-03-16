// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobalBPMTickCounter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE (FOnBeatDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE (FOnBeatStartedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE (FOnBPMClockCreatedDelegate);

class UQuartzSubsystem;
class UQuartzClockHandle;

UCLASS()
class ROCKUBE_API AGlobalBPMTickCounter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGlobalBPMTickCounter();

protected:
	virtual void BeginPlay() override;

public:	

	UFUNCTION (BlueprintCallable) void InitClock();
	UFUNCTION (BlueprintCallable) void StartBeatTick();
	UFUNCTION (BlueprintCallable) void StopBeatTick();

	UFUNCTION (BlueprintCallable) bool IsOnBeat (double GoodTolerance, double PerfectTolerance, bool& bIsPerfect, double& DeltaError);
	UFUNCTION (BlueprintCallable) double GetEstimatedRemainingTimeNextBeat();
	UFUNCTION (BlueprintCallable) void PerformQuartzBeat();

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

	UPROPERTY (BlueprintAssignable)
	FOnBPMClockCreatedDelegate OnBPMClockCreatedDelegate;

	UPROPERTY (BlueprintReadWrite)
	UQuartzClockHandle* QuartzClock;
	UPROPERTY (BlueprintReadOnly)
	FName				QuartzClockName = TEXT ("BMPSyncClock");

private:
	double				StartTime = 0.;
	double				QuartzStartTime = 0.;
	double				LastBeatTime = 0.;
	double				QuartzLastBeatTime = 0.;
	double				ExpectedNextBeatTime = 0;
	FTimerHandle		OffsetTimer;
	UQuartzSubsystem*	WorldQuartzSubsystem;
	bool				bIsFirstBeat = false;

	//not sure if I ever really need them
	//Will keep it in case GC making things dead
	FQuartzQuantizationBoundary QuantizationBoundary;
	FOnQuartzCommandEventBP		QuartzEventDelegate;
};
