// (c) 2024 by Crenetic GmbH Studios

#pragma once

#include "CoreMinimal.h"
#include "ComponentVisualizer.h"

/**
 * 
 */
class ANOTHERWORKINGTITLEEDITOR_API FAIDebugComponentVisualizer : public FComponentVisualizer
{
public:
	FAIDebugComponentVisualizer();
	virtual ~FAIDebugComponentVisualizer() override;

	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
};
