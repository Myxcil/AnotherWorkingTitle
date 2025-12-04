// (c) 2024 by Crenetic GmbH Studios

#pragma once

class UAISettings;
class UAIManager;
class IAgent;

ANOTHERWORKINGTITLE_API DECLARE_LOG_CATEGORY_EXTERN(LogGOAP, Log, All);

#if NO_LOGGING
#define AI_LOG(Format, ...)
#define AI_WARN(Format, ...)
#else
#define AI_LOG(Format, ...) UE_PRIVATE_LOG(PREPROCESSOR_NOTHING, constexpr, LogGOAP, Log, Format, ##__VA_ARGS__)
#define AI_WARN(Format, ...) UE_PRIVATE_LOG(PREPROCESSOR_NOTHING, constexpr, LogGOAP, Warning, Format, ##__VA_ARGS__)
#endif

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class EFactType : uint8
{
	Knowledge,
	
	Count
};
constexpr int32 NumFactTypes = static_cast<int32>(EFactType::Count);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class EWorldPropertyKey : uint8
{
	Wait,

	AtNode,
	PathExists,

	HasResource,
	Transfer,
		
	Construct,
	Need,

	Count,
};
constexpr int32 WorldPropertyKeyCount = static_cast<int32>(EWorldPropertyKey::Count);
FString GetWorldPropertyKeyName(const EWorldPropertyKey Key);

#define AI_ENUM_TO_FLAG(_f) (1 << static_cast<uint32>(_f))

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class EWorldPropertyType : uint8
{
	Unknown,
	
	Int,
	Bool,
	
	Node,
	NeedType,
	ResourceCategory,
	
	WorldPropertyKey,

	Count,
};
constexpr int32 WorldPropertyTypeCount = static_cast<int32>(EWorldPropertyType::Count);
FString GetWorldPropertyTypeName(EWorldPropertyType Type);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class EFactMask : uint8
{
	NodeType,
	Actor,
	Component,
	Object,
	ObjectClass,
	Position,
	Name,
	Value,
	
	Count,
};
constexpr int32 FactMaskCount = static_cast<int32>(EFactMask::Count);
FString GetFactMaskName(EFactMask FactMask);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ENodeType : uint8
{
	Unknown,
	
	ResourceNode,
	Stockpile,
	BuildingSite,
	NeedsModifier,
	NeedInteraction,
	
	Query,
	
	Count,
};
constexpr int32 NumNodeTypes = static_cast<int32>(ENodeType::Count);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class EBlackboardMask : uint8
{
	ResourceNode,
	Stockpile,
	BuildingSite,
	NeedsModifier,
	NeedInteraction,
};
FString GetBlackboardMaskName(EBlackboardMask BlackboardMask);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class EAgentMood : uint8
{
	Neutral,
	Content,
	Optimistic,
	Stressed,
	Afraid,
	Angry,
	Sad
};
ENUM_RANGE_BY_FIRST_AND_LAST(EAgentMood, EAgentMood::Neutral, EAgentMood::Sad);

#define MOOD_1(M0) AI_ENUM_TO_FLAG(M0)
#define MOOD_2(M0,M1) (MOOD_1(M0) | AI_ENUM_TO_FLAG(M1))
#define MOOD_3(M0,M1,M2) (MOOD_2(M0,M1) | AI_ENUM_TO_FLAG(M2))
#define MOOD_4(M0,M1,M2,M3) (MOOD_3(M0,M1,M2) | AI_ENUM_TO_FLAG(M3))