// Form1.cpp
#include "Form1.h"
#include "ClientForm.h"
#include "BankSystem.h"

using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Collections::Generic;

Form1::Form1() {
    bank = gcnew BankSystem();
    InitializeComponent();
    RefreshGrid();
}

void Form1::InitializeComponent() {
    this->SuspendLayout();

    dataGridView = gcnew DataGridView();
    lblTotal = gcnew Label();

    btnAddSimple = gcnew Button(); btnAddSimple->Text = "обычный";
    btnAddVIP = gcnew Button(); btnAddVIP->Text = "VIP";
    btnEdit = gcnew Button(); btnEdit->Text = "изменить";
    btnDelete = gcnew Button(); btnDelete->Text = "удалить";
    btnLoad = gcnew Button(); btnLoad->Text = "загрузить";
    btnSave = gcnew Button(); btnSave->Text = "сохранить";
    btnSort = gcnew Button(); btnSort->Text = "сортировка";

    // DataGridView
    dataGridView->Dock = DockStyle::Top;
    dataGridView->Height = 320;
    dataGridView->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
    dataGridView->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    dataGridView->MultiSelect = false;
    dataGridView->Columns->Add("Type", "тип");
    dataGridView->Columns->Add("Name", "имя");
    dataGridView->Columns->Add("Rate", "ставка (%)");
    dataGridView->Columns->Add("Amount", "вклад");
    dataGridView->Columns->Add("Income", "доход");

    // Total label
    lblTotal->Text = "общий доход: 0.00";
    lblTotal->Dock = DockStyle::Bottom;
    lblTotal->TextAlign = ContentAlignment::MiddleLeft;
    lblTotal->BackColor = Color::LightGray;
    lblTotal->Height = 30;

    // Button events
    btnAddSimple->Click += gcnew EventHandler(this, &Form1::OnAddSimple);
    btnAddVIP->Click += gcnew EventHandler(this, &Form1::OnAddVIP);
    btnEdit->Click += gcnew EventHandler(this, &Form1::OnEdit);
    btnDelete->Click += gcnew EventHandler(this, &Form1::OnDelete);
    btnLoad->Click += gcnew EventHandler(this, &Form1::OnLoad);
    btnSave->Click += gcnew EventHandler(this, &Form1::OnSave);
    btnSort->Click += gcnew EventHandler(this, &Form1::OnSortByName);

    // Panel
    FlowLayoutPanel^ panel = gcnew FlowLayoutPanel();
    panel->Dock = DockStyle::Bottom;
    panel->Padding = System::Windows::Forms::Padding(5);
    panel->Controls->AddRange(gcnew array<Control^>{
        btnAddSimple, btnAddVIP, btnEdit, btnDelete,
            btnLoad, btnSave, btnSort
    });

    // Final layout
    this->Controls->AddRange(gcnew array<Control^>{ dataGridView, panel, lblTotal });
    this->Text = "банковская система - лаба 4";
    this->Size = Drawing::Size(850, 520);
    this->StartPosition = FormStartPosition::WindowsDefaultBounds;

    this->ResumeLayout(false);
    dataGridView->AllowUserToAddRows = false;
}

void Form1::RefreshGrid() {
    dataGridView->Rows->Clear();
    for each (Client ^ c in bank->GetClients()) {
        dataGridView->Rows->Add(
            c->IsVIP() ? "VIP" : "обычный",
            c->Name,
            c->Rate.ToString(),
            c->Amount.ToString(),
            c->Calculate().ToString("F2")
        );
    }
    UpdateTotalLabel();
}

void Form1::UpdateTotalLabel() {
    lblTotal->Text = "общий доход: " + bank->CalculateTotalIncome().ToString("F2");
}

void Form1::OnAddSimple(System::Object^ sender, System::EventArgs^ e) {
    ClientForm^ form = gcnew ClientForm(false);
    if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        Client^ c = form->GetClient();
        if (c != nullptr) {
            bank->AddClient(c);
            RefreshGrid();
        }
    }
}

void Form1::OnAddVIP(System::Object^ sender, System::EventArgs^ e) {
    ClientForm^ form = gcnew ClientForm(true);
    if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        Client^ c = form->GetClient();
        if (c != nullptr) {
            bank->AddClient(c);
            RefreshGrid();
        }
    }
}

void Form1::OnEdit(System::Object^ sender, System::EventArgs^ e) {
    // проверка выбрана ли строка
    if (dataGridView->SelectedRows->Count == 0) {
        MessageBox::Show(
            "выберите клиента из списка для редактирования.",
            "нет выбора",
            MessageBoxButtons::OK,
            MessageBoxIcon::Warning
        );
        return;
    }

    int idx = dataGridView->SelectedRows[0]->Index;
    ClientForm^ form = gcnew ClientForm(bank->GetClients()[idx]);

    if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        Client^ updatedClient = form->GetClient();
        if (updatedClient != nullptr) {
            bank->UpdateClient(idx, updatedClient);
            RefreshGrid();
        }
    }
}

void Form1::OnDelete(System::Object^ sender, System::EventArgs^ e) {
    if (dataGridView->SelectedRows->Count == 0) return;
    if (MessageBox::Show("удалить клиента?", "подтверждение", MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::Yes) {
        bank->RemoveClient(dataGridView->SelectedRows[0]->Index);
        RefreshGrid();
    }
}

void Form1::OnLoad(System::Object^ sender, System::EventArgs^ e) {
    OpenFileDialog^ ofd = gcnew OpenFileDialog();
    ofd->Filter = "CSV файлы (*.csv)|*.csv";
    if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        try {
            bank->LoadFromFile(ofd->FileName);
            RefreshGrid();
        }
        catch (Exception^ ex) {
            MessageBox::Show("ошибка загрузки: " + ex->Message);
        }
    }
}

void Form1::OnSave(System::Object^ sender, System::EventArgs^ e) {
    SaveFileDialog^ sfd = gcnew SaveFileDialog();
    sfd->Filter = "CSV файлы (*.csv)|*.csv";
    sfd->DefaultExt = "csv";
    if (sfd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        try {
            bank->SaveToFile(sfd->FileName);
            MessageBox::Show("сохранено");
        }
        catch (Exception^ ex) {
            MessageBox::Show("ошибка сохранения: " + ex->Message);
        }
    }
}

void Form1::OnSortByName(System::Object^ sender, System::EventArgs^ e) {
    auto list = bank->GetClients();
    int n = list->Count;
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (String::Compare(list[i]->Name, list[j]->Name, true) > 0) {
                Client^ tmp = list[i];
                list[i] = list[j];
                list[j] = tmp;
            }
        }
    }
    RefreshGrid();
}
