//
//  NetTransfer.cpp
//  SlimeSoccer
//
//  Created by CK03-007 on 15-1-13.
//
//

#include "NetTransfer.h"

static NetTransfer* _nti = NULL;

NetTransfer::NetTransfer()
{
    _client = RakNet::RakPeerInterface::GetInstance();
    _client->AllowConnectionResponseIPMigration(false);
    strcpy(_ip,"127.0.0.1");
    strcpy(_clientPort, "12345");
    strcpy(_serverPort, "54321");
    
}

NetTransfer::~NetTransfer()
{
    
}

NetTransfer* NetTransfer::getInstance()
{
    if (_nti==NULL) {
        _nti = new NetTransfer();
    }
    return _nti;
}

void NetTransfer::setConnectionProperty(const char *ip, const char *serverPort, const char *clientPort)
{
    strcpy(_ip,ip);
    strcpy(_serverPort, serverPort);
    strcpy(_clientPort, clientPort);
}

void NetTransfer::start()
{
    RakNet::SocketDescriptor socketDescriptor(atoi(_clientPort),0);
    socketDescriptor.socketFamily=AF_INET;
    bool b = _client->Startup(8,&socketDescriptor, 1) == RakNet::RAKNET_STARTED;
    if (b) {
        printf("started");
    }
    _client->SetOccasionalPing(true);
    //reachability detection
    RakNet::ConnectionAttemptResult car = _client->Connect(_ip, atoi(_serverPort), "Rumpelstiltskin", (int) strlen("Rumpelstiltskin"));
    
    RakAssert(car==RakNet::CONNECTION_ATTEMPT_STARTED);
    
    printf("client started!\n");
}

void NetTransfer::stop()
{
    _client->Shutdown(100);
}

void NetTransfer::update()
{
    c_lock.lock();
    if (_dataBuffer.size()!=0) {
        _bitStream.Reset();
       
        _bitStream.Write((unsigned char) (ID_TIMESTAMP));
        RakNet::Time time = RakNet::GetTime();
        _bitStream.Write(time);
        
        for (int i=0; i<_dataBuffer.size(); i++) {
            DATA d = _dataBuffer[i];
            _bitStream.Write((unsigned char) (ID_USER_PACKET_ENUM+1));
            _bitStream.Write(d.packetNumber);
            printf("time stamp: %llu packet No. %d playeridx:%d posx:%d posy:%d\n ",time,d.packetNumber,d.playerIdx,d.posX,d.posY);
            _bitStream.Write(d.playerIdx);
            _bitStream.Write(d.command);
            _bitStream.Write(d.posX);
            _bitStream.Write(d.posY);
            _bitStream.Write(d.rot);
            
            
            
            //printf("send command %d  NO.%d\n",_dataBuffer[i].command,i);
            _dataBuffer.erase(_dataBuffer.begin()+i);
        }
        _client->Send(&_bitStream,HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
    }
    
    
    DATA recvd;
    for (_packet = _client->Receive(); _packet;_client->DeallocatePacket(_packet),_packet = _client->Receive()) {
        
        switch (_packet->data[0]) {
            case ID_NEW_INCOMING_CONNECTION:
                printf("ID_NEW_INCOMING_CONNECTION\n");
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                printf("ID_DISCONNECTION_NOTIFICATION\n");
                break;
            case ID_CONNECTION_LOST:
                printf("ID_CONNECTION_LOST\n");
                break;
                
            case ID_TIMESTAMP:
                _bitStream.Reset();
                _bitStream.Write((char*)_packet->data,_packet->length);
                _bitStream.IgnoreBits(8);
                RakNet::Time time;
                _bitStream.Read(time);
                _bitStream.IgnoreBits(8);
                
                while (_bitStream.Read(recvd.packetNumber)) {
                    //                    _bitStream.Read(recvd.packetNumber);
                    _bitStream.Read(recvd.playerIdx);
                    _bitStream.Read(recvd.command);
                    _bitStream.Read(recvd.posX);
                    _bitStream.Read(recvd.posY);
                    _bitStream.Read(recvd.rot);
                    if (recvd.command != WAIT) {
                        _recvBuffer.push_back(recvd);
                        printf("%d %d\n",recvd.playerIdx,recvd.command);
                    }else
                    {
                        _recvBuffer.clear();
                        break;
                    }
                    _bitStream.IgnoreBits(8);
                }
                
                
                
                
                break;
            default:
                break;
        }
        
    }
    c_lock.unlock();
}


bool NetTransfer::sortTimestamp(const DATA &v1, const DATA &v2)
{
    return (v1.packetNumber<v2.packetNumber);
}

DATA NetTransfer::getNextData()
{
   

    DATA d;
    d.posX =0;
    d.posY =0;
    d.rot = 0;
    d.playerIdx = 1;
    d.command = WAIT;
    c_lock.lock();
    
    if (_recvBuffer.size()!=0) {
        d = _recvBuffer[0];
        printf("time:%llu number:%d %d %d %d %d %d\n",d.time,d.packetNumber,d.playerIdx,d.command,d.posX,d.posY,d.rot);
        _recvBuffer.erase(_recvBuffer.begin());
        //printf("_recvBuffer size %lu",_recvBuffer.size());
    }
    
    c_lock.unlock();
    
    return d;
}


void NetTransfer::sendCommand(int playerIdx, int command,int posX,int posY, int rot)
{
    c_lock.lock();
    DATA data;
    data.playerIdx = playerIdx;
    data.command   = command;
    data.posX = posX;
    data.posY = posY;
    data.rot  = rot;
    _dataBuffer.push_back(data);
    c_lock.unlock();
}
