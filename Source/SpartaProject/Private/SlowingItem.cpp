#include "SlowingItem.h"
#include "SpartaCharacter.h"
#include "Kismet/GameplayStatics.h"

ASlowingItem::ASlowingItem()
{
	ItemType = "Slow";
}

void ASlowingItem::ActivateItem(AActor* Activator)
{
	if (bHasSlowed) return;
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0))
			{
				if (ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(Character))
				{
					SpartaCharacter->SlowSpeed(SlowMultiplier);
				}
			}
		}
	}
	
	StaticMesh->SetVisibility(false);

	GetWorld()->GetTimerManager().SetTimer(
		SlowTimerHandle,
		this,
		&ASlowingItem::DeactivateEffect,
		SlowingTime,
		false
	);

	bHasSlowed = true;
}

void ASlowingItem::DeactivateEffect()
{
	if (UWorld* World = GetWorld())
	{
		if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0))
		{
			if (ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(Character))
			{
				SpartaCharacter->UnslowSpeed(SlowMultiplier);
			}
		}
	}
	DestroyItem();
}
