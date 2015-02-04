#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

using namespace cocostudio::timeline;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("MainScene.csb");
    addChild(rootNode);
    
    auto closeItem = static_cast<ui::Button*>(rootNode->getChildByName("CloseBtn"));
    closeItem->addTouchEventListener(CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    P1Btn = static_cast<Button*>(rootNode->getChildByName("Player1Btn"));
    P1Btn->addTouchEventListener(CC_CALLBACK_1(HelloWorld::setPlayerIdxCallBack,this));
    
    P2Btn = static_cast<Button*>(rootNode->getChildByName("Player2Btn"));
    P2Btn->addTouchEventListener(CC_CALLBACK_1(HelloWorld::setPlayerIdxCallBack,this));
    
    player1 = rootNode->getChildByName<Sprite*>("player1");
    player2 = rootNode->getChildByName<Sprite*>("player2");
    ball    = rootNode->getChildByName<Sprite*>("ball");
    gateLeft = rootNode->getChildByName<Node*>("gateLeft");
    gateRight = rootNode->getChildByName<Node*>("gateRight");
    
    canOperate = true;
    
    posBall = ball->getPosition();
    posP1 = player1->getPosition();
    posP2 = player2->getPosition();
    
    playerIdx = 1;
    
    //NetTransfer::getInstance()->start();
    
    this->setKeyboardEnabled(true);//this must set true to use keyboard input
    
    return true;
}

void HelloWorld::update(float delta)
{
    
    
    NetTransfer::getInstance()->update();
    
    DATA d = NetTransfer::getInstance()->getNextData();
    
    if (d.command == SET) {
        if (d.playerIdx==1) {
            player1->setPosition(Vec2(d.posX,d.posY));
            player1->setRotation(d.rot);
        }else if(d.playerIdx == 2)
        {
            player2->setPosition(Vec2(d.posX, d.posY));
            player2->setRotation(d.rot);
        }else
        {
            ball->setPosition(Vec2(d.posX,d.posY));
            ball->setRotation(d.rot);
        }
    }    

}

void HelloWorld::menuCloseCallback(cocos2d::Ref *pSender)
{
    NetTransfer::getInstance()->stop();
}

void HelloWorld::setPlayerIdxCallBack(cocos2d::Ref *pSender)
{
    if (pSender == P1Btn) {
        playerIdx = 1;
        NetTransfer::getInstance()->setConnectionProperty("127.0.0.1", "54321", "12345");
        NetTransfer::getInstance()->start();
        scheduleUpdate();//use default update loop
    }else
    {
        playerIdx = 2;
        NetTransfer::getInstance()->setConnectionProperty("127.0.0.1", "54321", "67890");
        NetTransfer::getInstance()->start();
        scheduleUpdate();//use default update loop
    }
}

void HelloWorld::onKeyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
    if(canOperate)
    {
        log("key pressed");
        switch (keyCode) {
            case EventKeyboard::KeyCode::KEY_W:
                //dosometiing
                NetTransfer::getInstance()->sendCommand(playerIdx, UP);
                break;
            case EventKeyboard::KeyCode::KEY_S:
                NetTransfer::getInstance()->sendCommand(playerIdx, DOWN);
                break;
            case EventKeyboard::KeyCode::KEY_A:
                NetTransfer::getInstance()->sendCommand(playerIdx, LEFT);
                break;
            case EventKeyboard::KeyCode::KEY_D:
                NetTransfer::getInstance()->sendCommand(playerIdx, RIGHT);
                break;
            default:
                break;
        }
    }    
}



void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
    
    
}