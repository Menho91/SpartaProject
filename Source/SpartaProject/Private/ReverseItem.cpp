#include "ReverseItem.h"
#include "SpartaCharacter.h"
#include "Kismet/GameplayStatics.h"

AReverseItem::AReverseItem()
{
	ItemType = "Reverse";
}

void AReverseItem::ActivateItem(AActor* Activator)
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
					SpartaCharacter->Reverse(ReverseTime);
				}
			}
		}
		DestroyItem();
	}
}

