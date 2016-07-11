#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && CC_USE_ANDROID_FMOD
#include <cstring>
#include "cocos2d.h"
#include "fmod_errors.h"
#include "AudioEngine-fmod.h"

#include <unistd.h>
// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <unordered_map>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <jni.h>
#include "audio/include/AudioEngine.h"
#include "base/CCDirector.h"
#include "base/CCScheduler.h"
#include "platform/android/CCFileUtils-android.h"
#include "platform/android/jni/CocosPlayClient.h"
#include "platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"

using namespace cocos2d;
using namespace cocos2d::experimental;

AudioEngineImpl* g_AudioEngineImpl = nullptr;

void ERRCHECKWITHEXIT(FMOD_RESULT result) 
{
    if (result != FMOD_OK) {
        CCLOG("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
    }
}

bool ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK) {
        CCLOG("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        return true;
    }
    return false;
}

FMOD_RESULT F_CALLBACK channelCallback(FMOD_CHANNEL* channel,
                                       FMOD_CHANNEL_CALLBACKTYPE type,
                                       void* commandData1, void* commandData2)
{
    if (type == FMOD_CHANNEL_CALLBACKTYPE_END) {
        g_AudioEngineImpl->onSoundFinished(reinterpret_cast<FMOD::Channel*>(channel));
    }

    return FMOD_OK;
}

AudioEngineImpl::AudioEngineImpl()
: _fmodSystem(nullptr),
  _bufferLength(1024),
  _numBuffers(4),
  _lazyInitLoop(true),
  _currentAudioID(0)
{
}

AudioEngineImpl::~AudioEngineImpl()
{
    ERRCHECKWITHEXIT(_fmodSystem->close());
    ERRCHECKWITHEXIT(_fmodSystem->release());

	stopFmodJNI();
}

bool AudioEngineImpl::init()
{
	startFmodJNI();

    ERRCHECKWITHEXIT(FMOD::System_Create(&_fmodSystem));
    ERRCHECK(_fmodSystem->setDSPBufferSize(_bufferLength, _numBuffers));
    ERRCHECK(_fmodSystem->init(MAX_AUDIOINSTANCES, FMOD_INIT_NORMAL, 0));

    _channelInfoMap.clear();
    _soundMap.clear();

    auto scheduler = cocos2d::Director::getInstance()->getScheduler();
    scheduler->schedule(schedule_selector(AudioEngineImpl::update), this, 0.05f, false);

    g_AudioEngineImpl = this; 

    return true;
}

int AudioEngineImpl::play2d(const std::string &fileFullPath, bool loop, float volume)
{
    int audioID = preload(fileFullPath, nullptr);
    if (audioID != AudioEngine::INVALID_AUDIO_ID) {
        _channelInfoMap[audioID].loop = loop;
        _channelInfoMap[audioID].channel->setPaused(true);
        _channelInfoMap[audioID].volume = volume;
        AudioEngine::_audioIDInfoMap[audioID].state = AudioEngine::AudioState::PAUSED;

        resume(audioID);

        if (_lazyInitLoop) {
            _lazyInitLoop = false;

            auto scheduler = cocos2d::Director::getInstance()->getScheduler();
            scheduler->schedule(schedule_selector(AudioEngineImpl::update), this, 0.05f, false);
        }

        return audioID;
    }
    else {
        CCLOG("AudioEngineImpl::play2d -  Couldn't play sound from %s ", fileFullPath.c_str());
        return AudioEngine::INVALID_AUDIO_ID;
    }
}

