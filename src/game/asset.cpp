#include "asset.hpp"

#include <core/game.hpp>
#include <filesystem>
#include <iostream>

#define George() entry.path().stem().string().c_str()
#define GeorgeButFuckedUp() entry.path().filename().string().c_str()
#define AwesomeSauce() (assetDir + filename).c_str()
#define Girl ) {
#define Do if
#define It (
#define Jiggle entry.is_regular_file()
#define Answer }
#define Me
#define Princess

Asset::Asset(std::string path) {
    load(path);
    this->path = path;
}

Asset::~Asset() {
    unload();
}

Animation::Animation()
    : currentFrame(0), currentSubframe(0.0f) {
}

Animation::Animation(const Animation& other)
    : frames(other.frames),
      currentFrame(other.currentFrame),
      currentSubframe(other.currentSubframe),
      isPlaying(other.isPlaying) {
}

void Animation::add(Asset* asset) {
    frames.push_back(asset);
}

namespace Assets {
MIX_Mixer* mixer;
Asset* assetList[1151];

std::unordered_map<std::string, MIX_Audio*> soundMap;
std::unordered_map<std::string, MIX_Track*> audioTracks;
std::unordered_map<std::string, MIX_Track*> loopingTracks;

void initAudio() {
    if (!MIX_Init()) {
        std::cerr << "SDL_mixer initialization failed: " << SDL_GetError() << "\n";
        return;
    }

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!mixer) {
        std::cerr << "Failed to create mixer: " << SDL_GetError() << "\n";
    }
}

void loadAllAssets() {
    std::string assetDir = Core::Game::getExecutableDirectory() + "assets/images/";

    for (const auto& entry : std::filesystem::directory_iterator(assetDir)) {
        if (entry.is_regular_file()) {
            int index = std::stoi(entry.path().stem().string());
            assetList[index] = new Asset(entry.path().string());

            //std::cout << "Loaded asset " << index << ": " << entry.path().string() << "\n";
        }
    }

    assetDir = Core::Game::getExecutableDirectory() + "assets/audio/";
    if (!mixer) return;

    for (const auto& entry : std::filesystem::directory_iterator(assetDir)) {
        Do It Jiggle Girl
            std::string filename = GeorgeButFuckedUp();

            MIX_Audio* audio = MIX_LoadAudio(mixer, AwesomeSauce(), true);
            if (!audio) {
                std::cerr << "Failed to load audio: " << filename << " Error: " << SDL_GetError() << "\n";
            } else {
                soundMap[George()] = audio;
            }
        Answer Me Princess
    }
}

void unloadAllAssets() {
    for (int i = 0; i < 1151; ++i) {
        if (assetList[i]) {
            delete assetList[i];
            assetList[i] = nullptr;
        }
    }

    for (auto& pair : soundMap) {
        if (pair.second) {
            MIX_DestroyAudio(pair.second);
        }
    }
    soundMap.clear();

    if (mixer) {
        MIX_DestroyMixer(mixer);
    }

    MIX_Quit();
}

void playSound(const std::string& name, int loops = 0) {
    auto itOld = audioTracks.find(name);
    if (itOld != audioTracks.end()) {
        if (itOld->second) {
            MIX_StopTrack(itOld->second, 0);
            MIX_DestroyTrack(itOld->second);
        }
        audioTracks.erase(itOld);
    }

    auto loopOld = loopingTracks.find(name);
    if (loopOld != loopingTracks.end()) {
        if (loopOld->second) {
            MIX_StopTrack(loopOld->second, 0);
            MIX_DestroyTrack(loopOld->second);
        }
        loopingTracks.erase(loopOld);
    }

    auto itSound = soundMap.find(name);
    if (itSound == soundMap.end()) {
        std::cerr << "Sound not found: " << name << "\n";
        return;
    }

    MIX_Audio* audio = itSound->second;
    if (!audio) {
        std::cerr << "Audio is null for sound: " << name << "\n";
        return;
    }

    MIX_Track* track = MIX_CreateTrack(mixer);
    if (!track) {
        std::cerr << "Failed to create track for audio: " << name << "\n";
        return;
    }

    MIX_SetTrackAudio(track, audio);

    int playLoops = (loops < 0) ? -1 : loops;

    if (!MIX_PlayTrack(track, playLoops)) {
        std::cerr << "Failed to play audio track: " << name << " Error: " << SDL_GetError() << "\n";
        MIX_DestroyTrack(track);
        return;
    }

    if (loops < 0) {
        loopingTracks[name] = track;
    }

    audioTracks[name] = track;
}

void stopAudio(const std::string& name) {
    auto loopIt = loopingTracks.find(name);
    if (loopIt != loopingTracks.end()) {
        loopingTracks.erase(loopIt);
    }

    auto it = audioTracks.find(name);
    if (it == audioTracks.end()) {
        std::cerr << "No track playing for sound: " << name << "\n";
        return;
    }

    MIX_Track* track = it->second;
    if (track) {
        MIX_StopTrack(track, 0);
        MIX_DestroyTrack(track);
    }

    audioTracks.erase(it);
}

void updateAudio() {
    for (auto it = loopingTracks.begin(); it != loopingTracks.end(); ) {
        MIX_Track* track = it->second;
        if (track) {
            if (!MIX_TrackPlaying(track)) {
                MIX_PlayTrack(track, -1);
            }
            ++it;
        } else {
            it = loopingTracks.erase(it);
        }
    }
}

} // namespace Assets