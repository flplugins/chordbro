#!/usr/bin/env bash
set -euo pipefail

APP_NAME="ChordBro"
BUILD_DIR="${1:-build-mac}"
STAGE_DIR="dist/mac/${APP_NAME}"
DMG_PATH="dist/mac/${APP_NAME}-mac.dmg"

mkdir -p "${STAGE_DIR}"
cp -R "${BUILD_DIR}/${APP_NAME}_artefacts/Release/VST3/${APP_NAME}.vst3" "${STAGE_DIR}/"

if command -v create-dmg >/dev/null 2>&1; then
  create-dmg \
    --overwrite \
    --volname "${APP_NAME}" \
    "${DMG_PATH}" \
    "dist/mac"
else
  hdiutil create -volname "${APP_NAME}" -srcfolder "dist/mac" -ov -format UDZO "${DMG_PATH}"
fi

echo "DMG created at ${DMG_PATH}"
