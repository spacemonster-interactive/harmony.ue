#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"

#include "HarmonyPlyFactory.generated.h"

UCLASS()
class HARMONYEDITOR_API UHarmonyPlyFactory : public UFactory
{
    GENERATED_BODY()

public:
    UHarmonyPlyFactory();

    virtual bool FactoryCanImport(const FString& Filename) override;

    virtual UObject* FactoryCreateFile(
        UClass* InClass,
        UObject* InParent,
        FName InName,
        EObjectFlags Flags,
        const FString& Filename,
        const TCHAR* Parms,
        FFeedbackContext* Warn,
        bool& bOutOperationCanceled) override;
};

