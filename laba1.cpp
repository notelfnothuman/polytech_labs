//
#include <string>
#include <limits>
#include <locale>    
#include <codecvt> 
#include <iostream>
using namespace std;

class Bank {
private:
    string bankName ="";
    int depositAmount =0;
    int depositSize =0;
    int bankRate =0;
public:
    string getBankName() {
        return bankName;
    }
    void setBankName(string name) {
        bankName = name;
    }

    int getDepositAmount() {
        return depositAmount;
    }
    void setDepositAmount(int number) {
        depositAmount = number;
    }

    int getDepositSize() {
        return depositSize;
    }
    void setDepositSize(int size) {
        depositSize = size;
    }

    int getBankRate() {
        return bankRate;
    }
    void setBankRate(int rate) {
        bankRate = rate;
    }

    float totalPayment() {
        float a = depositAmount * (depositSize * bankRate / 100);
        return a;
    }
};

int getInt(string textnavvode, string errormessage, int minValue = numeric_limits<int>::min(), int maxValue = numeric_limits<int>::max()) {
    int value;
    cout << textnavvode;
    cin >> value;
    while (cin.fail() || value > maxValue || value < minValue) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << errormessage;
        cin >> value;
    }
    return value;
}

string getString(string textnavvode, string errormessage) {
    string value;
    cout << textnavvode;
    cin >> value;
    while (value.empty()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << errormessage << endl;
        cin >> value;
    }
    return value;
}

int main(){
    setlocale(0, "Russian");
    Bank Bank1;
    Bank1.setBankName(getString("Введите название банка : ", "Введите корректное значение : "));
    Bank1.setDepositAmount(getInt("Введите количество вкладов : ", "Введите корректное значение : ", 1));
    Bank1.setDepositSize(getInt("Введите размер депозита : ", "Введите корректное значение : ", 1));
    Bank1.setBankRate(getInt("Введите размер процентной ставки : ", "Введите корректное значение : ", 1, 1000));
    cout << endl;
    cout << "bank name " << Bank1.getBankName() << endl;
    cout << "Размер общей выплаты по процентам: " << Bank1.totalPayment() << endl;
}


//5. Предметная область: Банк. 
// В классе хранить информацию о 
// наименовании банка, 
// числе вкладов, 
// размере вклада (все вклады одинаковые), 
// размере процентной ставки. 
// Реализовать метод для подсчета общей выплаты по процентам. 
