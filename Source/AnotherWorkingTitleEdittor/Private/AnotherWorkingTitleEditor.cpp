#include "AnotherWorkingTitleEditor.h"

#include "AIDebugComponentVisualizer.h"
#include "UnrealEdGlobals.h"
#include "AI/GOAPAgentComponent.h"
#include "Editor/UnrealEdEngine.h"

#define LOCTEXT_NAMESPACE "FAnotherWorkingTitleEdittorModule"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FAnotherWorkingTitleEditorModule::StartupModule()
{
	RegisterComponentVisualizer(UGOAPAgentComponent::StaticClass()->GetFName(), MakeShareable(new FAIDebugComponentVisualizer));
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FAnotherWorkingTitleEditorModule::RegisterComponentVisualizer(const FName& ComponentClassName, const TSharedPtr<FComponentVisualizer>& Visualizer)
{
	if (GUnrealEd)
	{
		GUnrealEd->RegisterComponentVisualizer(ComponentClassName, Visualizer);
	}
	RegisteredComponentClassNames.Add(ComponentClassName);
	if (Visualizer.IsValid())
	{
		Visualizer->OnRegister();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FAnotherWorkingTitleEditorModule::ShutdownModule()
{
	// Ensure the editor engine is available before attempting to unregister visualizers.
	if (GUnrealEd)
	{
		// Iterate over all class names we registered for
		for (const FName ClassName : RegisteredComponentClassNames)
		{
			GUnrealEd->UnregisterComponentVisualizer(ClassName);
		}
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FAnotherWorkingTitleEditorModule, AnotherWorkingTitleEditor)