// BankSystem.cpp
#include "BankSystem.h"
#include "sqlite3.h"
#include <string>
#include <cstring>

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

static std::string StringToUTF8(System::String^ str) {
    if (str == nullptr) return "";
    array<Byte>^ bytes = Encoding::UTF8->GetBytes(str);
    pin_ptr<Byte> pinned = &bytes[0];
    return std::string((char*)pinned, bytes->Length);
}

static System::String^ UTF8ToString(const char* utf8) {
    if (!utf8) return "";
    return gcnew System::String((char*)utf8, 0, (int)strlen(utf8), Encoding::UTF8);
}

void BankSystem::SaveToFile(String^ filename) {
    // filename = путь к .db

    sqlite3* db = nullptr;
    int rc = sqlite3_open(StringToUTF8(filename).c_str(), &db);

    if (rc != SQLITE_OK) {
        String^ msg = "не удалось создать/открыть базу: " + filename + "\n" + UTF8ToString(sqlite3_errmsg(db));
        sqlite3_close(db);
        throw gcnew Exception(msg);
    }

    // Таблица клиентов
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS Clients (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            is_vip INTEGER NOT NULL,
            name TEXT NOT NULL,
            rate INTEGER NOT NULL,
            amount_base INTEGER NOT NULL
        );
    )";

    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        String^ msg = "ошибка создания таблицы: " + UTF8ToString(sqlite3_errmsg(db));
        sqlite3_close(db);
        throw gcnew Exception(msg);
    }

    // как в примере: чистим старое
    sqlite3_exec(db, "DELETE FROM Clients;", nullptr, nullptr, nullptr);

    const char* insertSQL =
        "INSERT INTO Clients (is_vip, name, rate, amount_base) VALUES (?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        String^ msg = "ошибка подготовки запроса: " + UTF8ToString(sqlite3_errmsg(db));
        sqlite3_close(db);
        throw gcnew Exception(msg);
    }

    try {
        for each (Client ^ c in clients) {
            int isVip = c->IsVIP() ? 1 : 0;

            // ВАЖНО: в БД кладём "чистую" сумму, без +1000
            int amountBase = isVip ? (c->Amount - 1000) : c->Amount;

            sqlite3_bind_int(stmt, 1, isVip);
            sqlite3_bind_text(stmt, 2, StringToUTF8(c->Name).c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 3, c->Rate);
            sqlite3_bind_int(stmt, 4, amountBase);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                throw gcnew Exception("ошибка вставки клиента: " + UTF8ToString(sqlite3_errmsg(db)));
            }

            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
        }
    }
    catch (Exception^) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        throw;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void BankSystem::LoadFromFile(String^ filename) {
    clients->Clear();

    sqlite3* db = nullptr;
    int rc = sqlite3_open(StringToUTF8(filename).c_str(), &db);

    if (rc != SQLITE_OK) {
        String^ msg = "не удалось открыть базу: " + filename + "\n" + UTF8ToString(sqlite3_errmsg(db));
        sqlite3_close(db);
        throw gcnew Exception(msg);
    }

    // на всякий случай создадим таблицу
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS Clients (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            is_vip INTEGER NOT NULL,
            name TEXT NOT NULL,
            rate INTEGER NOT NULL,
            amount_base INTEGER NOT NULL
        );
    )";

    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        String^ msg = "ошибка создания таблицы: " + UTF8ToString(sqlite3_errmsg(db));
        sqlite3_close(db);
        throw gcnew Exception(msg);
    }

    const char* selectSQL =
        "SELECT is_vip, name, rate, amount_base FROM Clients ORDER BY id;";

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        String^ msg = "ошибка запроса к базе: " + UTF8ToString(sqlite3_errmsg(db));
        sqlite3_close(db);
        throw gcnew Exception(msg);
    }

    try {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int isVip = sqlite3_column_int(stmt, 0);
            const char* nameUtf8 = (const char*)sqlite3_column_text(stmt, 1);
            int rate = sqlite3_column_int(stmt, 2);
            int amountBase = sqlite3_column_int(stmt, 3);

            String^ name = UTF8ToString(nameUtf8);

            if (isVip)
                AddClient(gcnew VIPClient(name, rate, amountBase));
            else
                AddClient(gcnew SimpleClient(name, rate, amountBase));
        }
    }
    catch (Exception^) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        throw;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
