#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UWidgetComponent;
class ASpartaPlayerController;

UCLASS()
class SPARTAPROJECT_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();

	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(int32 Amount);

	void SetCharacterSpeed();

	void SlowSpeed(float Time, float Multiplier);
	void UnslowSpeed();
	void Blind(float Time);
	void Unblind();
	void Reverse(float Time);
	void Unreverse();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);
	UFUNCTION()
	void Menu(const FInputActionValue& value);

	void OnDeath();
	void UpdateOverHeadWidget();
	void UpdateHPWidget();
	ASpartaPlayerController* GetSpartaPlayerController() const;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverHeadWidget;

	bool bIsBlind = false;
	bool bIsReverse = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 Health = MaxHealth;

	float SpeedMultiplier = 1.0f;

	FTimerHandle SlowTimerHandle;
	FTimerHandle BlindTimerHandle;
	FTimerHandle ReverseTimerHandle;

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	int32 SlowCount = 0;
};
