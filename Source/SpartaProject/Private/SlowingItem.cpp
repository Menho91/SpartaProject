#include "SlowingItem.h"
#include "SpartaCharacter.h"
#include "Kismet/GameplayStatics.h"

ASlowingItem::ASlowingItem()
{
	ItemType = "Slow";
}

void ASlowingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0))
			{
				if (ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(Character))
				{
					SpartaCharacter->SlowSpeed(SlowingTime, SlowMultiplier);
				}
			}
		}
		DestroyItem();
	}
}

