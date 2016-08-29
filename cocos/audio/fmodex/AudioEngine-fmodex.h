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
    int play2d(const std::string &fileFullPath, bool loop, float volume, int audioId);
    void setVolume(int audioID, float volume);
    void setLoop(int audioID, bool loop);
    bool pause(int audioID);
    bool resume(int audioID);
    bool stop(int audioID);
    void stopAll();
    float getDuration(int audioID);
    float getCurrentTime(int audioID);
    bool setCurrentTime(int audioID, float time);
    void setFinishCallback(int audioID, const std::function<void (int, const std::string &)> &callback);
    
    void uncache(const std::string& filePath);
    void uncacheAll();
    void preload(const std::string& filePath, std::function<void(bool)> callback);
    
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
