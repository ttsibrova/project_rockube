// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GlobalBPMTickCounter.h"

// Sets default values
AGlobalBPMTickCounter::AGlobalBPMTickCounter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGlobalBPMTickCounter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGlobalBPMTickCounter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bBeatTickEnabled && GetWorld()->GetTimeSeconds() - ExpectedNextBeatTime > 0) {
		PerformBeat();
	}
}

void AGlobalBPMTickCounter::StartBeatTick()
{
	bBeatTickEnabled = false;
	StartTime = GetWorld()->GetTimeSeconds() + StartOffset;
	LastBeatTime = StartTime;
	RecomputeBeatTime();
	NumOfPerformedBeats = 0;
	ExpectedNextBeatTime = StartTime;
	GetWorld()->GetTimerManager().SetTimer (BeatTickTimer, this, &AGlobalBPMTickCounter::PerformBeat, StartOffset, false);
	//GetWorld()->GetTimerManager().SetTimer (BeatTickTimer, this, &AGlobalBPMTickCounter::StartBroadcast, StartOffset, false);
	BeatOnStartedDelegate.Broadcast();
}

void AGlobalBPMTickCounter::StopBeatTick()
{
	bBeatTickEnabled = false;
}

bool AGlobalBPMTickCounter::IsOnBeat (double GoodTolerance, double PerfectTolerance, bool& bIsPerfect, double& DeltaError)
{
	double CurrentTime = GetWorld()->GetTimeSeconds();
	bIsPerfect = false;

	double LastDeltaTime = FMath::Abs (CurrentTime - LastBeatTime);
	double NextDeltaTime = FMath::Abs (ExpectedNextBeatTime - CurrentTime);
	DeltaError = LastDeltaTime > NextDeltaTime ? NextDeltaTime : LastDeltaTime;

	if (LastDeltaTime <= PerfectTolerance || NextDeltaTime <= PerfectTolerance) {
		bIsPerfect = true;
		return true;
	}
	if (LastDeltaTime <= GoodTolerance || NextDeltaTime <= GoodTolerance) {
		return true;
	}
	return false;
}

double AGlobalBPMTickCounter::GetEstimatedRemainingTimeNextBeat()
{
	return ExpectedNextBeatTime - GetWorld()->GetTimeSeconds();
}

void AGlobalBPMTickCounter::PerformBeat()
{
	LastBeatTime = ExpectedNextBeatTime;
	ExpectedNextBeatTime = StartTime + ((NumOfPerformedBeats + 1) * BeatTime);
	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage (-1, 2.0f, FColor::Red, TEXT ("Tick c++"));

	//Desync checker
	double RealCurrentTime = GetWorld ()->GetTimeSeconds ();
	double EstimatedBeatTickTime = StartTime + (NumOfPerformedBeats * BeatTime);
	double DeltaRealEstimated = RealCurrentTime - EstimatedBeatTickTime;
	if (GEngine && FMath::Abs (DeltaRealEstimated) > 0.01) {
		FString aWarningText = TEXT ("Desync detected. Desync size: ") + FString::SanitizeFloat (DeltaRealEstimated);
		GEngine->AddOnScreenDebugMessage (-1, 3.0f, FColor::Red, aWarningText);
	}
	NumOfPerformedBeats++;
	BeatOnDelegate.Broadcast();

	if (!bBeatTickEnabled) {
		bBeatTickEnabled = true;
	}
}

void AGlobalBPMTickCounter::StartBroadcast()
{
	BeatOnStartedDelegate.Broadcast();
}


//Helpers
void AGlobalBPMTickCounter::RecomputeBeatTime()
{
	BeatTime = 60. / BPM;
}

