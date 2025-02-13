#include "BlindItem.h"
#include "SpartaCharacter.h"
#include "Kismet/GameplayStatics.h"

ABlindItem::ABlindItem()
{
	ItemType = "Blind";
}

void ABlindItem::ActivateItem(AActor* Activator)
{
	if (bIsBlind) return;
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0))
			{
				if (ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(Character))
				{
					if (SpartaCharacter->bIsBlind)
					{
						GetWorld()->GetTimerManager().ClearTimer(BlindTimerHandle);
						SpartaCharacter->Blind(BlindTime);
					}
					else
					{
						SpartaCharacter->Blind(BlindTime);
					}
					
				}
			}
		}
	}

	StaticMesh->SetVisibility(false);

	GetWorld()->GetTimerManager().SetTimer(
		BlindTimerHandle,
		this,
		&ABlindItem::DeactivateEffect,
		BlindTime,
		false
	);

	bIsBlind = true;
}

void ABlindItem::DeactivateEffect()
{
	if (UWorld* World = GetWorld())
	{
		if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0))
		{
			if (ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(Character))
			{
				SpartaCharacter->Unblind();
			}
		}
	}
	DestroyItem();
}
