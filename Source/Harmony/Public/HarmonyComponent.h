#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "HarmonyComponent.generated.h"

class UHarmonyDataAsset;

UENUM(BlueprintType)
enum class EHarmonySplatDepthOffsetMode : uint8
{
    SimpleViewSpace UMETA(DisplayName="Simple View-Space Offset"),
    GroundWeighted UMETA(DisplayName="Ground-Weighted Offset")
};

USTRUCT(BlueprintType)
struct HARMONY_API FHarmonyAlphaFadeSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Near Fade", meta=(DisplayName="Camera Near Fade", ClampMin="0.0", UIMin="0.0", UIMax="5000.0"))
    float CameraNearFade = 100.0f;

    UPROPERTY(EditAnywhere, Category="Near Fade", meta=(DisplayName="Camera Near Fade Feather", ClampMin="0.0", UIMin="0.0", UIMax="5000.0"))
    float CameraNearFadeFeather = 200.0f;

    UPROPERTY(EditAnywhere, Category="Near Fade", meta=(DisplayName="Ground Alpha Fade Position", UIMin="-100000.0", UIMax="100000.0"))
    float GroundAlphaFadePosition = 0.0f;

    UPROPERTY(EditAnywhere, Category="Near Fade", meta=(DisplayName="Ground Alpha Fade Feather", ClampMin="0.0", UIMin="0.0", UIMax="5000.0"))
    float GroundAlphaFadeFeather = 0.0f;
};

USTRUCT(BlueprintType)
struct HARMONY_API FHarmonyDepthOffsetSettings
{
    GENERATED_BODY()

    // Selects how SplatDepthOffset is applied.
    UPROPERTY(EditAnywhere, Category="Depth Offset", meta=(DisplayName="Mode"))
    EHarmonySplatDepthOffsetMode SplatDepthOffsetMode = EHarmonySplatDepthOffsetMode::GroundWeighted;

    // Offsets this Harmony actor in view-space depth.
    // Positive values push splats farther away; negative values pull splats closer.
    // In Ground-Weighted mode this is the maximum offset applied near the ground, fading out with height.
    UPROPERTY(EditAnywhere, Category="Depth Offset", meta=(DisplayName="Splat Depth Offset", UIMin="-500.0", UIMax="500.0"))
    float SplatDepthOffset = 200.0f;

    // Local-space Z reference where Ground-Weighted depth offset is applied at full strength.
    UPROPERTY(EditAnywhere, Category="Depth Offset", meta=(DisplayName="Ground Offset Base Local Z", EditCondition="SplatDepthOffsetMode == EHarmonySplatDepthOffsetMode::GroundWeighted", EditConditionHides, UIMin="-100000.0", UIMax="100000.0"))
    float GroundDepthOffsetBaseLocalZ = 40.0f;

    // World-space height range over which Ground-Weighted depth offset fades to zero above Ground Offset Base Local Z.
    UPROPERTY(EditAnywhere, Category="Depth Offset", meta=(DisplayName="Ground Offset Fade Height", EditCondition="SplatDepthOffsetMode == EHarmonySplatDepthOffsetMode::GroundWeighted", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="2000.0"))
    float GroundDepthOffsetFadeHeight = 60.0f;
};

UCLASS(ClassGroup=(Rendering), meta=(BlueprintSpawnableComponent), showcategories=(Transform), hidecategories=(Collision, Physics, Networking, HLOD, Cooking, DataLayers, AssetUserData, Activation, Navigation, Tags, Replication, Input, LOD))
class HARMONY_API UHarmonyComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UHarmonyComponent();

    UPROPERTY(EditAnywhere, Category="Gaussian Splat")
    TSoftObjectPtr<UHarmonyDataAsset> SplatDataAsset;

    // Multiplies the final splat alpha for this Harmony actor after preprocess opacity is evaluated.
    // Values above 1 make the actor denser; values below 1 make it more transparent.
    UPROPERTY(EditAnywhere, Category="Gaussian Splat", meta=(ClampMin="0.0", UIMin="0.0", UIMax="4.0"))
    float OpacityScale = 1.0f;

    // Multiplies the projected splat size for this Harmony actor.
    // Values above 1 make splats larger on screen; values below 1 make them smaller.
    UPROPERTY(EditAnywhere, Category="Gaussian Splat", meta=(ClampMin="0.0", UIMin="0.0", UIMax="4.0"))
    float SplatScale = 1.0f;

    UPROPERTY(EditAnywhere, Category="Gaussian Splat", meta=(DisplayName="Near Fade"))
    FHarmonyAlphaFadeSettings AlphaFadeSettings;

    UPROPERTY(EditAnywhere, Category="Gaussian Splat", meta=(DisplayName="Depth Offset"))
    FHarmonyDepthOffsetSettings DepthOffsetSettings;

    // Allows this component's background splats to contribute to Harmony's background depth estimate and SceneDepth write.
    // This currently affects the main splat layer.
    UPROPERTY(EditAnywhere, Category="Gaussian Splat", meta=(DisplayName="Write Depth To Scene"))
    bool bWriteDepthToScene = true;

    virtual void OnRegister() override;
    virtual void OnUnregister() override;

    static void GetComponentsForWorld(const UWorld* World, TArray<UHarmonyComponent*>& OutComponents);

    bool GetEnabled() const { return bEnabled; }
    void SetEnabled(bool bInEnabled);

private:
    UPROPERTY()
    bool bEnabled = true;
};
