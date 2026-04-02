#pragma once

#include "CoreMinimal.h"

/**
 * Returns the embedded source for a given virtual shader path
 * (e.g. "/Plugin/Harmony/Private/Harmony.usf").
 *
 * Returns true and populates OutSource when the path is known.
 * Returns false when the path is not handled by this module,
 * allowing the caller to fall back to disk-based loading.
 */
HARMONYSHADERS_API bool GetHarmonyEmbeddedShaderSource(const FString& VirtualPath, FString& OutSource);
