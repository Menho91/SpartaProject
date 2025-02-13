#include "SpartaGameInstance.h"

USpartaGameInstance::USpartaGameInstance()
{

}

void USpartaGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
}
