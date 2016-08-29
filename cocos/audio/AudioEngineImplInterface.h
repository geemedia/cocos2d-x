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

#ifndef __AUDIO_ENGINE_IMPL_INTERFACE_H_
#define __AUDIO_ENGINE_IMPL_INTERFACE_H_

#include <functional>
#include <list>
#include <string>
#include <unordered_map>
#include "base/CCRef.h"

NS_CC_BEGIN
namespace experimental{

class CC_DLL AudioEngineImplInterface : public cocos2d::Ref
{
public:
    virtual bool init() = 0;
    virtual int play2d(const std::string &fileFullPath, bool loop, float volume, int audioId = -1) = 0;
    virtual void setVolume(int audioID,float volume) = 0;
    virtual void setLoop(int audioID, bool loop) = 0;
    virtual bool pause(int audioID) = 0;
    virtual bool resume(int audioID) = 0;
    virtual bool stop(int audioID) = 0;
    virtual void stopAll() = 0;
    virtual float getDuration(int audioID) = 0;
    virtual float getCurrentTime(int audioID) = 0;
    virtual bool setCurrentTime(int audioID, float time) = 0;
    virtual void setFinishCallback(int audioID, const std::function<void (int, const std::string &)> &callback) = 0;
    
    virtual void uncache(const std::string& filePath) = 0;
    virtual void uncacheAll() = 0;
    virtual void preload(const std::string& filePath, std::function<void(bool)> callback) = 0;
};
}
NS_CC_END
#endif // __AUDIO_ENGINE_IMPL_INTERFACE_H_
