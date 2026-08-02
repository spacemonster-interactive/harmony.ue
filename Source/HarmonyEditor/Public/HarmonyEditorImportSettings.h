#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "HarmonyEditorImportSettings.generated.h"

UCLASS(Config=Editor, DefaultConfig, meta=(DisplayName="Harmony Import"))
class HARMONYEDITOR_API UHarmonyEditorImportSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    virtual FName GetCategoryName() const override;
    virtual FText GetSectionText() const override;
    virtual FText GetSectionDescription() const override;
#endif

    UPROPERTY(Config, EditAnywhere, Category="Compression", meta=(DisplayName="Import Compressed", ToolTip="When enabled, expanded source formats such as PLY, SPLAT, SPZ, and KSPLAT are automatically imported into Harmony's compressed DataAsset layout when compression is supported."))
    bool bImportCompressed = true;

    UPROPERTY(Config, EditAnywhere, Category="Decimation", meta=(DisplayName="Decimation Target Ratio", ClampMin="0.01", ClampMax="1.0", UIMin="0.01", UIMax="1.0", ToolTip="Fraction of splats to keep on import (e.g. 0.5 keeps half). 1.0 disables import-time decimation. Runs before compression, so the imported asset ends up at this reduced count regardless of the Import Compressed setting."))
    float DecimationTargetRatio = 1.0f;
};
