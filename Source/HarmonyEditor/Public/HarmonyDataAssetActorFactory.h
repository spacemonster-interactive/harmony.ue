#pragma once

#include "ActorFactories/ActorFactory.h"

#include "HarmonyDataAssetActorFactory.generated.h"

class AActor;
class UHarmonyDataAsset;

UCLASS()
class HARMONYEDITOR_API UHarmonyDataAssetActorFactory : public UActorFactory
{
    GENERATED_BODY()

public:
    UHarmonyDataAssetActorFactory();

    virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
    virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
    virtual UObject* GetAssetFromActorInstance(AActor* ActorInstance) override;
};
