// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AIConstants.h"

class ANeedsModifier;
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
	
	void SetNeedsModifier(ANeedsModifier* InNeedsModifier) { SetFlag(EBlackboardMask::NeedsModifier); NeedsModifier = InNeedsModifier; }
	ANeedsModifier* GetNeedsModifier() const { return IsSet(EBlackboardMask::NeedsModifier) ? NeedsModifier : nullptr; }
	
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
	ANeedsModifier* NeedsModifier = nullptr;
};
