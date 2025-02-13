#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowingItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ASlowingItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ASlowingItem();

	virtual void ActivateItem(AActor* Activator) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	float SlowMultiplier = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	float SlowingTime = 3.0f;
};
