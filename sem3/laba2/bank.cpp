#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <iomanip>
#include <regex>

//типы вкладов
enum class DepositKind {
    FIXED = 1,     // срочный 
    SAVINGS = 2,   // накопительный
    LONG_TERM = 3  // долгосрочный
};

std::string depositKindToString(DepositKind k) {
    switch (k) {
    case DepositKind::FIXED:     return "срочный";
    case DepositKind::SAVINGS:   return "накопительный";
    case DepositKind::LONG_TERM: return "долгосрочный";
    default: return "неизвестный";
    }
}

class Client {
private:
    std::string fullName;
    std::string passport;   
    bool hasDepositFlag{ false };
public:
    Client() = default;
    Client(const std::string& name, const std::string& pass)
        : fullName(name), passport(pass), hasDepositFlag(false) {
    }

    const std::string& getName() const { return fullName; }
    const std::string& getPassport() const { return passport; }
    bool hasDeposit() const { return hasDepositFlag; }
    void setHasDeposit(bool v) { hasDepositFlag = v; }
};

// контейнеризируемый классс
class Deposit {
private:
    std::string clientPassport; 
    DepositKind kind{ DepositKind::FIXED };
    double amount{ 0.0 };         
public:
    Deposit() = default;
    Deposit(const std::string& passport, DepositKind k, double initial)
        : clientPassport(passport), kind(k), amount(initial) {
    }

    const std::string& getClientPassport() const { return clientPassport; }
    DepositKind getKind() const { return kind; }
    double getAmount() const { return amount; }

    bool topUp(double value) {
        if (value <= 0) return false;
        amount += value;
        return true;
    }

    // годовые проценты по ставке
    double computeYearInterest(double rate) const {
        if (rate < 0) return 0.0;
        return amount * rate;
    }
};

// таблица ставок - в долях
class RateTable {
private:
    std::map<DepositKind, double> rates; 
public:
    RateTable() {
        rates[DepositKind::FIXED] = 0.08; // 8%
        rates[DepositKind::SAVINGS] = 0.06; // 6%
        rates[DepositKind::LONG_TERM] = 0.10; // 10%
    }

    void setRate(DepositKind k, double r) { rates[k] = r; }

    double getRate(DepositKind k) const {
        auto it = rates.find(k);
        return (it != rates.end()) ? it->second : 0.0;
    }

    void print() const {
        std::cout << "текущие годовые ставки:\n";
        std::cout << "  1) " << depositKindToString(DepositKind::FIXED)
            << " : " << getRate(DepositKind::FIXED) * 100 << "%\n";
        std::cout << "  2) " << depositKindToString(DepositKind::SAVINGS)
            << " : " << getRate(DepositKind::SAVINGS) * 100 << "%\n";
        std::cout << "  3) " << depositKindToString(DepositKind::LONG_TERM)
            << " : " << getRate(DepositKind::LONG_TERM) * 100 << "%\n";
    }
};

// Bank Singleton
class Bank {
private:
    static Bank* instance;

    std::map<std::string, Client> clientsByPassport; 
    std::vector<Deposit> deposits;                   
    RateTable rateTable;

    Bank() = default;
    Bank(const Bank&) = delete;
    Bank& operator=(const Bank&) = delete;

public:
    static Bank& getInstance() {
        if (!instance) instance = new Bank();
        return *instance;
    }

    static void destroyInstance() {
        instance = nullptr;
    }

    // деструктор
    ~Bank() {
        std::cout << "\n[~Bank] банк корректно завершил работу\n";
    }

    // операции над ставками 
    RateTable& rates() { return rateTable; }
    const RateTable& rates() const { return rateTable; }

    // операции с клиентами
    bool addClient(const std::string& name, const std::string& passport) {
        if (name.empty() || passport.empty()) return false;
        if (clientsByPassport.count(passport) > 0) return false; 
        clientsByPassport.emplace(passport, Client{ name, passport });
        return true;
    }

    bool hasClient(const std::string& passport) const {
        return clientsByPassport.count(passport) > 0;
    }

    const Client* getClient(const std::string& passport) const {
        auto it = clientsByPassport.find(passport);
        return (it != clientsByPassport.end()) ? &it->second : nullptr;
    }

    // операции со вкладами 
    bool openDeposit(const std::string& passport, DepositKind kind, double initial) {
        auto it = clientsByPassport.find(passport);
        if (it == clientsByPassport.end()) return false;         
        if (initial <= 0) return false;
        if (it->second.hasDeposit()) return false;                

        deposits.emplace_back(passport, kind, initial);
        it->second.setHasDeposit(true);
        return true;
    }

    // пополнить вклад 
    bool topUpDeposit(const std::string& passport, double value) {
        if (value <= 0) return false;
        int idx = findDepositIndexByPassport(passport);
        if (idx < 0) return false;
        return deposits[idx].topUp(value);
    }

    // общая сумма процентов по всем вкладам 
    double calcTotalYearInterest() const {
        double total = 0.0;
        for (const auto& d : deposits) {
            double rate = rateTable.getRate(d.getKind());
            total += d.computeYearInterest(rate);
        }
        return total;
    }

    void printClients() const {
        if (clientsByPassport.empty()) {
            std::cout << "клиентов пока нет.\n";
            return;
        }
        std::cout << "клиенты банка:\n";
        for (const auto& kv : clientsByPassport) {
            const auto& c = kv.second;
            std::cout << " - " << c.getName()
                << " | паспорт: " << c.getPassport()
                << " | вклад: " << (c.hasDeposit() ? "есть" : "нет")
                << "\n";
        }
    }

