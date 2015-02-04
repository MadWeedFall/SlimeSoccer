#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "NetTransfer.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace cocos2d::ui;

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    void update(float delta);
    
    void menuCloseCallback(cocos2d::Ref* pSender);
    void setPlayerIdxCallBack(cocos2d::Ref* pSender);
    void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
    
    bool canOperate;
    
    Sprite *player1;
    Sprite *player2;
    Sprite *ball;
    Node *gateLeft;
    Node *gateRight;
    Vec2 posBall;
    Vec2 posP1;
    Vec2 posP2;
    Button *P1Btn;
    Button *P2Btn;
    
    
    int playerIdx;
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
