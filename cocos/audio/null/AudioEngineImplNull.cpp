    /****************************************************************************
 Copyright (c) 2016 Global Eagle Entertainment

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

#include "audio/null/AudioEngineImplNull.h"
#include "base/CCDirector.h"
#include "base/CCScheduler.h"

using namespace cocos2d;
using namespace cocos2d::experimental;

AudioEngineImplNull::AudioEngineImplNull(int currentAudioID) : _currentAudioID(currentAudioID)
{
    auto scheduler = cocos2d::Director::getInstance()->getScheduler();
    scheduler->schedule(schedule_selector(AudioEngineImplNull::update), this, 0.05f, false);
}

AudioEngineImplNull::~AudioEngineImplNull()
{
    auto scheduler = cocos2d::Director::getInstance()->getScheduler();
    scheduler->unschedule(schedule_selector(AudioEngineImplNull::update), this);
}

bool AudioEngineImplNull::init()
{
    return true;
}

int AudioEngineImplNull::play2d(const std::string &fileFullPath, bool loop, float volume, int audioId)
{
    AudioEngine::_audioIDInfoMap[_currentAudioID].state = AudioEngine::AudioState::PLAYING;
    return _currentAudioID++;
}

void AudioEngineImplNull::setVolume(int audioID,float volume)
{
    AudioEngine::_audioIDInfoMap[audioID].volume = volume;
}

void AudioEngineImplNull::setLoop(int audioID, bool loop)
{
    AudioEngine::_audioIDInfoMap[audioID].loop = loop;
}

bool AudioEngineImplNull::pause(int audioID)
{
    AudioEngine::_audioIDInfoMap[audioID].state = AudioEngine::AudioState::PAUSED;
    return true;
}

bool AudioEngineImplNull::resume(int audioID)
{
    AudioEngine::_audioIDInfoMap[audioID].state = AudioEngine::AudioState::PLAYING;
    return true;
}

bool AudioEngineImplNull::stop(int audioID)
{
    return true;
}

void AudioEngineImplNull::stopAll()
{
}

float AudioEngineImplNull::getDuration(int audioID)
{
    return AudioEngine::_audioIDInfoMap[audioID].duration;
}

float AudioEngineImplNull::getCurrentTime(int audioID)
{
    return AudioEngine::_audioIDInfoMap[audioID].currentTime;
}

bool AudioEngineImplNull::setCurrentTime(int audioID, float time)
{
    AudioEngine::_audioIDInfoMap[audioID].currentTime = time;
    return true;
}

void AudioEngineImplNull::setFinishCallback(int audioID, const std::function<void (int, const std::string &)> &callback)
{
    AudioEngine::_audioIDInfoMap[audioID].finishCallback = callback;
}

void AudioEngineImplNull::uncache(const std::string& filePath)
{
}

void AudioEngineImplNull::uncacheAll()
{
}

void AudioEngineImplNull::preload(const std::string& filePath, std::function<void(bool)> callback)
{
}

void experimental::AudioEngineImplNull::update(float dt)
{
    std::set<int> finishedAudioIds;
    for (auto& info : AudioEngine::_audioIDInfoMap)
    {
        if (info.second.state != AudioEngine::AudioState::PLAYING)
            continue;
        
        info.second.currentTime += dt;
        if (info.second.currentTime >= info.second.duration)
        {
            if (info.second.loop)
            {
                do
                {
                    info.second.currentTime -= info.second.duration;
                }
                while (info.second.currentTime >= info.second.duration);
            }
            else
            {
                if (info.second.finishCallback)
                    info.second.finishCallback(info.first, *info.second.filePath);

                finishedAudioIds.insert(info.first);
            }
        }
    }

    for (auto audioId : finishedAudioIds)
    {
        AudioEngine::remove(audioId);
    }
}
