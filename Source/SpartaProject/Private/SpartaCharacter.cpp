#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Overlay.h"

ASpartaCharacter::ASpartaCharacter()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(GetMesh());
	OverHeadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

int32 ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(int32 Amount)
{
	Health = FMath::Clamp(Health + Amount, 0, MaxHealth);
	
	UpdateHPWidget();
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHPWidget();
	UpdateOverHeadWidget();
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
				);
			}

			if (PlayerController->MenuAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MenuAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Menu
				);
			}
		}
	}
}

float ASpartaCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0, MaxHealth);
	
	UpdateHPWidget();

	if (Health <= 0)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverHeadWidget()
{
	if (!OverHeadWidget) return;

	UUserWidget* OverHeadWidgetInstance = OverHeadWidget->GetUserWidgetObject();
	if (!OverHeadWidgetInstance) return;

	if (SlowCount > 0)
	{
		UFunction* SlowFunc = OverHeadWidgetInstance->FindFunction(FName("SlowFunction"));
		if (SlowFunc)
		{
			OverHeadWidgetInstance->ProcessEvent(SlowFunc, nullptr);
		}
		if (UTextBlock* SlowText = Cast<UTextBlock>(OverHeadWidgetInstance->GetWidgetFromName(TEXT("SlowText"))))
		{
			SlowText->SetText(FText::FromString(FString::Printf(TEXT("X%d"), SlowCount)));
		}
	}
	else
	{
		UFunction* UnSlowFunc = OverHeadWidgetInstance->FindFunction(FName("UnSlowFunction"));
		if (UnSlowFunc)
		{
			OverHeadWidgetInstance->ProcessEvent(UnSlowFunc, nullptr);
		}
	}

	if (bIsBlind)
	{
		UFunction* BlindFunc = OverHeadWidgetInstance->FindFunction(FName("BlindFunction"));
		if (BlindFunc)
		{
			OverHeadWidgetInstance->ProcessEvent(BlindFunc, nullptr);
		}
	}
	else
	{
		UFunction* UnBlindFunc = OverHeadWidgetInstance->FindFunction(FName("UnBlindFunction"));
		if (UnBlindFunc)
		{
			OverHeadWidgetInstance->ProcessEvent(UnBlindFunc, nullptr);
		}
	}
}

void ASpartaCharacter::UpdateHPWidget()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UProgressBar* HPBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HPBar"))))
				{
					HPBar->SetPercent(static_cast<float>(Health) / static_cast<float>(MaxHealth));
				}

				if (UTextBlock* HPText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("HPText"))))
				{
					HPText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Health)));
				}

				UFunction* PlayAnimFunc = HUDWidget->FindFunction(FName("PlayDamageAnim"));
				if (PlayAnimFunc)
				{
					HUDWidget->ProcessEvent(PlayAnimFunc, nullptr);
				}
			}
		}
	}
}

void ASpartaCharacter::SlowSpeed(float Value)
{
	SlowCount++;
	NormalSpeed *= Value;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	UpdateOverHeadWidget();
}

void ASpartaCharacter::UnslowSpeed(float Value)
{
	SlowCount--;
	NormalSpeed /= Value;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	UpdateOverHeadWidget();
}

void ASpartaCharacter::Blind(float Value)
{
	CameraComp->SetActive(false);
	bIsBlind = true;
	UpdateOverHeadWidget();
}

void ASpartaCharacter::Unblind()
{
	SpringArmComp->bUsePawnControlRotation = true;
	bIsBlind = false;
	UpdateOverHeadWidget();
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
	if (!Controller)
	{
		return;
	}
	
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void ASpartaCharacter::Menu(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
			{
				SpartaPlayerController->SetPause(true);
				SpartaPlayerController->ShowMainMenu(true);
			}
		}
	}
}

