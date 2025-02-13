#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "BlindItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ABlindItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ABlindItem();

	virtual void ActivateItem(AActor* Activator) override;
	void DeactivateEffect();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	float BlindTime = 3.0f;

	FTimerHandle BlindTimerHandle;
	bool bIsBlind = false;
	
};
