#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "HarmonyVolumeActor.generated.h"

class UHarmonyVolumeComponent;
class UBoxComponent;
class USphereComponent;
struct FPropertyChangedEvent;

UCLASS(showcategories=(Transform), hidecategories=(Replication, Collision, Physics, Networking, Input, HLOD, WorldPartition, LevelInstance, Cooking, DataLayers))
class HARMONY_API AHarmonyVolumeActor : public AActor
{
    GENERATED_BODY()

public:
    AHarmonyVolumeActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Volume")
    TObjectPtr<UHarmonyVolumeComponent> VolumeComponent;

    virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    void SyncVisualizationFromVolume();

#if WITH_EDITORONLY_DATA
    UPROPERTY()
    TObjectPtr<UBoxComponent> BoxVisualizationComponent;

    UPROPERTY()
    TObjectPtr<UBoxComponent> FeatherBoxVisualizationComponent;

    UPROPERTY()
    TObjectPtr<USphereComponent> SphereVisualizationComponent;

    UPROPERTY()
    TObjectPtr<USphereComponent> FeatherSphereVisualizationComponent;
#endif
};
