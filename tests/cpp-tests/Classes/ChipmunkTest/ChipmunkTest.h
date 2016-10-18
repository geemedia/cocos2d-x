//
// cocos2d
//
#ifndef __CHIPMUNKTEST_H__
#define __CHIPMUNKTEST_H__

#include "cocos2d.h"
#include "../BaseTest.h"

#if CC_ENABLE_CHIPMUNK_INTEGRATION
#include "chipmunk/chipmunk.h"
#include "extensions/cocos-ext.h"
#endif

class ChipmunkTest : public TestCase
{
public:
    CREATE_FUNC(ChipmunkTest);

    ChipmunkTest();
    ~ChipmunkTest();
    void onEnter() override;
    void initPhysics();
    void createResetButton();
    void reset(cocos2d::Ref* sender);

    void addNewSpriteAtPosition(cocos2d::Vec2 p);
    void update(float dt) override;
    void toggleDebugCallback(cocos2d::Ref* sender);
    void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    virtual void onAcceleration(cocos2d::Acceleration* acc, cocos2d::Event* event);

private:
#if CC_ENABLE_CHIPMUNK_INTEGRATION
    cocos2d::Texture2D* _spriteTexture; // weak ref
    cocos2d::extension::PhysicsDebugNode* _debugLayer; // weak ref
    cpSpace* _space; // strong ref
    cpShape* _walls[4];
#endif
};

DEFINE_TEST_SUITE(ChipmunkTests);

#endif /* __CHIPMUNKACCELTOUCHTEST_H__ */
