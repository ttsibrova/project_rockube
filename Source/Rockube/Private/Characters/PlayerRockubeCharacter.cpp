// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerRockubeCharacter.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Movement/RockubeMovementComponent.h"

APlayerRockubeCharacter::APlayerRockubeCharacter (const FObjectInitializer& ObjectInitializer)
: Super (ObjectInitializer.SetDefaultSubobjectClass<URockubeMovementComponent> (ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	RockubeMovmentComponent = Cast <URockubeMovementComponent> (GetCharacterMovement());

	SpringArmComponent = CreateDefaultSubobject <USpringArmComponent> (TEXT ("SpringArmComponent"));
	SpringArmComponent->SetupAttachment (RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent> (TEXT ("CameraComponent"));
	CameraComponent->SetupAttachment (SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator (0.0f, 500.0f, 0.0f); // ...at this rotation rate

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void APlayerRockubeCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PlayerController = Cast<APlayerController> (Controller)) {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> (PlayerController->GetLocalPlayer())) {
            Subsystem->AddMappingContext (DefaultMappingContext, 0);
        }
    }
}


// Called to bind functionality to input
void APlayerRockubeCharacter::SetupPlayerInputComponent (UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent (PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent> (PlayerInputComponent)) {

		EnhancedInputComponent->BindAction (JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction (JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction (MoveAction, ETriggerEvent::Triggered, this, &APlayerRockubeCharacter::Move);
		EnhancedInputComponent->BindAction (LookAction, ETriggerEvent::Triggered, this, &APlayerRockubeCharacter::Look);
		
		EnhancedInputComponent->BindAction (DashAction, ETriggerEvent::Started, this, &APlayerRockubeCharacter::DashPressed);
		EnhancedInputComponent->BindAction (DashAction, ETriggerEvent::Completed, this, &APlayerRockubeCharacter::DashReleased);

		EnhancedInputComponent->BindAction (InteractAction, ETriggerEvent::Started, this, &APlayerRockubeCharacter::InteractPressed);
	}
}

void APlayerRockubeCharacter::Move (const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation (0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix (YawRotation).GetUnitAxis (EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix (YawRotation).GetUnitAxis (EAxis::Y);

		// add movement 
		AddMovementInput (ForwardDirection, MovementVector.Y);
		AddMovementInput (RightDirection, MovementVector.X);
	}
}

void APlayerRockubeCharacter::Look (const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput (LookAxisVector.X);
		AddControllerPitchInput (LookAxisVector.Y);
	}
}

void APlayerRockubeCharacter::Interact (const FInputActionValue& Value)
{
}

void APlayerRockubeCharacter::DashPressed (const FInputActionValue& Value)
{
	FString Time = FString::SanitizeFloat (GetWorld ()->GetTimeSeconds ());
	GEngine->AddOnScreenDebugMessage (-1, 10.0f, FColor::Black, Time);
	RockubeMovmentComponent->DashPressed();
}
void APlayerRockubeCharacter::DashReleased (const FInputActionValue& Value)
{
	RockubeMovmentComponent->DashReleased();
}

void APlayerRockubeCharacter::JumpPressed (const FInputActionValue& Value)
{
	RockubeMovmentComponent->JumpPressed();
}
void APlayerRockubeCharacter::JumpReleased (const FInputActionValue& Value)
{
	RockubeMovmentComponent->JumpReleased();
}

void APlayerRockubeCharacter::InteractPressed (const FInputActionValue& Value)
{
	double PerfTolerance = 0.05;
	double GoodTolerance = 0.1;
	double DeltaError;
	bool bIsPerfect;
	bool bIsOnBeat = GetBeatSyncComp()->GetBPMCounter()->IsOnBeat (GoodTolerance, PerfTolerance, bIsPerfect, DeltaError);
}
