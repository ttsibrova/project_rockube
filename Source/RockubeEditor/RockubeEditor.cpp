// Fill out your copyright notice in the Description page of Project Settings.

#include "RockubeEditor.h"
#include "Modules/ModuleManager.h"
#include "RockubeEditor/Public/TrajectoryProviderComponentVisualizer.h"

IMPLEMENT_GAME_MODULE (FRockubeEditorModule, RockubeEditor);

void FRockubeEditorModule::StartupModule()
{
    if (GUnrealEd != NULL)
    {
        TSharedPtr<FTrajectoryProviderComponentVisualizer> Visualizer = MakeShareable (new FTrajectoryProviderComponentVisualizer);

        if (Visualizer.IsValid ())
        {
            GUnrealEd->RegisterComponentVisualizer (UTrajectoryProviderComponent::StaticClass()->GetFName(), Visualizer);
            Visualizer->OnRegister ();
        }
    }
}

void FRockubeEditorModule::ShutdownModule()
{
    if (GUnrealEd != NULL)
    {
        GUnrealEd->UnregisterComponentVisualizer (UTrajectoryProviderComponent::StaticClass()->GetFName());
    }
}
