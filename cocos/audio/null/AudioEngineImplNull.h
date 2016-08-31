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

#ifndef __AUDIO_ENGINE_IMPL_NULL_H_
#define __AUDIO_ENGINE_IMPL_NULL_H_

#include <functional>
#include <iostream>
#include <map>
#include <set>
#include "audio/AudioEngineImplInterface.h"
#include "audio/include/AudioEngine.h"
#include "base/CCRef.h"

NS_CC_BEGIN
namespace experimental{

class CC_DLL AudioEngineImplNull: public AudioEngineImplInterface
{
public:
    AudioEngineImplNull(int currentAudioID);
    ~AudioEngineImplNull();
    bool init() override;
    int play2d(const std::string& fileFullPath, bool loop, float volume, int audioID) override;
    void setVolume(int audioID,float volume) override;
    void setLoop(int audioID, bool loop) override;
    bool pause(int audioID) override;
    bool resume(int audioID) override;
    bool stop(int audioID) override;
    void stopAll() override;
    float getDuration(int audioID) override;
    float getCurrentTime(int audioID) override;
    bool setCurrentTime(int audioID, float time) override;
    void setFinishCallback(int audioID, const std::function<void (int, const std::string &)>& callback) override;
    void uncache(const std::string& filePath) override;
    void uncacheAll() override;
    void preload(const std::string& filePath, std::function<void(bool)> callback) override;

    void update(float dt);
private:
    int _currentAudioID;
};
}
NS_CC_END
#endif // __AUDIO_ENGINE_IMPL_NULL_H_
