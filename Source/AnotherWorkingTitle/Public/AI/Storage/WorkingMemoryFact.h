// (c) 2024 by Crenetic GmbH Studios

#pragma once

#include "CoreMinimal.h"
#include "AI/AIConstants.h"

#define TO_WMFLAG(_F) (1 << static_cast<uint32>(_F))

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class alignas(16) FWorkingMemoryBaseFact
{
public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual ~FWorkingMemoryBaseFact() = default;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void SetType(const EFactType InFactType) { FactType = InFactType; }
	EFactType GetType() const { return FactType; }

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void FlagAsStatic() { bIsStatic = true; }
	bool IsStatic() const { return bIsStatic; }
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FWorkingMemoryBaseFact& SetNodeType(const ENodeType InNodeType)
	{
		Set(EFactMask::NodeType);
		NodeType = InNodeType;
		return *this;
	}

	FWorkingMemoryBaseFact& SetActor(AActor* InActor)
	{
		Set(EFactMask::Actor);
		Actor = InActor;
		return *this;
	}

	FWorkingMemoryBaseFact& SetComponent(UActorComponent* InComponent)
	{
		Set(EFactMask::Component);
		Component = InComponent;
		return *this;
	}

	FWorkingMemoryBaseFact& SetObject(UObject* InObject)
	{
		Set(EFactMask::Object);
		Object = InObject;
		return *this;
	}

	FWorkingMemoryBaseFact& SetObjectClass(UClass* InObjectClass)
	{
		Set(EFactMask::ObjectClass);
		ObjectClass = InObjectClass;
		return *this;
	}

	FWorkingMemoryBaseFact& SetPosition(const FVector& InPosition)
	{
		Set(EFactMask::Position);
		Position = InPosition;
		return *this;
	}

	FWorkingMemoryBaseFact& SetName(const FName& InName)
	{
		Set(EFactMask::Name);
		Name = InName;
		return *this;
	}

	FWorkingMemoryBaseFact& SetValue(const int32 InValue)
	{
		Set(EFactMask::Value);
		Value = InValue;
		return *this;
	}

	FWorkingMemoryBaseFact& CopyFrom(const FWorkingMemoryBaseFact& Source);

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Matches(const FWorkingMemoryBaseFact& Fact) const;
	uint32 GetFlags() const { return Flags; }

protected:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FWorkingMemoryBaseFact() :
		Flags(0),
		bIsStatic(false),
		FactType(EFactType::Knowledge),
		NodeType(ENodeType::Unknown),
		Actor(nullptr),
		Component(nullptr),
		Object(nullptr),
		ObjectClass(nullptr),
		Position(0, 0, 0),
		Value(0)
	{
	}

	FWorkingMemoryBaseFact(const EFactType InFactType, const ENodeType InNodeType) :
		Flags(TO_WMFLAG(EFactMask::NodeType)),
		bIsStatic(false),
		FactType(InFactType),
		NodeType(InNodeType),
		Actor(nullptr),
		Component(nullptr),
		Object(nullptr),
		ObjectClass(nullptr),
		Position(0, 0, 0),
		Value(0)
	{
	}

	FWorkingMemoryBaseFact(const EFactType InFactType, const ENodeType InNodeType, AActor* InActor) :
		Flags(TO_WMFLAG(EFactMask::NodeType) | TO_WMFLAG(EFactMask::Actor)),
		bIsStatic(false),
		FactType(InFactType),
		NodeType(InNodeType),
		Actor(InActor),
		Component(nullptr),
		Object(nullptr),
		ObjectClass(nullptr),
		Position(0, 0, 0),
		Value(0)
	{
	}

	FWorkingMemoryBaseFact(const EFactType InFactType, const ENodeType InNodeType, UActorComponent* InComponent) :
		Flags(TO_WMFLAG(EFactMask::NodeType) | TO_WMFLAG(EFactMask::Actor)),
		bIsStatic(false),
		FactType(InFactType),
		NodeType(InNodeType),
		Actor(nullptr),
		Component(InComponent),
		Object(nullptr),
		ObjectClass(nullptr),
		Position(0, 0, 0),
		Value(0)
	{
	}

	FWorkingMemoryBaseFact(const EFactType InFactType, const ENodeType InNodeType, UObject* InObject) :
		Flags(TO_WMFLAG(EFactMask::NodeType) | TO_WMFLAG(EFactMask::Actor)),
		bIsStatic(false),
		FactType(InFactType),
		NodeType(InNodeType),
		Actor(nullptr),
		Component(nullptr),
		Object(InObject),
		ObjectClass(nullptr),
		Position(0, 0, 0),
		Value(0)
	{
	}

	FWorkingMemoryBaseFact(const EFactType InFactType, const ENodeType InNodeType, UClass* InObjectClass) :
		Flags(TO_WMFLAG(EFactMask::NodeType) | TO_WMFLAG(EFactMask::Actor)),
		bIsStatic(false),
		FactType(InFactType),
		NodeType(InNodeType),
		Actor(nullptr),
		Component(nullptr),
		Object(nullptr),
		ObjectClass(InObjectClass),
		Position(0, 0, 0),
		Value(0)
	{
	}

	FWorkingMemoryBaseFact(const EFactType InFactType, const ENodeType InNodeType, const FName& InName) :
		Flags(TO_WMFLAG(EFactMask::NodeType) | TO_WMFLAG(EFactMask::Name)),
		bIsStatic(false),
		FactType(InFactType),
		NodeType(InNodeType),
		Actor(nullptr),
		Component(nullptr),
		Object(nullptr),
		ObjectClass(nullptr),
		Position(0, 0, 0),
		Name(InName),
		Value(0)
	{
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void Set(EFactMask FactMask) { Flags |= TO_WMFLAG(FactMask); }
	bool IsSet(EFactMask FactMask) const { return IsValid(FactMask, Flags); }
	bool Matches(const uint32 CheckFlags, const FWorkingMemoryBaseFact& Fact) const;
	static bool IsValid(EFactMask FactMask, const uint32 Flag) { return (Flag & TO_WMFLAG(FactMask)) != 0; }

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	uint32 Flags;
	bool bIsStatic;

	EFactType FactType;

	ENodeType NodeType;

	AActor* Actor;
	UActorComponent* Component;
	UObject* Object;
	UClass* ObjectClass;

	FVector Position;
	FName Name;
	int32 Value;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class FWorkingMemoryQuery final : public FWorkingMemoryBaseFact
{
public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FWorkingMemoryQuery() { }
	FWorkingMemoryQuery(const EFactType InFactType, const ENodeType InNodeType) : FWorkingMemoryBaseFact(InFactType, InNodeType) { }
	FWorkingMemoryQuery(const EFactType InFactType, const ENodeType InNodeType, AActor* InActor) : FWorkingMemoryBaseFact(InFactType, InNodeType, InActor) { }
	FWorkingMemoryQuery(const EFactType InFactType, const ENodeType InNodeType, UActorComponent* InComponent) : FWorkingMemoryBaseFact(InFactType, InNodeType, InComponent) { }
	FWorkingMemoryQuery(const EFactType InFactType, const ENodeType InNodeType, UObject* InObject) : FWorkingMemoryBaseFact(InFactType, InNodeType, InObject) { }
	FWorkingMemoryQuery(const EFactType InFactType, const ENodeType InNodeType, UClass* InObjectClass) : FWorkingMemoryBaseFact(InFactType, InNodeType, InObjectClass) { }
	FWorkingMemoryQuery(const EFactType InFactType, const ENodeType InNodeType, const FName& InName) : FWorkingMemoryBaseFact(InFactType, InNodeType, InName) { }
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void SetRequiredField(EFactMask FactMask) { QueryFlags |= 1 << static_cast<uint32>(FactMask); }
	virtual bool Matches(const FWorkingMemoryBaseFact& Fact) const override;

private:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	uint32 QueryFlags = 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class FWorkingMemoryFact final : public FWorkingMemoryBaseFact
{
public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FWorkingMemoryFact() { }
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	ENodeType GetNodeType() const { return NodeType; }
	AActor* GetActor() const { return Actor; }
	UActorComponent* GetComponent() const { return Component; }
	UObject* GetObject() const { return Object; }
	const FVector& GetPosition() const { return Position; }
	const FName& GetName() const { return Name; }
	int32 GetValue() const { return Value; }

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	bool IsValid() const { return Flags != 0; }
	bool IsValid(const EFactMask FactMask) const { return FWorkingMemoryBaseFact::IsValid(FactMask, Flags); }
	bool IsValid(const uint32 InFlag) const { return (Flags & InFlag) != 0; }

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void Invalidate() { check(!bIsStatic); Flags = 0; }
};
