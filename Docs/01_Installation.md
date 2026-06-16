# Installation

## Requirements

| | |
|---|---|
| **Unreal Engine** | 5.7 or later |
| **Platforms** | Windows, macOS |
| **Rendering** | SM6 (Windows) / Metal (macOS) |

Support for additional platforms and older engine versions may be added in future releases.

---

## Installing the Plugin

The plugin is installed manually — it is not distributed through the Unreal Marketplace.

1. Download or clone the **Harmony** plugin folder
2. Locate your Unreal project's root directory (the folder containing your `.uproject` file)
3. Create a `Plugins/` folder in that directory if one does not already exist
4. Copy the entire `Harmony` folder into `Plugins/`

Your project structure should look like this:

```
MyProject/
  MyProject.uproject
  Plugins/
    Harmony/
      Harmony.uplugin
      Source/
      Shaders/
      ...
```

---

## First Launch

1. Open your project in Unreal Editor (compile when prompted)
2. Go to *Edit → Plugins → Rendering* and confirm **Harmony** is enabled
3. Restart the editor if prompted

Once active, the importers register automatically. You can immediately drag supported splat files into the Content Browser.

---

## Next Steps

- [Importing Splats](02_Importing.md) — bring Gaussian Splat captures into your project
- [Scene Setup](03_SceneSetup.md) — place splats in a level
