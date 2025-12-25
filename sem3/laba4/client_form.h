#pragma once
// ClientForm.h
#pragma once
ref class Client;

using namespace System;
using namespace System::Windows::Forms;

public ref class ClientForm : public Form {
private:
    TextBox^ txtName, ^ txtRate, ^ txtAmount;
    Button^ btnOK, ^ btnCancel;
    bool isVIPMode;
    Client^ originalClient;

    void InitializeComponent();
    void OnOK(System::Object^ sender, System::EventArgs^ e);
    void OnCancel(System::Object^ sender, System::EventArgs^ e);

public:
    ClientForm(bool isVIP);
    ClientForm(Client^ client);
    Client^ GetClient();
};
