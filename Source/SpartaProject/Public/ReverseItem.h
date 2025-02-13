#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ReverseItem.generated.h"

UCLASS()
class SPARTAPROJECT_API AReverseItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AReverseItem();

	virtual void ActivateItem(AActor* Activator) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	float ReverseTime = 3.0f;
};
