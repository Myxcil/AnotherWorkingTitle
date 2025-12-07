#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FComponentVisualizer;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
class FAnotherWorkingTitleEdittorModule : public IModuleInterface
{
public:
    //--------------------------------------------------------------------------------------------------------------------------------------------------------
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    
private:
    //--------------------------------------------------------------------------------------------------------------------------------------------------------
    void RegisterComponentVisualizer(const FName& ComponentClassName, const TSharedPtr<FComponentVisualizer>& Visualizer);

    //--------------------------------------------------------------------------------------------------------------------------------------------------------
    TArray<FName> RegisteredComponentClassNames;
};
