// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Settlers/Needs.h"
#include "AWTHelperFunctions.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UAWTHelperFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintPure)
	static FText GetNeedsDisplayName(const ENeedType NeedType);

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename E>
	static FString GetEnumValueName(E EnumValue)
	{
		const UEnum* Enum = StaticEnum<E>();
		const FString FullValueName = Enum->GetNameStringByValue(static_cast<int64>(EnumValue));
		//const int32 EnumNameLength = Enum->GetName().Len();
		//return FullValueName.RightChop(EnumNameLength+2);
		return FullValueName;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category="Game Config")
	static FString GetAppVersion();
};
