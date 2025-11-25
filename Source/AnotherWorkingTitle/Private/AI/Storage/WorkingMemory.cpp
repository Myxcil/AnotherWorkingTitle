// (c) 2024 by Crenetic GmbH Studios

#include "AI/Storage/WorkingMemory.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FFactStorage::FFactStorage()
{
	Facts.Reset(1024);
	FreeIndices.Reset(1024);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FFactStorage::~FFactStorage()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FWorkingMemoryFact& FFactStorage::AllocateFact()
{
	int32 Index;
	if (FreeIndices.Num() == 0)
	{
		Index = Facts.AddDefaulted();
	}
	else
	{
		const int32 Last = FreeIndices.Num() - 1;
		Index = FreeIndices[Last];
		FreeIndices.RemoveAt(Last);
	}
	check(Facts.IsValidIndex(Index));
	return Facts[Index];
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool FFactStorage::InvalidateFact(const FWorkingMemoryFact* Fact)
{
	const int32 Index = Fact - Facts.GetData();
	if (Facts.IsValidIndex(Index))
	{
		check(!Facts[Index].IsStatic());
		Facts[Index].Invalidate();
		FreeIndices.Push(Index);
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FFactStorage::Clear()
{
	for (int32 I=0; I < Facts.Num(); ++I)
	{
		if (Facts.IsValidIndex(I) && !Facts[I].IsStatic())
		{
			Facts[I].Invalidate();
			FreeIndices.Push(I);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------------------------------------------
FWorkingMemory::FWorkingMemory()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FWorkingMemory::~FWorkingMemory()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FWorkingMemoryFact& FWorkingMemory::Create(const EFactType FactType)
{
	FWorkingMemoryFact& Fact = FactStorage[static_cast<int32>(FactType)].AllocateFact();
	Fact.SetType(FactType);
	bModified = true;
	return Fact;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorkingMemory::Free(const FWorkingMemoryFact* Fact)
{
	if (Fact != nullptr)
	{
		const EFactType Type = Fact->GetType();
		if (!FactStorage[static_cast<int32>(Type)].InvalidateFact(Fact))
		{
			//UE_LOG(?, Warning, TEXT("FWorkingMemory: failed to free fact %p"), Fact);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorkingMemory::Move(const FWorkingMemoryFact* Fact, FWorkingMemory& DestMemory)
{
	check(Fact);
	check(&DestMemory != this);

	FWorkingMemoryFact& NewFact = DestMemory.Create(Fact->GetType());
	NewFact.CopyFrom(*Fact);
	Free(Fact);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorkingMemory::Clear()
{
	// Skip Static facts, they are forever
	for (int32 I=0; I < NumFactTypes; ++I)
	{
		FactStorage[I].Clear();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool FWorkingMemory::IsModified()
{
	const bool bWasModified = bModified;
	bModified = false;
	return bWasModified;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
const FWorkingMemoryFact* FWorkingMemory::Find(const FWorkingMemoryQuery& Query) const
{
	const EFactType Type = Query.GetType();
	const int32 TypeIndex = static_cast<int32>(Type);
	const TArray<FWorkingMemoryFact>& Facts = FactStorage[TypeIndex].GetFacts();
	for (int32 Index = 0; Index < Facts.Num(); ++Index)
	{
		if (Facts[Index].IsValid())
		{
			if (Query.Matches(Facts[Index]))
			{
				return &Facts[Index];
			}
		}
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorkingMemory::Collect(const FWorkingMemoryQuery& Query, TArray<const FWorkingMemoryFact*>& Results) const
{
	const TArray<FWorkingMemoryFact>& Facts = FactStorage[static_cast<int32>(Query.GetType())].GetFacts();
	for (int32 Index = 0; Index < Facts.Num(); ++Index)
	{
		if (Facts[Index].IsValid())
		{
			if (Query.Matches(Facts[Index]))
			{
				Results.Add(&Facts[Index]);
			}
		}
	}
}
