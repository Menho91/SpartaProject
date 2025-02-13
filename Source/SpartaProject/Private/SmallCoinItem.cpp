#include "SmallCoinItem.h"

ASmallCoinItem::ASmallCoinItem()
{
	ItemType = "SmallCoin";
	PointValue = 10;
}

void ASmallCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}

