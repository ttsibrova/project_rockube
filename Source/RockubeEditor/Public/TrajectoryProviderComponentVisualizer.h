// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComponentVisualizer.h"
#include "Movement/TrajectoryProviderComponent.h"

struct HTrajectoryProviderVisProxy: HComponentVisProxy
{
    DECLARE_HIT_PROXY();
    HTrajectoryProviderVisProxy (const UActorComponent* InComponent)
        : HComponentVisProxy (InComponent, HPP_Wireframe) {}

};




class ROCKUBEEDITOR_API FTrajectoryProviderComponentVisualizer : public FComponentVisualizer
{
public:
    void DrawVisualization (const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
    bool VisProxyHandleClick (FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click) override;
    bool GetWidgetLocation (const FEditorViewportClient* ViewportClient, FVector& OutLocation) const override;
    bool HandleInputDelta (FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale);
    UActorComponent* GetEditedComponent() const override;
    void EndEditing() override;

    UActorComponent* myEditedComponent;
};
