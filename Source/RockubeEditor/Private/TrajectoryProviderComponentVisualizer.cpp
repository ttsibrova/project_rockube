// Fill out your copyright notice in the Description page of Project Settings.


#include "TrajectoryProviderComponentVisualizer.h"

IMPLEMENT_HIT_PROXY (HTrajectoryProviderVisProxy, HComponentVisProxy)

void FTrajectoryProviderComponentVisualizer::DrawVisualization (const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
    const UTrajectoryProviderComponent* TrajectoryProvider = Cast<UTrajectoryProviderComponent> (Component);
    if (TrajectoryProvider) {
        PDI->SetHitProxy (new HTrajectoryProviderVisProxy (Component));
        PDI->DrawLine (TrajectoryProvider->GetTrajectoryStartLocation(), TrajectoryProvider->GetTrajectoryEndLocation(), FColor::Red, SDPG_Foreground);
        PDI->DrawPoint (TrajectoryProvider->GetTrajectoryEndLocation(), FColor::Green, 20.f, SDPG_Foreground);
        PDI->SetHitProxy (NULL);
    }
}

bool FTrajectoryProviderComponentVisualizer::VisProxyHandleClick (FEditorViewportClient* InViewportClient,
                                                                  HComponentVisProxy* VisProxy,
                                                                  const FViewportClick& Click)
{
    bool bHandle = false;
    if (VisProxy) {
        auto TrajectoryVisProxy = HitProxyCast <HTrajectoryProviderVisProxy> (VisProxy);
        if (TrajectoryVisProxy) {
            myEditedComponent = FComponentPropertyPath (TrajectoryVisProxy->Component.Get()).GetComponent();
            bHandle = true;
        }
    }
    return bHandle;
}

bool FTrajectoryProviderComponentVisualizer::GetWidgetLocation (const FEditorViewportClient* ViewportClient, FVector& OutLocation) const
{
    auto anEditetComp = GetEditedComponent();
    const UTrajectoryProviderComponent* EditedComponent = Cast<UTrajectoryProviderComponent> (anEditetComp);
    if (EditedComponent && EditedComponent->TrajectoryType == ETrajectoryType::TRAJECTORY_SINGLE ||
        EditedComponent->TrajectoryType == ETrajectoryType::TRAJECTORY_FIXED) {
        OutLocation = EditedComponent->GetTrajectoryEndLocation();
        return true;
    }
    return false;
}

bool FTrajectoryProviderComponentVisualizer::HandleInputDelta (FEditorViewportClient* ViewportClient,
                                                               FViewport* Viewport,
                                                               FVector& DeltaTranslate,
                                                               FRotator& DeltaRotate,
                                                               FVector& DeltaScale)
{
    bool bHandled = false;
    UTrajectoryProviderComponent* EditedComponent = Cast<UTrajectoryProviderComponent> (GetEditedComponent());
    if (EditedComponent && EditedComponent->TrajectoryType == ETrajectoryType::TRAJECTORY_SINGLE ||
        EditedComponent->TrajectoryType == ETrajectoryType::TRAJECTORY_FIXED) {
        EditedComponent->SetTargetLocation (EditedComponent->TargetLocation + DeltaTranslate);
        return true;
    }
    return false;
}

UActorComponent* FTrajectoryProviderComponentVisualizer::GetEditedComponent() const
{
    return myEditedComponent;
}

void FTrajectoryProviderComponentVisualizer::EndEditing()
{
    myEditedComponent = nullptr;
}