int AudioEngineImpl::preload(const std::string& filePath, std::function<void(bool isSuccess)> callback)
{
    FMOD::Sound* sound = findSound(filePath);
    if (sound == nullptr) {
        Data soundData = FileUtils::getInstance()->getDataFromFile(filePath);
        FMOD_CREATESOUNDEXINFO exinfo;
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        exinfo.length = static_cast<int>(soundData.getSize());
        FMOD_MODE mode = FMOD_HARDWARE | FMOD_OPENMEMORY | FMOD_LOOP_OFF;
        FMOD_RESULT result = _fmodSystem->createSound(reinterpret_cast<const char*>(soundData.getBytes()), mode, &exinfo, &sound);
        if (ERRCHECK(result)) {
            CCLOG("Sound effect in %s could not be preloaded\n", filePath.c_str());
            if (callback) {
                callback(false);
            }
            return AudioEngine::INVALID_AUDIO_ID;
        }
        _soundMap[filePath] = sound;
    }

    int audioID = _currentAudioID++;
    auto& chanelInfo = _channelInfoMap[audioID];
    chanelInfo.sound = sound;
    chanelInfo.id = static_cast<size_t>(audioID);
    chanelInfo.channel = nullptr;
    chanelInfo.callback = nullptr;
    chanelInfo.path = filePath;
        
    if (callback) {
        callback(true);
    }

    return audioID;
}

void AudioEngineImpl::setVolume(int audioID, float volume)
{
    if (_channelInfoMap.find(audioID) != _channelInfoMap.end()) {
        ERRCHECK(_channelInfoMap[audioID].channel->setVolume(volume));
    } 
    else {
        CCLOG("AudioEngineImpl::setVolume: invalid audioID: %d\n", audioID);
    }
}

void AudioEngineImpl::setLoop(int audioID, bool loop)
{
    if (_channelInfoMap.find(audioID) != _channelInfoMap.end()) {
        _channelInfoMap[audioID].channel->setLoopCount(loop ? -1 : 0);
    }
    else {
        CCLOG("AudioEngineImpl::setLoop: invalid audioID: %d\n", audioID);
    }
}

bool AudioEngineImpl::pause(int audioID)
{
    if (_channelInfoMap.find(audioID) != _channelInfoMap.end()) {
        _channelInfoMap[audioID].channel->setPaused(true);
        AudioEngine::_audioIDInfoMap[audioID].state = AudioEngine::AudioState::PAUSED;
        return true;
    }
    else {
        CCLOG("AudioEngineImpl::pause: invalid audioID: %d\n", audioID);
        return false;
    }
}

