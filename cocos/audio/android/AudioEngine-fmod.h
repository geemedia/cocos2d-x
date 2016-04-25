#include "platform/CCPlatformConfig.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#ifndef __AUDIO_ENGINE_ANDROID_H_
#define __AUDIO_ENGINE_ANDROID_H_

#include <functional>
#include <iostream>
#include <map>
#include <fmod.hpp>
#include "AudioEngine.h"
#include "base/CCRef.h"

#define MAX_AUDIOINSTANCES 24

NS_CC_BEGIN
    namespace experimental{
class AudioEngineImpl;

class CC_DLL AudioEngineImpl : public cocos2d::Ref
{
public:
    AudioEngineImpl();
    ~AudioEngineImpl();
    
    bool init();
    int play2d(const std::string &fileFullPath ,bool loop ,float volume);
    void setVolume(int audioID,float volume);
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
    int preload(const std::string& filePath, std::function<void(bool)> callback);
    
    void update(float dt);
    
    //Internal fmod callback
    void onSoundFinished(FMOD::Channel* channel);

private:
    // returns null if a sound with the given path is not found
    FMOD::Sound* findSound(const std::string &path);
  
    FMOD::Channel* getChannel(FMOD::Sound*);

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
};
}
NS_CC_END
#endif // __AUDIO_ENGINE_ANDROID_H_
#endif
