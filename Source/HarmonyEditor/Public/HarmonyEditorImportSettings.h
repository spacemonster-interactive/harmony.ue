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
};
