#include <NimBLEDevice.h>
#include "xboxcontroller.h"

static uint8_t directionalMap[9] = {
    //UDRL
    0b0000,
    0b1000,
    0b1001,
    0b0001,
    0b0101,
    0b0100,
    0b0110,
    0b0010,
    0b1010,
};

static NimBLEUUID serviceHid("1812");
static const uint16_t appearance = 964;
static const char* manufacturerDataNormal = "060000";
static const char* manufacturerDataSearching = "0600030080";

enum class State {
    None,
    Scanning,
    Found,
    Waiting,
    Connected
};

static uint16_t toUint16(uint8_t* data, size_t offset) {
    size_t size = offset * sizeof(uint8_t);
    return *((uint16_t*)(data + size));
}

void XboxState::parse(uint8_t* data, size_t length) {
    
    if (length > 11) {
        LeftJoyStick.horizontal = toUint16(data, 0);
        LeftJoyStick.vertical = toUint16(data, 2);
        RightJoyStick.horizontal = toUint16(data, 4);
        RightJoyStick.vertical = toUint16(data, 6);
        LeftTrigger = toUint16(data, 8);
        RightTrigger = toUint16(data, 10);
    }

    if (length > 12) {
        auto bitField = directionalMap[data[12] % 9];
        Right = bitField & 0b0001;
        Left = bitField & 0b0010;        
        Down = bitField & 0b0100;
        Up = bitField & 0b1000;
    }

    if (length > 13) {
        auto bitField = data[13];
        A =  bitField & 0b00000001;
        B =  bitField & 0b00000010;
        X =  bitField & 0b00001000;
        Y =  bitField & 0b00010000;
        LeftBumper = bitField & 0b01000000;
        RightBumper = bitField & 0b10000000;
    }

    if (length > 14) {
        auto bitField = data[14];
        Select = bitField & 0b00000100;
        Start = bitField & 0b00001000;
        Xbox = bitField & 0b00010000;
        LeftStick = bitField & 0b00100000;
        RightStick = bitField & 0b01000000;
    }

    if (length > 15) {
        Share = data[15] & 0b00000001;
    }
}

struct XboxController::XboxControllerImpl {
    
    class ClientCallbacks : public NimBLEClientCallbacks {
    public:
        ClientCallbacks(XboxController::XboxControllerImpl* controllerImpl) {
            this->controllerImpl = controllerImpl;
        }

        void onConnect(NimBLEClient* client) {
            controllerImpl->connectionState = State::Waiting;
        }

        void onDisconnect(NimBLEClient* client) {
            controllerImpl->connectionState = State::Scanning;
            controllerImpl->client = nullptr;
        }

        uint32_t onPassKeyRequest() {
            return 0;
        }

        bool onConfirmPIN(uint32_t passkey) {            
            return true;
        }

        void onAuthenticationComplete(ble_gap_conn_desc* descriptor) {
            if (descriptor->sec_state.encrypted) {
                return; 
            }
            
            NimBLEDevice::getClientByID(descriptor->conn_handle)->disconnect();
        }

    private:
        XboxController::XboxControllerImpl* controllerImpl;
    };

    class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    public:
        AdvertisedDeviceCallbacks(XboxController::XboxControllerImpl* controllerImpl) {
            this->controllerImpl = controllerImpl;
        }

        void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
            auto manufacturerData = advertisedDevice->getManufacturerData();
            uint8_t* data = (uint8_t*)manufacturerData.data();
            size_t len = manufacturerData.length();

            char* hexData = NimBLEUtils::buildHexData(nullptr, data, len);

            if (controllerImpl->targetDeviceAddress == nullptr) {            
                bool appearanceMatch = advertisedDevice->getAppearance() == appearance;
                bool serviceUUIDMatch = advertisedDevice->getServiceUUID().equals(serviceHid);
                bool normalManufactureDataMatch = strcmp(hexData, manufacturerDataNormal) == 0;
                bool searchingManufactureDataMatch = strcmp(hexData, manufacturerDataSearching) == 0;
                bool manufactureDataMatch = normalManufactureDataMatch || searchingManufactureDataMatch;

                if (appearanceMatch && serviceUUIDMatch && manufactureDataMatch) {
                    controllerImpl->targetDeviceAddress = new NimBLEAddress(advertisedDevice->getAddress());
                }
            }
            
