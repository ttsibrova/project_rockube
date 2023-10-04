// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GlobalBPMTickCounter.h"
#include "Quartz/QuartzSubsystem.h"
#include "Quartz/AudioMixerClockHandle.h"

#include "Sound/QuartzQuantizationUtilities.h"


// Sets default values
AGlobalBPMTickCounter::AGlobalBPMTickCounter()
{
     // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    QuantizationBoundary.CountingReferencePoint = EQuarztQuantizationReference::CurrentTimeRelative;

}

// Called when the game starts or when spawned
void AGlobalBPMTickCounter::BeginPlay()
{
    Super::BeginPlay();

}

//Creating the most essential clocks for the game
//BPM and other important setting should be initialized already
void AGlobalBPMTickCounter::InitClock()
{
    UWorld* CurrentWorld = GetWorld ();
    if (CurrentWorld) {
        UQuartzSubsystem* QuartzSubsystem = CurrentWorld->GetSubsystem<UQuartzSubsystem> ();
        if (QuartzSubsystem) {
            WorldQuartzSubsystem = QuartzSubsystem;
            //Default setting are fine for now, should be changed via changing Quarts Time Signature inside settings
            //Beat is 4/4
            FQuartzClockSettings QuartzClockSettings;
            QuartzClock = WorldQuartzSubsystem->CreateNewClock (CurrentWorld, QuartzClockName, QuartzClockSettings);
            if (!QuartzClock) {
                UE_LOG (LogTemp, Fatal, TEXT ("Failed to create QuartzClock for BPMCounter"))
                    return;
            }
            QuartzClock->SetBeatsPerMinute (CurrentWorld, QuantizationBoundary, QuartzEventDelegate, QuartzClock, BPM);
            OnBPMClockCreatedDelegate.Broadcast();
        }
    }
}

void AGlobalBPMTickCounter::StartBeatTick()
{
    QuartzClock->StartClock (GetWorld(), QuartzClock);
    QuartzStartTime = GetWorld()->GetTimeSeconds();
    bIsFirstBeat = true;
    if (StartOffset > 0) {
        GetWorld()->GetTimerManager().SetTimer (OffsetTimer, this, &AGlobalBPMTickCounter::PerformBeat, StartOffset, false);
    } else {
        PerformBeat();
    }
}

void AGlobalBPMTickCounter::StopBeatTick()
{
    QuartzClock->StopClock (GetWorld(), true, QuartzClock);
}

bool AGlobalBPMTickCounter::IsOnBeat (double GoodTolerance, double PerfectTolerance, bool& bIsPerfect, double& DeltaError)
{
    double CurrentTime = GetWorld()->GetTimeSeconds();
    FString Time = FString::SanitizeFloat (GetWorld ()->GetTimeSeconds ());
    GEngine->AddOnScreenDebugMessage (-1, 10.0f, FColor::Black, Time);
    bIsPerfect = false;

    double LastDeltaTime = FMath::Abs (CurrentTime - LastBeatTime);
    double NextDeltaTime = FMath::Abs (ExpectedNextBeatTime - CurrentTime);
    double LastDeltaTimeNoAbs = CurrentTime - LastBeatTime;
    double NextDeltaTimeNoAbs = ExpectedNextBeatTime - CurrentTime;
    DeltaError = LastDeltaTime > NextDeltaTime ? NextDeltaTime : LastDeltaTime;

    if (LastDeltaTime <= PerfectTolerance || NextDeltaTime <= PerfectTolerance) {
        FString aWarningText = TEXT ("C++ Perfect. LastDelta error: ") + FString::SanitizeFloat (LastDeltaTimeNoAbs) + TEXT ("    NextDelta error: ") + FString::SanitizeFloat (NextDeltaTimeNoAbs);
        GEngine->AddOnScreenDebugMessage (-1, 5.0f, FColor::Green, aWarningText);
        bIsPerfect = true;
        return true;
    }
    if (LastDeltaTime <= GoodTolerance || NextDeltaTime <= GoodTolerance) {
        FString aWarningText = TEXT ("C++ Good. LastDelta error: ") + FString::SanitizeFloat (LastDeltaTimeNoAbs) + TEXT ("    NextDelta error: ") + FString::SanitizeFloat (NextDeltaTimeNoAbs);
        GEngine->AddOnScreenDebugMessage (-1, 5.0f, FColor::Yellow, aWarningText);
        return true;
    }
    FString aWarningText = TEXT ("C++ Miss. LastDelta error: ") + FString::SanitizeFloat (LastDeltaTimeNoAbs) + TEXT ("    NextDelta error: ") + FString::SanitizeFloat (NextDeltaTimeNoAbs);
    GEngine->AddOnScreenDebugMessage (-1, 5.0f, FColor::Red, aWarningText);
    return false;
}

double AGlobalBPMTickCounter::GetEstimatedRemainingTimeNextBeat()
{
    return ExpectedNextBeatTime - GetWorld()->GetTimeSeconds();
}

void AGlobalBPMTickCounter::PerformBeat()
{
    double CurrentTime = GetWorld()->GetTimeSeconds();
    if (bIsFirstBeat) {
        StartTime = CurrentTime;
        RecomputeBeatTime();
        LastBeatTime = StartTime;
        ExpectedNextBeatTime = StartTime + BeatTime;
        bIsFirstBeat = false;
    }
    LastBeatTime = CurrentTime;
    ExpectedNextBeatTime = QuartzLastBeatTime + StartOffset + BeatTime;

    //Desync checker
    double DeltaRealEstimated = LastBeatTime - QuartzLastBeatTime - StartOffset;
    if (GEngine && FMath::Abs (DeltaRealEstimated) > 0.01) {
        FString aWarningText = TEXT ("Desync detected. Desync size: ") + FString::SanitizeFloat (DeltaRealEstimated);
        GEngine->AddOnScreenDebugMessage (-1, 3.0f, FColor::Red, aWarningText);
    }
    BeatOnDelegate.Broadcast();
}

void AGlobalBPMTickCounter::PerformQuartzBeat()
{
    QuartzLastBeatTime = GetWorld()->GetTimeSeconds();
    if (StartOffset > 0) {
        GetWorld()->GetTimerManager().SetTimer (OffsetTimer, this, &AGlobalBPMTickCounter::PerformBeat, StartOffset, false);
    } else {
        PerformBeat();
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

