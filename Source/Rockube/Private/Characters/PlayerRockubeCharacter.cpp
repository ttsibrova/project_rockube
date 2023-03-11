// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerRockubeCharacter.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerRockubeCharacter::APlayerRockubeCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComponent = CreateDefaultSubobject <USpringArmComponent> (TEXT ("SpringArmComponent"));
	SpringArmComponent->SetupAttachment (RootComponent);
	SpringArmComponent->bUsePawnControlRotation = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent> (TEXT ("CameraComponent"));
	CameraComponent->SetupAttachment (SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement ()->bOrientRotationToMovement = true;
	GetCharacterMovement ()->RotationRate = FRotator (0.0f, 50.0f, 0.0f); // ...at this rotation rate

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void APlayerRockubeCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PlayerController = Cast<APlayerController> (Controller)) {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> (PlayerController->GetLocalPlayer ())) {
            Subsystem->AddMappingContext (DefaultMappingContext, 0);
        }
    }
}

//
//void APlayerRockubeCharacter::Tick (float DeltaTime)
//{
//}

// Called to bind functionality to input
void APlayerRockubeCharacter::SetupPlayerInputComponent (UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent (PlayerInputComponent);

}