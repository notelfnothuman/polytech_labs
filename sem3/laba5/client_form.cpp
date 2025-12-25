// ClientForm.cpp
#include "ClientForm.h"
#include "Client.h"

using namespace System::Windows::Forms;
using namespace System::Drawing;

ClientForm::ClientForm(bool isVIP) {
    this->isVIPMode = isVIP;
    this->originalClient = nullptr;
    InitializeComponent();
    this->Text = isVIP ? "новый VIP клиент" : "новый обычный клиент";
}

ClientForm::ClientForm(Client^ client) {
    this->originalClient = client;
    this->isVIPMode = client->IsVIP();
    InitializeComponent();
    this->Text = "редактировать клиента";
    txtName->Text = client->Name;
    txtRate->Text = client->Rate.ToString();
    txtAmount->Text = (isVIPMode ? (client->Amount - 1000).ToString() : client->Amount.ToString());
}

Client^ ClientForm::GetClient() {
    String^ name = txtName->Text->Trim();
    if (String::IsNullOrEmpty(name)) {
        MessageBox::Show("имя не может быть пустым.", "ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        return nullptr;
    }

    int rate, amount;
    if (!Int32::TryParse(txtRate->Text, rate) || !Int32::TryParse(txtAmount->Text, amount)) {
        MessageBox::Show("ставка и сумма должны быть целыми числами.", "ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        return nullptr;
    }

    if (rate <= 0) {
        MessageBox::Show("ставка должна быть положительной.");
        return nullptr;
    }
    if (rate > 100) {
        MessageBox::Show("ставка не может превышать 100%.");
        return nullptr;
    }
    if (amount <= 0) {
        MessageBox::Show("сумма вклада должна быть положительной.");
        return nullptr;
    }
    if (amount >= 10000000) {
        MessageBox::Show("вклад слишком большой (максимально 9 999 999).");
        return nullptr;
    }

    if (isVIPMode) {
        return gcnew VIPClient(name, rate, amount);
    }
    else {
        return gcnew SimpleClient(name, rate, amount);
    }
}

void ClientForm::InitializeComponent() {
    this->SuspendLayout();

    Label^ lblName = gcnew Label();
    lblName->Text = "имя:";
    lblName->Location = Drawing::Point(20, 20);
    lblName->AutoSize = true;

    Label^ lblRate = gcnew Label();
    lblRate->Text = "ставка (%):";
    lblRate->Location = Drawing::Point(20, 50);
    lblRate->AutoSize = true;

    Label^ lblAmount = gcnew Label();
    if (isVIPMode) {
        lblAmount->Text = "сумма вклада (+1000):";
    }
    else {
        lblAmount->Text = "сумма вклада:";
    }
    lblAmount->Location = Drawing::Point(20, 80);
    lblAmount->AutoSize = true;

    txtName = gcnew TextBox();
    txtName->Location = Drawing::Point(120, 20);
    txtName->Width = 200;

    txtRate = gcnew TextBox();
    txtRate->Location = Drawing::Point(120, 50);
    txtRate->Width = 100;

    txtAmount = gcnew TextBox();
    txtAmount->Location = Drawing::Point(140, 80);
    txtAmount->Width = 100;

    btnOK = gcnew Button();
    btnOK->Text = "OK";
    btnOK->Location = Drawing::Point(120, 120);
    btnOK->Click += gcnew EventHandler(this, &ClientForm::OnOK);

    btnCancel = gcnew Button();
    btnCancel->Text = "отмена";
    btnCancel->Location = Drawing::Point(200, 120);
    btnCancel->Click += gcnew EventHandler(this, &ClientForm::OnCancel);

    this->Controls->AddRange(gcnew array<Control^>{
        lblName, lblRate, lblAmount, txtName, txtRate, txtAmount, btnOK, btnCancel
    });

    this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
    this->MaximizeBox = false;
    this->MinimizeBox = false;
    this->Size = Drawing::Size(340, 180);
    this->StartPosition = FormStartPosition::CenterParent;

    this->ResumeLayout(false);
}

void ClientForm::OnOK(System::Object^ sender, System::EventArgs^ e) {
    this->DialogResult = System::Windows::Forms::DialogResult::OK;
    this->Close();
}

void ClientForm::OnCancel(System::Object^ sender, System::EventArgs^ e) {
    this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
    this->Close();
}
