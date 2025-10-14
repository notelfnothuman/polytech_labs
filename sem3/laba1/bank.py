import sys

class Bank:
    def __init__(self):
        self.__bank_name = ""
        self.__deposit_amount = 0
        self.__deposit_size = 0
        self.__bank_rate = 0

    def get_bank_name(self):
        return self.__bank_name

    def set_bank_name(self, name):
        self.__bank_name = name

    def get_deposit_amount(self):
        return self.__deposit_amount

    def set_deposit_amount(self, number):
        self.__deposit_amount = number

    def get_deposit_size(self):
        return self.__deposit_size

    def set_deposit_size(self, size):
        self.__deposit_size = size

    def get_bank_rate(self):
        return self.__bank_rate

    def set_bank_rate(self, rate):
        self.__bank_rate = rate

    def total_payment(self):
        a = self.__deposit_amount * (self.__deposit_size * self.__bank_rate / 100)
        return a


def get_int(prompt, error_message, min_value=-sys.maxsize, max_value=sys.maxsize):
    while True:
        try:
            value = int(input(prompt))
            if min_value <= value <= max_value:
                return value
            else:
                print(error_message)
        except ValueError:
            print(error_message)


def get_string(prompt, error_message):
    while True:
        value = input(prompt).strip()
        if value:
            return value
        else:
            print(error_message)


def main():
    bank1 = Bank()

    bank1.set_bank_name(get_string("Введите название банка : ", "Введите корректное значение : "))
    bank1.set_deposit_amount(get_int("Введите количество вкладов : ", "Введите корректное значение : ", 1))
    bank1.set_deposit_size(get_int("Введите размер депозита : ", "Введите корректное значение : ", 1))
    bank1.set_bank_rate(get_int("Введите размер процентной ставки : ", "Введите корректное значение : ", 1, 1000))

    print()
    print("bank name", bank1.get_bank_name())
    print("Размер общей выплаты по процентам:", bank1.total_payment())


if __name__ == "__main__":
    main()
