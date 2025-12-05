#pragma once

#include <core/rendering/image.hpp>
#include <core/objects/basic.hpp>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <SDL3_mixer/SDL_mixer.h>

class Asset : public Core::Rendering::Image {
public:
    Asset(std::string path);
    Asset() = default;
    ~Asset();

    std::string path;
};

class Animation {
public:
    std::vector<Asset*> frames;
    int currentFrame;
    float currentSubframe;
    bool isPlaying;
    Animation();
    void add(Asset *asset);
};

namespace Assets {
extern MIX_Mixer* mixer;
extern Asset* assetList[1151];
void initAudio();
void playSound(const std::string& name, int loops);
void stopAudio(const std::string& name);
void loadAllAssets();
void unloadAllAssets();
void updateAudio();

extern std::unordered_map<std::string, MIX_Audio*> soundMap;
extern std::unordered_map<std::string, MIX_Track*> audioTracks;
extern std::unordered_map<std::string, MIX_Track*> loopingTracks;
}