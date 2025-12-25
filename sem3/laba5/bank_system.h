#pragma once
// BankSystem.h
#pragma once
#include "Client.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;

ref class BankSystem {
private:
    List<Client^>^ clients;

public:
    BankSystem();
    void AddClient(Client^ c);
    void RemoveClient(int index);
    void UpdateClient(int index, Client^ c);
    List<Client^>^ GetClients();
    double CalculateTotalIncome();
    void SaveToFile(String^ filename);
    void LoadFromFile(String^ filename);
};
