// (c) 2025 MK


#include "AWTHelperFunctions.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FText UAWTHelperFunctions::GetNeedsDisplayName(const ENeedType NeedType)
{
	switch (NeedType)
	{
	case ENeedType::Hunger:
		return NSLOCTEXT("Needs", "Need_Hunger", "Hunger");
	case ENeedType::Thirst:
		return NSLOCTEXT("Needs", "Need_Thirst", "Durst");
	case ENeedType::Cold:
		return NSLOCTEXT("Needs", "Need_Cold", "Kälte");
	case ENeedType::Fatigue:
		return NSLOCTEXT("Needs", "Need_Fatigue", "Müdigkeit");
	case ENeedType::Damage:
		return NSLOCTEXT("Needs", "Need_Damage", "Schaden");
	}
	return FText::GetEmpty();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FString UAWTHelperFunctions::GetAppVersion()
{
	FString AppVersion;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		AppVersion,
		GGameIni
	);
	return AppVersion;
}
