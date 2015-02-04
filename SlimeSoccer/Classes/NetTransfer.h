//
//  NetTransfer.h
//  SlimeSoccer
//
//  Created by CK03-007 on 15-1-13.
//
//

#ifndef __SlimeSoccer__NetTransfer__
#define __SlimeSoccer__NetTransfer__

#include <stdio.h>
#include "MessageIdentifiers.h"

#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "PacketLogger.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "RakNetTypes.h"
#include "TransferDataTypes.h"
#include <vector>
#include "BitStream.h"
#include <mutex>
#include "GetTime.h"
using namespace std;



class NetTransfer
{
public:
    ~NetTransfer();
    static NetTransfer* getInstance();
    void setConnectionProperty(const char*ip,const char*serverPort,const char*clientPort);
    void start();
    void update();
    void stop();
    void sendCommand(int playerIdx,int command,int posX = 0,int posY = 0, int rot = 0);
    static bool sortTimestamp(const DATA &v1,const DATA &v2);
    DATA getNextData();
    mutex c_lock;
private:
    NetTransfer();
    RakNet::RakNetStatistics *_rss;
    RakNet::RakPeerInterface *_client;
    RakNet::Packet *_packet;
    RakNet::SystemAddress _clientID;
    RakNet::BitStream _bitStream;
    char _ip[64];
    char _serverPort[30];
    char _clientPort[30];
    char _message[2048];
    vector<DATA> _dataBuffer;
    vector<DATA> _recvBuffer;
};

#endif /* defined(__SlimeSoccer__NetTransfer__) */
