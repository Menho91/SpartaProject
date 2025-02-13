#include "SpartaGameState.h"
#include "SpartaPlayerController.h"
#include "SpartaGameInstance.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (USpartaGameInstance* SpartaGameInstance = GetSpartaGameInstance())
	{
		SpartaGameInstance->AddToScore(Amount);
	}

	if(ASpartaPlayerController* SpartaPlayerController = GetSpartaPlayerController())
	{
		if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
		{
			UFunction* PlayCoinAnimFunc = HUDWidget->FindFunction(FName("PlayCoinAnim"));
			if (PlayCoinAnimFunc)
			{
				HUDWidget->ProcessEvent(PlayCoinAnimFunc, nullptr);
			}
		}
	}
}

void ASpartaGameState::OnGameOver()
{
	if (ASpartaPlayerController* SpartaPlayerController = GetSpartaPlayerController())
	{
		SpartaPlayerController->SetPause(true);
		SpartaPlayerController->ShowMainMenu(true);
	}
}

void ASpartaGameState::StartLevel()
{
	if (ASpartaPlayerController* SpartaPlayerController = GetSpartaPlayerController())
	{
		SpartaPlayerController->ShowGameHUD();
	}

	if (USpartaGameInstance* SpartaGameInstance = GetSpartaGameInstance())
	{
		CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		CurrentWaveIndex = SpartaGameInstance->CurrentWaveIndex;
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = SpawnItemCountForWaves[CurrentWaveIndex];
	ASpawnVolume* SpawnVolume = nullptr;
	
	for (int i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	if (SpawnVolume)
	{
		SpawnVolume->SpawnDebuffItem((CurrentWaveIndex + 1)* 2);
	}
	
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Wave %d Start!"), CurrentWaveIndex + 1);

	if (ASpartaPlayerController* SpartaPlayerController = GetSpartaPlayerController())
	{
		if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
		{
			UFunction* PlayWaveAnimFunc = HUDWidget->FindFunction(FName("PlayWaveAnim"));
			if (PlayWaveAnimFunc)
			{
				HUDWidget->ProcessEvent(PlayWaveAnimFunc, nullptr);
			}
		}
	}
}

void ASpartaGameState::OnLevelTimeUp()
{
	EndWave();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UpdateHUD();

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndWave();
	}
}

void ASpartaGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	if (USpartaGameInstance* SpartaGameInstance = GetSpartaGameInstance())
	{
		CurrentWaveIndex++;
		SpartaGameInstance->CurrentWaveIndex = CurrentWaveIndex;
	}

	AddScore(Score);

	if (CurrentWaveIndex >= MaxWaves)
	{
		CurrentWaveIndex = 0;
		EndLevel();
	}
	else
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
}

void ASpartaGameState::EndLevel()
{
	if (USpartaGameInstance* SpartaGameInstance = GetSpartaGameInstance())
	{
		CurrentLevelIndex++;
		SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		SpartaGameInstance->CurrentWaveIndex = 0;
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}
	
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (ASpartaPlayerController* SpartaPlayerController = GetSpartaPlayerController())
	{
		if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
		{
			if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
			{
				float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
				TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time : %.1f"), RemainingTime)));
			}

			if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
			{
				if (USpartaGameInstance* SpartaGameInstance = GetSpartaGameInstance())
				{
					ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), SpartaGameInstance->TotalScore)));
				}
			}

			if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
			{
				LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), CurrentLevelIndex + 1)));
			}

			if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
			{
				WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), CurrentWaveIndex + 1)));
			}

			if (UProgressBar* CoinBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("Coin"))))
			{
				CoinBar->SetPercent(static_cast<float>(CollectedCoinCount) / static_cast<float>(SpawnedCoinCount));
			}
			if (UTextBlock* CoinText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CoinText"))))
			{
				CoinText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CollectedCoinCount, SpawnedCoinCount)));
			}
		}
	}
}

ASpartaPlayerController* ASpartaGameState::GetSpartaPlayerController() const
{
	return Cast<ASpartaPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

USpartaGameInstance* ASpartaGameState::GetSpartaGameInstance() const
{
	return Cast<USpartaGameInstance>(GetGameInstance());
}
