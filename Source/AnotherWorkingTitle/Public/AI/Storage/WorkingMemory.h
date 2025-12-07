// (c) 2024 by Crenetic GmbH Studios

#pragma once

#include "CoreMinimal.h"
#include "WorkingMemoryFact.h"
#include "AI/AIConstants.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class FFactStorage
{
public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FFactStorage();
	~FFactStorage();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FWorkingMemoryFact&			AllocateFact();
	bool						InvalidateFact(const FWorkingMemoryFact* Fact);
	void						Clear();		

	TArray<FWorkingMemoryFact>&			GetFacts()			{ return Facts; }
	const TArray<FWorkingMemoryFact>&	GetFacts() const	{ return Facts; }

private:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	TArray<FWorkingMemoryFact>	Facts;
	TArray<int32>				FreeIndices;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class FWorkingMemory
{
public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FWorkingMemory();
	~FWorkingMemory();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FWorkingMemoryFact&			Create(const EFactType FactType);
	void						Free(const FWorkingMemoryFact* Fact);
	void						Move(const FWorkingMemoryFact* Fact, FWorkingMemory& DestMemory);
	void						Clear();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	bool						IsModified();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	const FWorkingMemoryFact*	Find(const FWorkingMemoryQuery& Query) const;
	void						Collect(const FWorkingMemoryQuery& Query, TArray<const FWorkingMemoryFact*>& Results) const;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename Func>
	const FWorkingMemoryFact*	Find(const FWorkingMemoryQuery& Query, const Func& Evaluator) const;

private:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FFactStorage				FactStorage[NumFactTypes];
	bool						bModified = false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename Func>
const FWorkingMemoryFact* FWorkingMemory::Find(const FWorkingMemoryQuery& Query, const Func& Evaluator) const
{
	const TArray<FWorkingMemoryFact>& Facts = FactStorage[static_cast<int32>(Query.GetType())].GetFacts();
	for (int32 Index = 0; Index < Facts.Num(); ++Index)
	{
		if (Facts[Index].IsValid())
		{
			if (Query.Matches(Facts[Index]) && Evaluator(Facts[Index]))
			{
				return &Facts[Index];
			}
		}
	}
	return nullptr;
}
