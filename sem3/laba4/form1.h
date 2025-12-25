// Form1.h
#pragma once

// Forward declaration, чтобы избежать циклических зависимостей
ref class BankSystem;

using namespace System;
using namespace System::Windows::Forms;

public ref class Form1 : public Form {
private:
    BankSystem^ bank;

    DataGridView^ dataGridView;
    Label^ lblTotal;

    Button^ btnAddSimple;
    Button^ btnAddVIP;
    Button^ btnEdit;
    Button^ btnDelete;
    Button^ btnLoad;
    Button^ btnSave;
    Button^ btnSort;

    // Только объявления методов — без тел!
    void InitializeComponent();
    void RefreshGrid();
    void UpdateTotalLabel();

    // Обработчики событий — только сигнатуры
    void OnAddSimple(System::Object^ sender, System::EventArgs^ e);
    void OnAddVIP(System::Object^ sender, System::EventArgs^ e);
    void OnEdit(System::Object^ sender, System::EventArgs^ e);
    void OnDelete(System::Object^ sender, System::EventArgs^ e);
    void OnLoad(System::Object^ sender, System::EventArgs^ e);
    void OnSave(System::Object^ sender, System::EventArgs^ e);
    void OnSortByName(System::Object^ sender, System::EventArgs^ e);

public:
    Form1(); // только объявление конструктора
};
