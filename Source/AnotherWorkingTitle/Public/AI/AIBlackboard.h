// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AIConstants.h"

class ABuildingSite;
class AStockpile;
class AResourceNode;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
class FAIBlackboard
{
public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void SetResourceNode(AResourceNode* InResourceNode) { SetFlag(EBlackboardMask::ResourceNode); ResourceNode = InResourceNode; }
	AResourceNode* GetResourceNode() const { return IsSet(EBlackboardMask::ResourceNode) ? ResourceNode : nullptr; }
	
	void SetStockpile(AStockpile* InStockpile) { SetFlag(EBlackboardMask::Stockpile); Stockpile = InStockpile; }
	AStockpile* GetStockpile() const { return IsSet(EBlackboardMask::Stockpile) ? Stockpile : nullptr; }
	
	void SetBuildingSite(ABuildingSite* InBuildingSite) { SetFlag(EBlackboardMask::BuildingSite); BuildingSite = InBuildingSite; }
	ABuildingSite* GetBuildingSite() const { return IsSet(EBlackboardMask::BuildingSite) ? BuildingSite : nullptr; }
	
	void SetSlotIndex(const int32 InSlotIndex) { SetFlag(EBlackboardMask::SlotIndex); SlotIndex = InSlotIndex; }
	int32 GetSlotIndex() const { return IsSet(EBlackboardMask::SlotIndex) ? SlotIndex : INDEX_NONE; }
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool IsSet(EBlackboardMask Mask) const { return (Flags & AI_ENUM_TO_FLAG(Mask)) != 0; }

	void Clear(EBlackboardMask Mask) { Flags &= ~AI_ENUM_TO_FLAG(Mask); }
	void ClearAll() { Flags = 0; }
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void SetFlag(EBlackboardMask Mask) { Flags |= AI_ENUM_TO_FLAG(Mask); }
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	uint32 Flags = 0;
	AResourceNode* ResourceNode = nullptr;
	AStockpile* Stockpile = nullptr;
	ABuildingSite* BuildingSite = nullptr;
	int32 SlotIndex = INDEX_NONE;
};
