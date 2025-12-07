// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AIConstants.h"

class ANeedModifierInteraction;
class ANeedModifierArea;
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
	
	void SetNeedsModifier(ANeedModifierArea* InNeedsModifier) { SetFlag(EBlackboardMask::NeedsModifier); NeedsModifier = InNeedsModifier; }
	ANeedModifierArea* GetNeedsModifier() const { return IsSet(EBlackboardMask::NeedsModifier) ? NeedsModifier : nullptr; }
	
	void SetNeedInteraction(ANeedModifierInteraction* InNeedInteraction) { SetFlag(EBlackboardMask::NeedInteraction); NeedInteraction = InNeedInteraction; }
	ANeedModifierInteraction* GetNeedInteraction() const { return IsSet(EBlackboardMask::NeedInteraction) ? NeedInteraction : nullptr; }
	
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
	ANeedModifierArea* NeedsModifier = nullptr;
	ANeedModifierInteraction* NeedInteraction = nullptr;
};
