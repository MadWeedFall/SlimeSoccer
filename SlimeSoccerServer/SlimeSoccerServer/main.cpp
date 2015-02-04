//
//  main.cpp
//  SlimeSoccerServer
//
//  Created by CK03-007 on 15-1-13.
//  Copyright (c) 2015年 CK03-007. All rights reserved.
//

#include <iostream>
#include "NetTransfer.h"
#include "kbhit.h"
#include "box2d.h"
#include <thread>
#include <chrono>

b2Vec2 gravity(0,0);
b2World world(gravity);

b2BodyDef p1bdef;
b2Body* p1body;
b2BodyDef p2bdef;
b2Body* p2body;
b2BodyDef ballbdef;
b2Body* ballbody;
mutex u_lock;

b2Vec2 p1prpos;
b2Vec2 p2prpos;
b2Vec2 ballprpos;

void addCommandFunc()
{
    while (true) {
        std::chrono::microseconds dr(1000/60);
        std::this_thread::sleep_for(dr);
        u_lock.lock();
        //printf("setCommand loop\n");
        
        if (p1body->IsAwake()&&((int)p1prpos.x!=(int)p1body->GetPosition().x||(int)p1prpos.y!=(int)p1body->GetPosition().y)) {
            
            NetTransfer::getInstance()->sendCommand(1, SET,p1body->GetPosition().x,p1body->GetPosition().y,p1body->GetAngle());
            p1prpos = p1body->GetPosition();
        }
        if (p2body->IsAwake()&&((int)p2prpos.x!=(int)p2body->GetPosition().x||(int)p2prpos.y!=(int)p2body->GetPosition().y)) {
            
           
            NetTransfer::getInstance()->sendCommand(2, SET,p2body->GetPosition().x,p2body->GetPosition().y,p2body->GetAngle());
            p2prpos = p2body->GetPosition();
        }
        if (ballbody->IsAwake()&&((int)ballprpos.x!=(int)ballbody->GetPosition().x||(int)ballprpos.y!=(int)ballbody->GetPosition().y)) {
            
            NetTransfer::getInstance()->sendCommand(3, SET,ballbody->GetPosition().x,ballbody->GetPosition().y,ballbody->GetAngle());
            ballprpos = ballbody->GetPosition();
        }
        
        u_lock.unlock();
        
    }
}

//tell if player or ball is still in the field
bool outofsquare(b2Vec2 pos)
{
    if (pos.x>=1280||pos.x<=0||pos.y>=800||pos.y<=0) {
        return true;
    }
    return false;
}