    void printDeposits() const {
        if (deposits.empty()) {
            std::cout << "вкладов пока нет.\n";
            return;
        }
        std::cout << "вклады:\n";
        for (const auto& d : deposits) {
            std::cout << " - паспорт: " << d.getClientPassport()
                << " | тип: " << depositKindToString(d.getKind())
                << " | сумма: " << std::fixed << std::setprecision(2) << d.getAmount()
                << "\n";
        }
    }

private:
    int findDepositIndexByPassport(const std::string& passport) const {
        for (size_t i = 0; i < deposits.size(); ++i) {
            if (deposits[i].getClientPassport() == passport) return static_cast<int>(i);
        }
        return -1;
    }
};

Bank* Bank::instance = nullptr;

// ВВОД/ПРОВЕРКИ 
void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool isInteger(const std::string& s) {
    return std::regex_match(s, std::regex("^[0-9]+$"));
}

bool isPositiveDouble(const std::string& s) {
    return std::regex_match(s, std::regex("^[0-9]+(\\.[0-9]+)?$"));
}

int readIntInRange(const std::string& prompt, int low, int high) {
    std::string input;
    int x;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        if (isInteger(input)) {
            x = std::stoi(input);
            if (x >= low && x <= high) return x;
        }
        std::cout << "введите целое число в диапазоне [" << low << ";" << high << "]\n";
    }
}

double readPositiveDouble(const std::string& prompt) {
    std::string input;
    double v;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        if (isPositiveDouble(input)) {
            v = std::stod(input);
            if (v > 0.0) return v;
        }
        std::cout << "введите положительное число\n";
    }
}

double readNonNegativeDouble(const std::string& prompt) {
    std::string input;
    double v;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        if (isPositiveDouble(input) || input == "0" || input == "0.0") {
            v = std::stod(input);
            if (v >= 0.0) return v;
        }
        std::cout << "введите число не меньше 0 \n";
    }
}

std::string readNonEmptyLine(const std::string& prompt) {
    std::string s;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, s);
        size_t b = s.find_first_not_of(" \t\r\n");
        size_t e = s.find_last_not_of(" \t\r\n");
        if (b != std::string::npos)
            s = s.substr(b, e - b + 1);
        else
            s.clear();

        if (!s.empty()) return s;
        std::cout << "строка не должна быть пустой\n";
    }
}

DepositKind readDepositKind() {
    std::cout << "выберите тип вклада:\n";
    std::cout << "  1) " << depositKindToString(DepositKind::FIXED) << "\n";
    std::cout << "  2) " << depositKindToString(DepositKind::SAVINGS) << "\n";
    std::cout << "  3) " << depositKindToString(DepositKind::LONG_TERM) << "\n";
    int c = readIntInRange("ваш выбор (1-3): ", 1, 3);
    return static_cast<DepositKind>(c);
}

//  МЕНЮ 
void printMenu() {
    std::cout << "\n--- М Е Н Ю ---\n";
    std::cout << "1. показать текущие ставки\n";
    std::cout << "2. изменить ставку по типу вклада\n";
    std::cout << "3. добавить клиента\n";
    std::cout << "4. открыть вклад клиенту\n";
    std::cout << "5. пополнить вклад\n";
    std::cout << "6. показать всех клиентов\n";
    std::cout << "7. показать все вклады\n";
    std::cout << "8. посчитать общую сумму годовых выплат по всем вкладам\n";
    std::cout << "0. выход\n";
    std::cout << "=============================\n";
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::cout << std::fixed << std::setprecision(2);

    Bank& bank = Bank::getInstance();

    bool running = true;
    while (running) {
        printMenu();
        int cmd = readIntInRange("команда: ", 0, 8);
        switch (cmd) {
        case 1: {
            bank.rates().print();
            break;
        }
        case 2: {
            auto kind = readDepositKind();
            double perc = readNonNegativeDouble("введите новую ставку в процентах (например, 7.5): ");
            bank.rates().setRate(kind, perc / 100.0);
            std::cout << "ставка обновлена.\n";
            break;
        }
        case 3: {
            std::string name = readNonEmptyLine("ФИО клиента: ");
            std::string pass = readNonEmptyLine("паспорт (уникально): ");
            if (bank.addClient(name, pass)) {
                std::cout << "клиент добавлен.\n";
            }
            else {
                std::cout << "клиент уже есть или введены некорректные данные\n";
            }
            break;
        }
        case 4: {
            std::string pass = readNonEmptyLine("паспорт клиента: ");
            if (!bank.hasClient(pass)) {
                std::cout << "такого клиента нет. сначала добавьте клиента\n";
                break;
            }
            auto kind = readDepositKind();
            double initial = readPositiveDouble("начальная сумма вклада (> 0): ");
            if (bank.openDeposit(pass, kind, initial)) {
                std::cout << "вклад успешно открыт.\n";
            }
            else {
                std::cout << "возможно у клиента уже есть вклад, или сумма некорректна\n";
            }
            break;
        }
        case 5: {
            std::string pass = readNonEmptyLine("паспорт клиента: ");
            double add = readPositiveDouble("сумма пополнения (> 0): ");
            if (bank.topUpDeposit(pass, add)) {
                std::cout << "вклад пополнен.\n";
            }
            else {
                std::cout << "вклад не найден или сумма некорректна\n";
            }
            break;
        }
        case 6: {
            bank.printClients();
            break;
        }
        case 7: {
            bank.printDeposits();
            break;
        }
        case 8: {
            double total = bank.calcTotalYearInterest();
            std::cout << "общая сумма выплат по процентам (за 1 год по текущим ставкам): "
                << total << " руб.\n";
            break;
        }
        case 0: {
            running = false;
            break;
        }
        default:
            std::cout << "неизвестная команда.\n";
        }
    }

    Bank::destroyInstance();
    return 0;
}

