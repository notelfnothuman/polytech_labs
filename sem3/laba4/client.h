#pragma once
// Client.h
#pragma once
using namespace System;

ref class Client {
public:
    property String^ Name;
    property int Rate;
    property int Amount;

    virtual double Calculate() = 0;
    virtual String^ GetInfo() = 0;
    virtual bool IsVIP() = 0;
};

ref class SimpleClient : public Client {
public:
    SimpleClient(String^ name, int rate, int amount);

    virtual double Calculate() override;
    virtual String^ GetInfo() override;
    virtual bool IsVIP() override;
};

ref class VIPClient : public Client {
public:
    VIPClient(String^ name, int rate, int amount);

    virtual double Calculate() override;
    virtual String^ GetInfo() override;
    virtual bool IsVIP() override;
};