bool AudioEngineImpl::resume(int audioID)
{
    if (_channelInfoMap.find(audioID) != _channelInfoMap.end()) {
        if (!_channelInfoMap[audioID].channel) {
            FMOD::Channel* channel = nullptr;
            FMOD_RESULT result = _fmodSystem->playSound(FMOD_CHANNEL_FREE, _channelInfoMap[audioID].sound, true, &channel);
            if (ERRCHECK(result)) {
                return false;
            }

            channel->setMode(_channelInfoMap[audioID].loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
            channel->setLoopCount(_channelInfoMap[audioID].loop ? -1 : 0);
            channel->setVolume(_channelInfoMap[audioID].volume);
            channel->setUserData((void *)_channelInfoMap[audioID].id);
            _channelInfoMap[audioID].channel = channel;
        }

        _channelInfoMap[audioID].channel->setPaused(false);
        AudioEngine::_audioIDInfoMap[audioID].state = AudioEngine::AudioState::PLAYING;

        return true;
    }
    else {
        CCLOG("AudioEngineImpl::resume: invalid audioID: %d\n", audioID);
        return false;
    }
}

bool AudioEngineImpl::stop(int audioID)
{
    auto it = _channelInfoMap.find(audioID);
    if (it != _channelInfoMap.end()) {
        it->second.channel->stop();
        it->second.channel = nullptr;
        return true;
    }
    else {      
        CCLOG("AudioEngineImpl::stop: invalid audioID: %d\n", audioID);
        return false;
    }
}

void AudioEngineImpl::stopAll(){
    for (auto it = _channelInfoMap.begin(); it != _channelInfoMap.end(); ++it) {
        it->second.channel->stop();
        it->second.channel = nullptr;
    }
}

float AudioEngineImpl::getDuration(int audioID){
    if (_channelInfoMap.find(audioID) != _channelInfoMap.end()) {
        FMOD::Sound * sound = _channelInfoMap[audioID].sound; 
        unsigned int length; 
        ERRCHECK(sound->getLength(&length, FMOD_TIMEUNIT_MS));
        float duration = (float)length / 1000.0f; 
        return duration;
    }
    else {
        CCLOG("AudioEngineImpl::getDuration: invalid audioID: %d\n", audioID);
        return AudioEngine::TIME_UNKNOWN;
    }
}

float AudioEngineImpl::getCurrentTime(int audioID) {
    if (_channelInfoMap.find(audioID) != _channelInfoMap.end()) {
        unsigned int position; 
        ERRCHECK(_channelInfoMap[audioID].channel->getPosition(&position, FMOD_TIMEUNIT_MS));
        float currenttime = position /1000.0f;
        return currenttime; 
    }
    else {
        CCLOG("AudioEngineImpl::getCurrentTime: invalid audioID: %d\n", audioID);
        return AudioEngine::TIME_UNKNOWN;
    }
}

bool AudioEngineImpl::setCurrentTime(int audioID, float time) {
    if (_channelInfoMap.find(audioID) != _channelInfoMap.end()) {
        unsigned int position = static_cast<unsigned int>(time * 1000.0f);
        ERRCHECK(_channelInfoMap[audioID].channel->setPosition(position, FMOD_TIMEUNIT_MS));
        return true;
    } 
    else {
        CCLOG("AudioEngineImpl::setCurrentTime: invalid audioID: %d\n", audioID);
        return false;
    }
}

void AudioEngineImpl::setFinishCallback(int audioID, const std::function<void(int, const std::string &)> &callback)
{
    if (_channelInfoMap.find(audioID) != _channelInfoMap.end()) {
        FMOD::Channel* channel = _channelInfoMap[audioID].channel;
        _channelInfoMap[audioID].callback = callback;
        ERRCHECK(channel->setCallback(channelCallback));
    } 
    else {
        CCLOG("AudioEngineImpl::setFinishCallback: invalid audioID: %d\n", audioID);
    }
}

void AudioEngineImpl::onSoundFinished(FMOD::Channel* channel)
{
    if (channel != nullptr) {
        void* data;
        channel->getUserData(&data);
        auto audioID = reinterpret_cast<int>(data);
        auto it = _channelInfoMap.find(audioID);
        if (it != _channelInfoMap.end()) {
            ChannelInfo& audioRef = it->second;

            if (audioRef.callback) {
                audioRef.callback(audioID, audioRef.path);
            }

            if (audioRef.channel != nullptr) {
                audioRef.channel->stop();
            }

            audioRef.channel = nullptr;
            audioRef.callback = nullptr;
            _finishedAudioIDs.insert(audioID);
        } else {
            CCLOG("AudioEngineImpl::onSoundFinished: invalid audioID: %d\n", audioID);
        }
    } 
    else {
        CCLOG("AudioEngineImpl::onSoundFinished: invalid channel\n");
    }
}

void AudioEngineImpl::uncache(const std::string& path) 
{
    auto it = _soundMap.find(path);
    if (it != _soundMap.end()) {
        FMOD::Sound* sound = it->second;
        if (sound) {
            sound->release();
            _soundMap.erase(it);
        }
    }
}

void AudioEngineImpl::uncacheAll()
{
    for (auto it = _soundMap.cbegin(); it != _soundMap.cend(); ++it) {
        auto sound = it->second;
        if (sound) {
            sound->release();
        }
    }
    _soundMap.clear();
}

void AudioEngineImpl::update(float dt)
{
    _fmodSystem->update();

    for(auto audioID : _finishedAudioIDs) {
        AudioEngine::remove(audioID);
        _channelInfoMap.erase(audioID);
    }
    _finishedAudioIDs.clear();

    if (_channelInfoMap.empty()) {
        _lazyInitLoop = true;

        auto scheduler = cocos2d::Director::getInstance()->getScheduler();
        scheduler->unschedule(schedule_selector(AudioEngineImpl::update), this);
    }
}

FMOD::Sound* AudioEngineImpl::findSound(const std::string &path)
{
    auto it = _soundMap.find(path);
    return (it!=_soundMap.end())?(it->second):nullptr; 
}

#endif