            auto targetDeviceAddress = controllerImpl->targetDeviceAddress;
            bool targetDeviceIsSet = targetDeviceAddress != nullptr;
            
            if (targetDeviceIsSet && advertisedDevice->getAddress().equals(*targetDeviceAddress)) {
                controllerImpl->connectionState = State::Found;
                controllerImpl->advertisedDevice = advertisedDevice;                
            }        
        }

    private:
        XboxController::XboxControllerImpl* controllerImpl;
    };

    XboxControllerImpl(const char* deviceAddress = nullptr) {
        if (deviceAddress != nullptr) {
            targetDeviceAddress = new NimBLEAddress(deviceAddress);
        }
    }

    ~XboxControllerImpl() {
        if (targetDeviceAddress != nullptr) {
            delete targetDeviceAddress;
        }
    }

    NimBLEAddress* targetDeviceAddress = nullptr;
    NimBLEAdvertisedDevice* advertisedDevice = nullptr;
    NimBLEClient* client = nullptr;
    XboxState state;
    XboxStatusListener* listener = nullptr;
    State connectionState = State::None;

    void start() {
        NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DEVICE);
        NimBLEDevice::init("TETRIS ESP32");
        NimBLEDevice::setOwnAddrType(BLE_OWN_ADDR_PUBLIC);
        NimBLEDevice::setSecurityAuth(true, false, false);
        NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    }

    bool connectToServer() {
        if (advertisedDevice == nullptr) {
            return false;
        }

        if (NimBLEDevice::getClientListSize()) {
            client = NimBLEDevice::getClientByPeerAddress(advertisedDevice->getAddress());
            if (client != nullptr) {
                client->connect();
            }
        }

        if (client == nullptr) {
            if (NimBLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS) {
                return false;
            }

            client = NimBLEDevice::createClient();
            client->setClientCallbacks(new ClientCallbacks(this), true);
            client->connect(advertisedDevice, true);
        }

        while (!client->isConnected()) {
            client->connect();
            delay(1000);
        }        

        for (auto service : *client->getServices(true)) {
            auto uuid = service->getUUID();
            if (!uuid.equals(serviceHid)) {
                continue;
            }

            for (auto characteristic : *service->getCharacteristics(true)) {
                handleSubscription(characteristic);
            }
        }
        return true;
    }

    void handleSubscription(NimBLERemoteCharacteristic* characteristic) {

        if (characteristic->canRead()) {
            auto str = characteristic->readValue();
            if (str.size() == 0) {
                str = characteristic->readValue();
            }
        }

        if (!characteristic->canNotify()) {
            return;
        }

        notify_callback notify([&](NimBLERemoteCharacteristic* remoteCharacteristic, uint8_t* data, size_t length, bool isNotify) {
            auto uuid = remoteCharacteristic->getRemoteService()->getUUID();
                        
            if (connectionState != State::Connected) {
                connectionState = State::Connected;
            }

            if (uuid.equals(serviceHid)) {
                state.parse(data, length);
                if (listener != nullptr) {
                    listener(state);
                }
            }
        });

        characteristic->subscribe(true, notify, true);
    }

    void reconnect() {
        if (connectionState == State::Connected) {
            return;
        }

        if (connectToServer()) {
            return;
        }
        
        if (advertisedDevice != nullptr) {
            NimBLEDevice::deleteBond(advertisedDevice->getAddress());
            advertisedDevice = nullptr;
        }
        if (connectionState != State::Scanning) {
            connectionState = State::Scanning;
            auto scanner = NimBLEDevice::getScan();
            scanner->setDuplicateFilter(false);
            scanner->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks(this));
            scanner->setInterval(97);
            scanner->setWindow(97);
            scanner->start(4);
        }        
    }
};

XboxController::XboxController() : impl{std::make_shared<XboxControllerImpl>()} {}

void XboxController::start() {
    impl->start();
}

bool XboxController::isConnected() {
    return impl->connectionState == State::Connected;
}

bool XboxController::isWaiting() {
    return impl->connectionState == State::Waiting;
}

void XboxController::reconnect() {
    impl->reconnect();
}

void XboxController::listen(XboxStatusListener& listener) {
    impl->listener = &listener;
}
