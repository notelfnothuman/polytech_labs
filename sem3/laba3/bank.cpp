// laba3sem3aip.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>   

class BonusStrategy {
public:
    virtual double calculateFinalAmount(double baseAmount) const = 0;

    virtual std::string getName() const = 0;

    virtual ~BonusStrategy() = default;
};

// без бонуса
class NoBonusStrategy : public BonusStrategy {
public:
    double calculateFinalAmount(double baseAmount) const override {
        return baseAmount;
    }

    std::string getName() const override {
        return "без бонуса";
    }
};

//  фиксированный бонус
class FixedBonusStrategy : public BonusStrategy {
private:
    double bonus; // фиксированная сумма, добавляется к вкладу
public:
    explicit FixedBonusStrategy(double bonusAmount)
        : bonus(bonusAmount) {
    }

    double calculateFinalAmount(double baseAmount) const override {
        return baseAmount + bonus;
    }

    std::string getName() const override {
        return "фиксированный бонус (" + std::to_string(bonus) + ")";
    }
};

//  вкладчик
class Depositor {
private:
    std::string name;   
    double amount;      

public:
    Depositor(const std::string& depositorName,
        double baseAmount,
        const BonusStrategy& strategy)
        : name(depositorName)
    {
        //полиморфизм
        amount = strategy.calculateFinalAmount(baseAmount);
    }

    const std::string& getName() const {
        return name;
    }

    double getAmount() const {
        return amount;
    }
};

// банк
class Bank {
private:
    std::vector<Depositor> depositors; 

public:
    void addDepositor(const Depositor& d) {
        depositors.push_back(d);
    }

    double getTotal() const {
        double sum = 0.0;
        for (const auto& d : depositors) {
            sum += d.getAmount();
        }
        return sum;
    }

    void printAll() const {
        if (depositors.empty()) {
            std::cout << "в банке пока нет вкладчиков.\n";
            return;
        }
        std::cout << "список вкладчиков:\n";
        for (const auto& d : depositors) {
            std::cout << "имя: " << d.getName()
                << ", вклад: " << d.getAmount() << "\n";
        }
    }
};

// безопаснsq ввод

//корректное число 
bool isValidNumberString(const std::string& s) {
    bool hasDot = false;
    if (s.empty()) return false;

    size_t start = 0;
    if (s[0] == '+' || s[0] == '-') {
        if (s.size() == 1) return false; 
        start = 1;
    }

    for (size_t i = start; i < s.size(); ++i) {
        if (s[i] == '.') {
            if (hasDot) {
                return false;
            }
            hasDot = true;
        }
        else if (!std::isdigit(static_cast<unsigned char>(s[i]))) {
            return false;
        }
    }

    return true;
}

//буквы и лишние символы
double readDouble(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);

        try {
            if (!isValidNumberString(input)) {
                throw std::invalid_argument("введено нечисловое значение.");
            }
            double value = std::stod(input);
            if (value < 0) {
                throw std::invalid_argument("сумма не может быть отрицательной.");
            }
            return value;
        }
        catch (const std::invalid_argument& e) {
            std::cout << "ошибка: " << e.what()
                << " попробуйте ещё раз.\n\n";
        }
        catch (const std::out_of_range&) {
            std::cout << "слишком большое число, попробуйте ещё раз.\n\n";
        }
    }
}

int readInt(const std::string& prompt, int minValue, int maxValue) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);

        try {
            if (!isValidNumberString(input)) {
                throw std::invalid_argument("введено нецелое число.");
            }
            int value = std::stoi(input);

            if (value < minValue || value > maxValue) {
                throw std::out_of_range("число вне допустимого диапазона.");
            }

            return value;
        }
        catch (const std::invalid_argument& e) {
            std::cout << "ошибка: " << e.what()
                << " попробуйте ещё раз.\n\n";
        }
        catch (const std::out_of_range& e) {
            std::cout << "ошибка: " << e.what()
                << " допустимый диапазон: от "
                << minValue << " до " << maxValue << ".\n\n";
        }
    }
}

//  меню

void printMenu() {
    std::cout << "\n--- МЕНЮ БАНКА ---\n"
        << "1. добавить вкладчика\n"
        << "2. показать всех вкладчиков\n"
        << "3. показать общую сумму вкладов\n"
        << "0. выход\n";
}

int main() {
    setlocale(LC_ALL, "Russian");
    Bank bank; 

    while (true) {
        printMenu();
        int choice = readInt("ваш выбор: ", 0, 3);

        if (choice == 0) {
            std::cout << "выход из программы.\n";
            break;
        }
        else if (choice == 1) {
            std::cout << "введите имя вкладчика: ";
            std::string name;
            std::getline(std::cin, name);

            double baseAmount = readDouble("введите сумму вклада: ");

            std::cout << "выберите тип бонуса:\n"
                << "1. без бонуса\n"
                << "2. фиксированный бонус (f.e. 500 ед)\n";

            int bonusChoice = readInt("Ваш выбор: ", 1, 2);

            NoBonusStrategy noBonus;
            FixedBonusStrategy fixedBonus(500.0); 

            const BonusStrategy* strategyPtr = nullptr;

            if (bonusChoice == 1) {
                strategyPtr = &noBonus;
            }
            else {
                strategyPtr = &fixedBonus;
            }

            std::cout << "вы выбрали: "
                << strategyPtr->getName() << "\n";

            Depositor d(name, baseAmount, *strategyPtr);
            bank.addDepositor(d);

            std::cout << "вкладчик успешно добавлен.\n";
        }
        else if (choice == 2) {
            bank.printAll();
        }
        else if (choice == 3) {
            double total = bank.getTotal();
            std::cout << "общая сумма вкладов: " << total << "\n";
        }
    }

    return 0;
}

