# macOS Build And DMG Instructions

1. Install JUCE and point `JUCE_DIR` at it.
2. Configure the build:

```bash
cmake -S ripchord-main -B build-mac -G Xcode -DJUCE_DIR=/path/to/JUCE
cmake --build build-mac --config Release --target ChordBro_VST3
```

3. Expected VST3 output:

```text
build-mac/ChordBro_artefacts/Release/VST3/ChordBro.vst3
```

4. Optional AU target:

```bash
cmake --build build-mac --config Release --target ChordBro_AU
```

5. Stage the installer payload:

```bash
mkdir -p dist/mac/ChordBro
cp -R build-mac/ChordBro_artefacts/Release/VST3/ChordBro.vst3 dist/mac/ChordBro/
```

6. Build the DMG with `create-dmg` or a native `hdiutil` flow. If `create-dmg` is unavailable, use `package_mac.sh`.

7. Signing step if Developer ID credentials are available:

```bash
codesign --force --deep --sign "Developer ID Application: <TEAM>" dist/mac/ChordBro/ChordBro.vst3
```

8. Notarization step if Apple credentials are available:

```bash
xcrun notarytool submit dist/mac/ChordBro-mac.dmg --apple-id <ID> --team-id <TEAM> --password <APP_SPECIFIC_PASSWORD>
```
