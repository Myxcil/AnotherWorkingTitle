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
enum class ENodeType : uint8
{
	Unknown,
	Actor,
	Component,
	Object,
	ObjectClass,
	FreePosition,
	
	Count,
};
constexpr int32 NumNodeTypes = static_cast<int32>(ENodeType::Count);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class EWorldPropertyKey : uint8
{
	Wait,

	AtNode,
	PathExists,

	Harvest,
	Transfer,
	Construct,
	
	Count,
};
constexpr int32 WorldPropertyKeyCount = static_cast<int32>(EWorldPropertyKey::Count);
FString GetWorldPropertyKeyName(const EWorldPropertyKey Key);

#define WORLD_PROPERTY_KEY_FLAG(_f) (1 << static_cast<uint32>(_f))

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class EWorldPropertyType : uint8
{
	Unknown,
	Int,
	Bool,
	Node,
	Object,
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
