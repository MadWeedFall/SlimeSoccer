//
//  NetTransfer.cpp
//  SlimeSoccer
//
//  Created by CK03-007 on 15-1-13.
//
//

#include "NetTransfer.h"

static NetTransfer* _nti = NULL;
static unsigned int _packetCounter = 0;

NetTransfer::NetTransfer()
{
    _server=RakNet::RakPeerInterface::GetInstance();
    _server->SetIncomingPassword("Rumpelstiltskin", (int)strlen("Rumpelstiltskin"));
    _server->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
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
    RakNet::SocketDescriptor sd;
    sd.port=atoi(_serverPort);
    sd.socketFamily=AF_INET; // Test out IPV4
    bool b = _server->Startup(4, &sd, 1 )==RakNet::RAKNET_STARTED;
    _server->SetMaximumIncomingConnections(4);
    if (!b)
    {
        puts("Server failed to start.  Terminating.");
        exit(1);
        
    }
    _server->SetOccasionalPing(true);
    _server->SetUnreliableTimeout(1000);
}

void NetTransfer::stop()
{
    _server->Shutdown(100);
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
            
            _bitStream.Write(d.playerIdx);
            _bitStream.Write(d.command);
            _bitStream.Write(d.posX);
            _bitStream.Write(d.posY);
            _bitStream.Write(d.rot);
            
            
            
            //printf("send command %d  NO.%d\n",_dataBuffer[i].command,i);
            _dataBuffer.erase(_dataBuffer.begin()+i);
        }
        printf("time stamp: %llu\n ",time);
        _server->Send(&_bitStream,HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
    }
    
    
    DATA recvd;
    for (_packet = _server->Receive(); _packet;_server->DeallocatePacket(_packet),_packet = _server->Receive()) {
        
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
                    }
                }

                

                
                break;
            default:
                break;
        }
        
    }
    c_lock.unlock();
}

DATA NetTransfer::getNextData()
{
    DATA d;
    d.posX =.0f;
    d.posY =.0f;
    d.rot = .0f;
    d.playerIdx = 1;
    d.command = WAIT;
    c_lock.lock();
    
    if (_recvBuffer.size()!=0) {
        d = _recvBuffer[0];
        _recvBuffer.erase(_recvBuffer.begin());
    }
    
    if (d.command==1) {
        printf("recieve data command %d\n",d.command);
    }
    
    c_lock.unlock();
    
    return d;
}

void NetTransfer::clearCommand()
{
    c_lock.lock();
    
    _dataBuffer.clear();
    
    c_lock.unlock();
}

void NetTransfer::clearCommandForId(int playerIdx)
{
    c_lock.lock();
    
    for(vector<DATA>::iterator it = _dataBuffer.begin();it!=_dataBuffer.end();++it)
    {
        if (it->playerIdx ==playerIdx) {
            printf("remove it %d\n",it->packetNumber);
            _dataBuffer.erase(it);
        }
    }
    
    
    c_lock.unlock();
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
    
    if (_packetCounter<INT32_MAX) {
        _packetCounter++;
    }else
    {
        _packetCounter = 0;
    }
    data.packetNumber = _packetCounter;
    
    _dataBuffer.push_back(data);

    
    c_lock.unlock();
}
