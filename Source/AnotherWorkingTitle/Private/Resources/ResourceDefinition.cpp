// (c) 2025 MK


#include "Resources/ResourceDefinition.h"

#include "Resources/ResourceStack.h"
#include "Settlers/Needs.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const FResourceStack FResourceStack::Invalid;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const FPrimaryAssetType UResourceDefinition::AssetType = TEXT("Resource");

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float UResourceDefinition::GetNeedChange(const ENeedType NeedType) const
{
	switch (NeedType)
	{
	case ENeedType::Hunger: return HungerChange;
	case ENeedType::Thirst: return ThirstChange;
	case ENeedType::Cold: return ColdChange;
	case ENeedType::Fatigue: return FatigueChange;
	case ENeedType::Damage: return DamageChange;
	}
	return 0;
}

#if WITH_EDITOR

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FResourceStack::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(FResourceStack, Amount) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(FResourceStack, Resource))
	{
		if (Resource)
		{
			Amount = FMath::Clamp(Amount, 1, Resource->MaxStackSize);
		}
	}
}


#endif