void update()
{
    while (true)
    {
        std::chrono::microseconds dr(1000/60);
        std::this_thread::sleep_for(dr);
        
        u_lock.lock();
        world.Step(1.0/60.0f, 6, 2);
        
        
        
        
        NetTransfer::getInstance()->update();
        
        DATA d = NetTransfer::getInstance()->getNextData();
        if (d.command==1) {
            printf("recieve data command %d\n",d.command);
        }
        
        float force = 10000;
        
        switch (d.command) {
            case UP:
                NetTransfer::getInstance()->clearCommandForId(d.playerIdx);
                if (d.playerIdx==1) {
                    
                    p1body->ApplyForceToCenter(b2Vec2(0, force),true);
                    //p1body->ApplyLinearImpulse(b2Vec2(0,force),p1body->GetPosition()+p1body->GetLocalCenter(), true);
                }
                if (d.playerIdx==2) {

                    p2body->ApplyForceToCenter(b2Vec2(0, force),true);
                    //p2body->ApplyLinearImpulse(b2Vec2(0, force), p2body->GetPosition()+p2body->GetLocalCenter(), true);
                }
                break;
            case DOWN:
                NetTransfer::getInstance()->clearCommandForId(d.playerIdx);
                if (d.playerIdx==1) {

                    p1body->ApplyForceToCenter(b2Vec2(0, -force), true);
                    //p1body->ApplyLinearImpulse(b2Vec2(0,-force),p1body->GetPosition()+p1body->GetLocalCenter(), true);
                }
                if (d.playerIdx==2) {

                    p2body->ApplyForceToCenter(b2Vec2(0, -force), true);
                    //p2body->ApplyLinearImpulse(b2Vec2(0,-force),p2body->GetPosition()+p2body->GetLocalCenter(), true);
                }
                break;
            case LEFT:
                NetTransfer::getInstance()->clearCommandForId(d.playerIdx);
                if (d.playerIdx==1) {

                    p1body->ApplyForceToCenter(b2Vec2(-force, 0), true);
                    //p1body->ApplyLinearImpulse(b2Vec2(-force,0),p1body->GetPosition()+p1body->GetLocalCenter(), true);
                }
                if (d.playerIdx==2) {

                    p2body->ApplyForceToCenter(b2Vec2(-force, 0), true);
                    //p2body->ApplyLinearImpulse(b2Vec2(-force, 0), p2body->GetPosition()+p2body->GetLocalCenter(), true);
                }
                break;
            case RIGHT:
                NetTransfer::getInstance()->clearCommandForId(d.playerIdx);
                if (d.playerIdx==1) {

                    p1body->ApplyForceToCenter(b2Vec2(force, 0), true);
                    //p1body->ApplyLinearImpulse(b2Vec2(force,0),p1body->GetPosition()+p1body->GetLocalCenter(), true);
                }
                if (d.playerIdx==2) {

                    p2body->ApplyForceToCenter(b2Vec2(force, 0), true);
                    //wp2body->ApplyLinearImpulse(b2Vec2(force, 0), p2body->GetPosition()+p2body->GetLocalCenter(), true);
                }
                break;
                
            default:
                break;
        }
        
        if (outofsquare(p1body->GetPosition())) {
            p1body->SetLinearVelocity(b2Vec2_zero);
            p1body->SetTransform(b2Vec2(490, 400), 0);
            
            
        }
        if (outofsquare(p2body->GetPosition())) {
            p2body->SetLinearVelocity(b2Vec2_zero);
            p2body->SetTransform(b2Vec2(790,400), 0);
            
           
        }
        if (outofsquare(ballbody->GetPosition())) {
            ballbody->SetLinearVelocity(b2Vec2_zero);
            ballbody->SetTransform(b2Vec2(640, 400), 0);
            
            
        }
        
        if (kbhit())
        {
            if (getchar()=='q')
            {
                break;
            }
            
            
        }
        
        u_lock.unlock();
    }
    
    
    NetTransfer::getInstance()->stop();
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Wellcome to slime soccer server!\n";
    NetTransfer::getInstance()->start();
    
    const float p2m = 2.0f;
    

    world.SetAllowSleeping(true);
    world.SetContinuousPhysics(true);
    
    
    p1bdef.type = b2_dynamicBody;
    p1bdef.position.Set(490.0f, 400.0f);
    p1body = world.CreateBody(&p1bdef);
    b2PolygonShape boxshape;
    boxshape.SetAsBox(46.0f/p2m, 46.0f/p2m);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxshape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    p1body->CreateFixture(&fixtureDef);
    
    p2bdef.type = b2_dynamicBody;
    p2bdef.position.Set(790.0f, 400.0f);
    p2body = world.CreateBody(&p2bdef);
    b2PolygonShape boxshape2;
    boxshape2.SetAsBox(46.0f/p2m, 46.0f/p2m);
    b2FixtureDef fixtureDef2;
    fixtureDef2.shape = &boxshape2;
    fixtureDef2.density = 1.0f;
    fixtureDef2.friction = 0.3f;
    p2body->CreateFixture(&fixtureDef2);
    
    
    ballbdef.type = b2_dynamicBody;
    ballbdef.position.Set(640, 400);
    ballbody = world.CreateBody(&ballbdef);
    b2CircleShape ballshape;
    ballshape.m_radius = 30.0f/p2m;
    b2FixtureDef fixtureDef3;
    fixtureDef3.shape = &ballshape;
    fixtureDef3.density = 1.0f;
    fixtureDef3.friction = 0.3f;
    ballbody->CreateFixture(&fixtureDef3);
    
    p1prpos = p1body->GetPosition();
    p2prpos = p2body->GetPosition();
    ballprpos = ballbody->GetPosition();
    
    std::thread updateThread(update);
    std::thread cmdThread(addCommandFunc);
    
    updateThread.join();
    cmdThread.join();
    

    

    return 0;
}
