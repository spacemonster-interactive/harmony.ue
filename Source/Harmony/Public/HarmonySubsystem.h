#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"

#include "HarmonySubsystem.generated.h"

class FHarmonyViewExtension;

UCLASS()
class HARMONY_API UHarmonySubsystem : public UEngineSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

protected:
    TSharedPtr<FHarmonyViewExtension, ESPMode::ThreadSafe> HarmonyViewExtension;
};
