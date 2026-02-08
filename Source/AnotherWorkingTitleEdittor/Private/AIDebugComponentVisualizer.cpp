// (c) 2024 by Crenetic GmbH Studios


#include "AIDebugComponentVisualizer.h"

#include "CanvasTypes.h"
#include "AI/GOAPAgentComponent.h"
#include "AI/Actions/AbstractAction.h"
#include "AI/Goals/AbstractGoal.h"

class UGOAPAgentComponent;

namespace
{
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	FVector Project(const FSceneView* View, const FCanvas* Canvas, const FVector& Location)
	{
		if (View != nullptr && Canvas != nullptr)
		{
			Location.DiagnosticCheckNaN();
			const FPlane V = View->Project(Location);

			const FIntRect CanvasRect = Canvas->GetViewRect();
			const FVector::FReal HalfWidth = CanvasRect.Width() * 0.5;
			const FVector::FReal HalfHeight = CanvasRect.Height() * 0.5;

			return FVector(FMath::FloorToFloat((1 + V.X) * HalfWidth), FMath::FloorToFloat((1 - V.Y) * HalfHeight), V.Z);
		}
	
		return FVector::ZeroVector;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	const TCHAR* AIDebugName = TEXT("GoapDebug");
	[[maybe_unused]] TCustomShowFlag<> ShowAIDebug(AIDebugName, true, SFG_Developer, FText::FromString("GOAP Debug"));

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool IsFlagSet(const FSceneView* View, const TCHAR* FlagName)
	{
		const uint32 ShowFlagIndex = static_cast<uint32>(FEngineShowFlags::FindIndexByName(FlagName));
		return View && View->Family && View->Family->EngineShowFlags.GetSingleFlag(ShowFlagIndex); 
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FAIDebugComponentVisualizer::FAIDebugComponentVisualizer()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FAIDebugComponentVisualizer::~FAIDebugComponentVisualizer()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FAIDebugComponentVisualizer::DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	if (!IsFlagSet(View, AIDebugName))
		return;

	const UGOAPAgentComponent* GOAPAgent = Cast<UGOAPAgentComponent>(Component);
	if (!GOAPAgent)
		return;

	if (!GOAPAgent->GetWorld()->IsPlayInEditor())
		return;

	const FVector WorldPos = GOAPAgent->GetOwner()->GetActorLocation() + FVector::UpVector * 200.0f;
	const float ViewDistance = FVector::Distance(WorldPos, View->ViewLocation);
	if (ViewDistance >= 10000)
		return;

	UFont* SmallFont = GEngine->GetSmallFont();
	const FVector ScreenPos = Project(View, Canvas, WorldPos);
	if (ScreenPos.Z <= 0)
		return;

	FString OutputString = GOAPAgent->GetOwner()->GetName();

	int32 LineCount = 1;
	if (const UAbstractGoal* Goal = GOAPAgent->CurrentGoal.Get())
	{
		OutputString += TEXT("\n\n");
		OutputString += Goal->GetTypeName();
		LineCount += 2;
		for (int32 I=0; I < GOAPAgent->Plan.Num(); ++I)
		{
			OutputString += GOAPAgent->PlanStep == I ? TEXT("\n+") : TEXT("\n ");
			OutputString += GOAPAgent->Plan[I].Action->GetTypeName();
			LineCount += 2;
		}
	}

	int32 XL,YL;
	StringSize(SmallFont, XL, YL, OutputString);
	YL *= LineCount;

	XL = 170;
	YL = LineCount * 12;

	Canvas->DrawTile(ScreenPos.X - 0.5f * XL - 5, ScreenPos.Y - YL - 5, XL+10, YL+10, 0, 0, 0, 0, FLinearColor::Black);

	constexpr FLinearColor Color = FLinearColor(0.5f, 1.0f, 0.5f); 
	Canvas->DrawShadowedString(ScreenPos.X - 0.5f * XL, ScreenPos.Y - YL, OutputString, SmallFont, Color);
}
