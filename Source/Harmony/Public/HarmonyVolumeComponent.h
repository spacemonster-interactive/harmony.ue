#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "HarmonyVolumeComponent.generated.h"

UENUM(BlueprintType)
enum class EHarmonyVolumeShape : uint8
{
    Box UMETA(DisplayName="Box"),
    Sphere UMETA(DisplayName="Sphere")
};

UENUM(BlueprintType)
enum class EHarmonyVolumeBoundaryMode : uint8
{
    HardCull UMETA(DisplayName="Hard"),
    FeatherFade UMETA(DisplayName="Feather")
};

UENUM(BlueprintType)
enum class EHarmonyVolumeRuleMode : uint8
{
    AlwaysCull UMETA(DisplayName="Always Cull"),
    CullIfCameraDistanceLessThan UMETA(DisplayName="Cull If Camera Distance Less Than")
};

UENUM(BlueprintType)
enum class EHarmonyVolumeEffectMode : uint8
{
    AlphaCull UMETA(DisplayName="Alpha Cull"),
    DepthOffset UMETA(DisplayName="Depth Offset")
};

UENUM(BlueprintType)
enum class EHarmonyVolumeEvaluationStage : uint8
{
    Fragment UMETA(DisplayName="Fragment"),
    Preprocess UMETA(DisplayName="Preprocess")
};

UCLASS(ClassGroup=(Rendering), meta=(BlueprintSpawnableComponent), showcategories=(Transform), hidecategories=(Collision, Physics, Networking, HLOD, Cooking, DataLayers, AssetUserData, Activation, Navigation, Tags, Replication, Input, LOD))
class HARMONY_API UHarmonyVolumeComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UHarmonyVolumeComponent();

    UPROPERTY(EditAnywhere, Category="Volume", meta=(ToolTip="Fragment preserves the current per-pixel behavior. Preprocess culls splats by center before sorting and drawing, which is faster but less exact at boundaries."))
    EHarmonyVolumeEvaluationStage EvaluationStage = EHarmonyVolumeEvaluationStage::Fragment;

    UPROPERTY(EditAnywhere, Category="Volume", meta=(EditCondition="EvaluationStage == EHarmonyVolumeEvaluationStage::Fragment", EditConditionHides))
    EHarmonyVolumeEffectMode EffectMode = EHarmonyVolumeEffectMode::AlphaCull;

    UPROPERTY(EditAnywhere, Category="Volume")
    EHarmonyVolumeShape Shape = EHarmonyVolumeShape::Box;

    UPROPERTY(EditAnywhere, Category="Volume", meta=(DisplayName="Box Extents", EditCondition="Shape == EHarmonyVolumeShape::Box", EditConditionHides, ClampMin="0.0", UIMin="0.0"))
    FVector BoxExtents = FVector(100.0, 100.0, 100.0);

    UPROPERTY(EditAnywhere, Category="Volume", meta=(DisplayName="Sphere Radius", EditCondition="Shape == EHarmonyVolumeShape::Sphere", EditConditionHides, ClampMin="0.0", UIMin="0.0"))
    float SphereRadius = 100.0f;

    UPROPERTY(EditAnywhere, Category="Volume", meta=(EditCondition="EvaluationStage == EHarmonyVolumeEvaluationStage::Fragment", EditConditionHides))
    EHarmonyVolumeBoundaryMode BoundaryMode = EHarmonyVolumeBoundaryMode::HardCull;

    UPROPERTY(EditAnywhere, Category="Volume", meta=(DisplayName="Feather Distance", EditCondition="EvaluationStage == EHarmonyVolumeEvaluationStage::Fragment && BoundaryMode == EHarmonyVolumeBoundaryMode::FeatherFade", EditConditionHides, ClampMin="0.0", UIMin="0.0"))
    float FeatherDistance = 50.0f;

    UPROPERTY(EditAnywhere, Category="Volume", meta=(EditCondition="EvaluationStage == EHarmonyVolumeEvaluationStage::Preprocess || EffectMode == EHarmonyVolumeEffectMode::AlphaCull", EditConditionHides))
    EHarmonyVolumeRuleMode RuleMode = EHarmonyVolumeRuleMode::AlwaysCull;

    UPROPERTY(EditAnywhere, Category="Volume", meta=(DisplayName="Camera Distance Threshold", EditCondition="(EvaluationStage == EHarmonyVolumeEvaluationStage::Preprocess || EffectMode == EHarmonyVolumeEffectMode::AlphaCull) && RuleMode == EHarmonyVolumeRuleMode::CullIfCameraDistanceLessThan", EditConditionHides, ClampMin="0.0", UIMin="0.0"))
    float CameraDistanceThreshold = 300.0f;

    UPROPERTY(EditAnywhere, Category="Volume", meta=(DisplayName="Depth Offset Amount", EditCondition="EvaluationStage == EHarmonyVolumeEvaluationStage::Fragment && EffectMode == EHarmonyVolumeEffectMode::DepthOffset", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="1000.0"))
    float DepthOffsetAmount = 100.0f;

    virtual void OnRegister() override;
    virtual void OnUnregister() override;

    static void GetComponentsForWorld(const UWorld* World, TArray<UHarmonyVolumeComponent*>& OutComponents);
};
