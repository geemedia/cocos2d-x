#include "platform/CCPlatformConfig.h"

#ifdef CC_USE_FMOD_EX

#ifndef __AUDIO_ENGINE_FMODEX_H_
#define __AUDIO_ENGINE_FMODEX_H_

#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <fmod.hpp>
#include "audio/AudioEngineImplInterface.h"
#include "audio/include/AudioEngine.h"
#include "base/CCRef.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define MAX_AUDIOINSTANCES 24
#else
#define MAX_AUDIOINSTANCES 32
#endif

NS_CC_BEGIN
namespace experimental{

class CC_DLL AudioEngineImpl : public AudioEngineImplInterface
{
public:
    AudioEngineImpl();
    ~AudioEngineImpl();
    
    bool init();
    int play2d(const std::string& fileFullPath, bool loop, float volume, int audioID) override;
    void setVolume(int audioID, float volume) override;
    void setLoop(int audioID, bool loop) override;
    bool pause(int audioID) override;
    bool resume(int audioID) override;
    bool stop(int audioID) override;
    void stopAll() override;
    float getDuration(int audioID) override;
    float getCurrentTime(int audioID) override;
    bool setCurrentTime(int audioID, float time) override;
    void setFinishCallback(int audioID, const std::function<void (int, const std::string &)> &callback) override;
    
    void uncache(const std::string& filePath) override;
    void uncacheAll() override;
    void preload(const std::string& filePath, std::function<void(bool)> callback) override;
    
    void update(float dt);
    
    //Internal fmod callback
    void onSoundFinished(FMOD::Channel* channel);

private:
    int _preload(const std::string& filePath, std::function<void(bool)> callback);
    // returns null if a sound with the given path is not found
    FMOD::Sound* findSound(const std::string &path);

    struct ChannelInfo{
        size_t id; 
        std::string path; 
        FMOD::Sound* sound;
        FMOD::Channel* channel;
        bool loop; 
        float volume; 
        std::function<void (int, const std::string &)> callback;
    };

    std::unordered_map<std::string, FMOD::Sound*> _soundMap;
    
    std::unordered_map<int, ChannelInfo> _channelInfoMap;

    FMOD::System* _fmodSystem;

    unsigned int _bufferLength;
    int _numBuffers;
    bool _lazyInitLoop;
    int _currentAudioID;
    std::set<int> _finishedAudioIDs;
};
}
NS_CC_END
#endif // __AUDIO_ENGINE_FMODEX_H_
#endif
