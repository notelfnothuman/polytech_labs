// BankSystem.cpp
#include "BankSystem.h"

BankSystem::BankSystem() {
    clients = gcnew List<Client^>();
}

void BankSystem::AddClient(Client^ c) {
    if (c != nullptr) clients->Add(c);
}

void BankSystem::RemoveClient(int index) {
    if (index >= 0 && index < clients->Count)
        clients->RemoveAt(index);
}

void BankSystem::UpdateClient(int index, Client^ c) {
    if (index >= 0 && index < clients->Count && c != nullptr)
        clients[index] = c;
}

List<Client^>^ BankSystem::GetClients() {
    return clients;
}

double BankSystem::CalculateTotalIncome() {
    double total = 0.0;
    for each (Client ^ c in clients) {
        total += c->Calculate();
    }
    return total;
}

void BankSystem::SaveToFile(String^ filename) {
    StreamWriter^ sw = gcnew StreamWriter(filename, false, Encoding::UTF8);
    for each (Client ^ c in clients) {
        sw->WriteLine("{0},{1},{2},{3}",
            c->IsVIP() ? "VIP" : "Simple",
            c->Name,
            c->Rate,
            c->IsVIP() ? c->Amount - 1000 : c->Amount
        );
    }
    sw->Close();
}

void BankSystem::LoadFromFile(String^ filename) {
    clients->Clear();
    StreamReader^ sr = gcnew StreamReader(filename, Encoding::UTF8);
    String^ line;
    while ((line = sr->ReadLine()) != nullptr) {
        array<String^>^ parts = line->Split(',');
        if (parts->Length == 4) {
            bool isVIP = (parts[0] == "VIP");
            String^ name = parts[1];
            int rate, amount;
            if (Int32::TryParse(parts[2], rate) && Int32::TryParse(parts[3], amount)) {
                if (isVIP)
                    AddClient(gcnew VIPClient(name, rate, amount));
                else
                    AddClient(gcnew SimpleClient(name, rate, amount));
            }
        }
    }
    sr->Close();
}
