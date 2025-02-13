#pragma once

#include "CoreMinimal.h"
#include "DebuffItemRow.generated.h"

USTRUCT(BlueprintType)
struct FDebuffItemRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ItemClass;
};
