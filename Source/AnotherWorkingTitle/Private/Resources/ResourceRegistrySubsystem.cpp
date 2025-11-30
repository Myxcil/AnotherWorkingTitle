// (c) 2025 MK


#include "Resources/ResourceRegistrySubsystem.h"

#include "Resources/ResourceDefinition.h"
#include "Engine/AssetManager.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UResourceRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	TArray<FPrimaryAssetId> ResourceIds;
	UAssetManager::Get().GetPrimaryAssetIdList(UResourceDefinition::AssetType, ResourceIds);
	
	Resources.Reset();
	Resources.Reserve(ResourceIds.Num());
	
	ResourceMapByCategory.Reset();
	
	int32 NextRuntimeId = 1;
	for (const FPrimaryAssetId& Id : ResourceIds)
	{
		if (UObject* Asset = UAssetManager::Get().GetPrimaryAssetObject(Id))
		{
			if (UResourceDefinition* Resource = Cast<UResourceDefinition>(Asset))
			{
				Resource->RuntimeId = NextRuntimeId++;
				Resources.Add(Resource);
				
				TArray<const UResourceDefinition*>& ResourceMap = ResourceMapByCategory.FindOrAdd(Resource->Category);
				ResourceMap.Add(Resource);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const UResourceDefinition* UResourceRegistrySubsystem::FindById(const FName Id) const
{
	for (const UResourceDefinition* Resource : Resources)
	{
		if (Resource && Resource->Id == Id)
		{
			return Resource;
		}
	}
	return nullptr;
}
