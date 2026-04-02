#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "HarmonyActor.generated.h"

class UHarmonyComponent;

UCLASS(showcategories=(Transform), hidecategories=(Replication, Collision, Physics, Networking, Input, HLOD, WorldPartition, LevelInstance, Cooking, DataLayers))
class HARMONY_API AHarmonyActor : public AActor
{
    GENERATED_BODY()

public:
    AHarmonyActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gaussian Splat")
    TObjectPtr<UHarmonyComponent> GaussianSplatComponent;

    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void SetActorHiddenInGame(bool bNewHidden) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    void SyncComponentVisibilityFromActor();
};
