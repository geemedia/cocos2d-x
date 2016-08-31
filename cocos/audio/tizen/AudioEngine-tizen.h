/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN

#ifndef __AUDIO_ENGINE_TIZEN_H_
#define __AUDIO_ENGINE_TIZEN_H_

#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <player.h>

#include "audio/AudioEngineImplInterface.h"
#include "base/CCRef.h"
#include "base/ccUtils.h"

#define MAX_AUDIOINSTANCES 8

#define ERRORLOG(msg) log("fun:%s,line:%d,msg:%s",__func__,__LINE__,#msg)

NS_CC_BEGIN
    namespace experimental{
class AudioEngineImpl;

class AudioPlayer
{
public:
    AudioPlayer();
    ~AudioPlayer();

    void init(const std::string& fileFullPath, float volume, bool loop);
    void stopPlayer();

    player_h _playerHandle;
    std::function<void()> _initCallback;
    std::mutex _taskMutex;

private:
    bool _playOver;
    float _duration;
    int _audioID;
    bool _initSucceed;
    
    std::function<void (int, const std::string &)> _finishCallback;

    friend class AudioEngineImpl;
};

class AudioEngineImpl : public AudioEngineImplInterface
{
public:
    AudioEngineImpl();
    ~AudioEngineImpl();

    bool init() override;
    int play2d(const std::string& fileFullPath, bool loop, float volume, int audioID) override;
    void setVolume(int audioID, float volume) override;
    void setLoop(int audioID, bool loop) override;
    void pause(int audioID) override;
    void resume(int audioID) override;
    void stop(int audioID) override;
    void stopAll() override;
    float getDuration(int audioID) override;
    float getCurrentTime(int audioID) override;
    bool setCurrentTime(int audioID, float time) override;
    void setFinishCallback(int audioID, const std::function<void (int, const std::string &)>& callback) override;

    void uncache(const std::string& filePath) override {}
    void uncacheAll() override {}

    void preload(const std::string& filePath, std::function<void(bool)> callback) override;
    
    void update(float dt);

private:
    void initPlayerCallback(AudioPlayer* player, int audioID);

    //audioID,AudioInfo
    std::unordered_map<int, AudioPlayer>  _audioPlayers;

    std::mutex _threadMutex;
    std::vector<int> _toRemoveAudioIDs;

    int currentAudioID;
    
    bool _lazyInitLoop;
};

 }
NS_CC_END

#endif // __AUDIO_ENGINE_TIZEN_H_

#endif
