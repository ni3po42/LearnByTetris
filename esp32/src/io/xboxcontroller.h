#ifndef __xboxcontroller_h
#define __xboxcontroller_h

#include <Arduino.h>
#include <memory>

struct Pair {
    uint16_t horizontal;
    uint16_t vertical;
};

struct XboxState
{   
    bool A;
    bool B;
    bool X;
    bool Y;
    bool Share;
    bool Start;
    bool Select;
    bool Xbox;
    bool LeftBumper;
    bool RightBumper;
    bool LeftStick;
    bool RightStick;
    bool Up;
    bool Left;
    bool Right;
    bool Down;
    Pair LeftJoyStick;
    Pair RightJoyStick;
    uint16_t LeftTrigger;
    uint16_t RightTrigger;

    void parse(uint8_t* data, size_t length);
};

typedef void XboxStatusListener(const XboxState& state);

class XboxController {
    private:
        struct XboxControllerImpl;
        std::shared_ptr<XboxControllerImpl> impl;

    public:
        XboxController();
        void listen(XboxStatusListener& listener);
        void start();
        bool isConnected();
        bool isWaiting();
        void reconnect();        
};

#endif