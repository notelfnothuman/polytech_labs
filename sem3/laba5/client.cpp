// Client.cpp
#include "Client.h"

SimpleClient::SimpleClient(String^ name, int rate, int amount) {
    Name = name; Rate = rate; Amount = amount;
}

double SimpleClient::Calculate() {
    return static_cast<double>(Rate * Amount) / 100.0;
}

String^ SimpleClient::GetInfo() {
    return "обычный клиент: " + Name + ", ставка: " + Rate + "%, вклад: " + Amount;
}

bool SimpleClient::IsVIP() {
    return false;
}

VIPClient::VIPClient(String^ name, int rate, int amount) {
    Name = name; Rate = rate; Amount = amount + 1000;
}

double VIPClient::Calculate() {
    return static_cast<double>(Rate * Amount) / 100.0;
}

String^ VIPClient::GetInfo() {
    return "VIP клиент: " + Name + ", ставка: " + Rate + "%, вклад: " + Amount;
}

bool VIPClient::IsVIP() {
    return true;
}
