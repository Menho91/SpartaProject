#include "BlindItem.h"
#include "SpartaCharacter.h"
#include "Kismet/GameplayStatics.h"

ABlindItem::ABlindItem()
{
	ItemType = "Blind";
}

void ABlindItem::ActivateItem(AActor* Activator)
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
					SpartaCharacter->Blind(BlindTime);
				}
			}
		}
		DestroyItem();
	}
}

