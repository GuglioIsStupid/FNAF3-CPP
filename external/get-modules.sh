#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

for tool in wget unzip git; do
    if ! command -v "$tool" &>/dev/null; then
        echo "Error: $tool is not installed. Please install it to proceed."
        exit 1
    fi
done

MODULES_LIST=$(cat <<'EOF'
dr_mp3.h|https://github.com/mackron/dr_libs/raw/refs/heads/master/dr_mp3.h
dr_wav.h|https://github.com/mackron/dr_libs/raw/refs/heads/master/dr_wav.h
easings.zip|https://github.com/nicolausYes/easing-functions/archive/refs/heads/master.zip
glm.zip|https://github.com/g-truc/glm/archive/a583c59e1616a628b18195869767ea4d6faca5f4.zip
imgui.zip|https://github.com/ocornut/imgui/archive/refs/heads/master.zip
miniz.zip|https://github.com/richgel999/miniz/releases/download/3.1.0/miniz-3.1.0.zip
sdl3.zip|https://github.com/libsdl-org/SDL/archive/refs/heads/main.zip
sdl_image.zip|https://github.com/libsdl-org/SDL_image/archive/refs/heads/main.zip
sdl_mixer.zip|https://github.com/libsdl-org/SDL_mixer/archive/refs/heads/main.zip
harfbuzz.zip|https://github.com/harfbuzz/harfbuzz/archive/refs/heads/main.zip
freetype.zip|https://github.com/freetype/freetype/archive/refs/heads/master.zip
stb_vorbis.c|https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_vorbis.c
EOF
)

echo "Starting download of external modules..."
while IFS='|' read -r FILE URL; do
    [ -z "$FILE" ] && continue
    if [ -f "$FILE" ]; then
        echo "$FILE already exists, skipping."
    else
        echo "Downloading $FILE ..."
        wget -q -O "$FILE" --header="User-Agent: Mozilla/5.0" "$URL" || echo "Failed to download $FILE"
        
    fi
done <<< "$MODULES_LIST"

extract_module() {
    local zip="$1"
    local pattern="$2"
    local target="$3"

    if [ ! -d "$target" ]; then
        echo "Extracting $zip ..."
        unzip -oq "$zip"
        local extracted
        extracted=$(find . -maxdepth 1 -type d -name "$pattern" | head -n1 || true)
        if [ -n "$extracted" ]; then
            mv "$extracted" "$target"
        else
            echo "Warning: could not find extracted folder for $zip matching $pattern"
        fi
    fi
}

mkdir -p dr_libs stb

echo
echo "Extracting downloaded modules..."
mv -f dr_mp3.h dr_libs/ 2>/dev/null || true
mv -f dr_wav.h dr_libs/ 2>/dev/null || true
extract_module "easings.zip" "easing-functions-*" "easings"
extract_module "glm.zip" "glm-*" "glm"
extract_module "imgui.zip" "imgui-*" "imgui"

if [ ! -d "miniz" ]; then
    mkdir -p miniz
    temp_dir=$(mktemp -d)
    unzip -q "miniz.zip" -d "$temp_dir"
    find "$temp_dir" -type f \( -name "miniz.h" -o -name "miniz.c" \) -exec mv {} miniz/ \;
    rm -rf "$temp_dir"
fi
extract_module "harfbuzz.zip" "harfbuzz-*" "harfbuzz"
extract_module "freetype.zip" "freetype-*" "freetype"
extract_module "rapidjson.zip" "rapidjson-*" "rapidjson"
extract_module "sdl3.zip" "SDL-*" "SDL"
extract_module "sdl_image.zip" "SDL_image-*" "SDL_image"
extract_module "sdl_mixer.zip" "SDL_mixer-*" "SDL_mixer"
mv -f "stb_vorbis.c" "stb/"

echo
echo "Cleaning up temporary files..."
rm -f LICENSE README.md ChangeLog.md miniz.{c,h} || true
rm -rf examples

echo
echo "Fetching SDL external dependencies..."
set +e
bash "SDL_image/external/download.sh"
bash "SDL_mixer/external/download.sh"
set -e

echo
echo "All external modules downloaded and extracted successfully!"
