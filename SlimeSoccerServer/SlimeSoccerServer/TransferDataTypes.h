//
//  TansferDataTypes.h
//  SlimeSoccer
//
//  Created by CK03-007 on 15-1-13.
//
//

#ifndef SlimeSoccer_TansferDataTypes_h
#define SlimeSoccer_TansferDataTypes_h

typedef struct transferData
{
    RakNet::Time time;
    unsigned int packetNumber;
    int playerIdx;
    int command;
    int posX;
    int posY;
    int rot;
    
    
} DATA;

enum COMMAND
{
    WAIT,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SET
    
};

#endif
