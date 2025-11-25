// (c) 2024 by Crenetic GmbH Studios

#include "AI/Storage/WorkingMemoryFact.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FWorkingMemoryBaseFact& FWorkingMemoryBaseFact::CopyFrom(const FWorkingMemoryBaseFact& Source)
{
	Flags = Source.Flags;
	NodeType = Source.NodeType;

	Actor = Source.Actor;
	Object = Source.Object;
	Component = Source.Component;
	ObjectClass = Source.ObjectClass;

	Position = Source.Position;
	Name = Source.Name;
	Value = Source.Value;

	return *this;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool FWorkingMemoryBaseFact::Matches(const FWorkingMemoryBaseFact& Fact) const
{
	const uint32 MatchingFlags = Flags & Fact.Flags;
	if (MatchingFlags == 0)
		return false;

	return Matches(MatchingFlags, Fact);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool FWorkingMemoryBaseFact::Matches(const uint32 CheckFlags, const FWorkingMemoryBaseFact& Fact) const
{
	if (IsValid(EFactMask::Actor, CheckFlags) && Fact.Actor != Actor)
		return false;

	if (IsValid(EFactMask::Component, CheckFlags) && Fact.Component != Component)
		return false;

	if (IsValid(EFactMask::Object, CheckFlags) && Fact.Object != Object)
		return false;

	if (IsValid(EFactMask::ObjectClass, CheckFlags) && Fact.ObjectClass != ObjectClass)
		return false;

	if (IsValid(EFactMask::NodeType, CheckFlags) && Fact.NodeType != NodeType)
		return false;

	constexpr float PositionThreshold = 50.0f;
	if (IsValid(EFactMask::Position, CheckFlags) && !Fact.Position.Equals(Position, PositionThreshold))
		return false;

	if (IsValid(EFactMask::Name, CheckFlags) && Fact.Name != Name)
		return false;

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool FWorkingMemoryQuery::Matches(const FWorkingMemoryBaseFact& Fact) const
{
	const uint32 CombinedFlags = QueryFlags | Flags;
	if ((CombinedFlags & Fact.GetFlags()) == 0)
		return false;

	return FWorkingMemoryBaseFact::Matches(Flags, Fact);
}
