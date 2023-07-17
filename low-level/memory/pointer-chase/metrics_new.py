import csv
import datetime
import math
import time


class Address(object):
    def __init__(self, address_line, zipcode):
        self.address_line = address_line
        self.zipcode = zipcode


class DollarAmount(object):
    def __init__(self, dollars, cents):
        self.dollars = dollars
        self.cents = cents


class Payment(object):
    def __init__(self, dollar_amount, time):
        self.amount = dollar_amount
        self.time = time


class User(object):
    def __init__(self, user_id, name, age, address, payments):
        self.user_id = user_id
        self.name = name
        self.age = age
        self.address = address
        self.payments = payments


def payment_to_dollars(p: Payment):
    return float(p.amount.dollars) + float(p.amount.cents) / 100


def average_age(ages):
    return sum(ages) / len(ages)


def average_payment_amount(users, payments):
    return sum(payments) / len(payments)


def stddev_payment_amount(users, payments):
    mean = average_payment_amount(users, payments)
    count = len(payments)
    squared_diffs = sum([(p - mean) ** 2 for p in payments])
    return math.sqrt(squared_diffs / count)


def load_data():
    users = {}
    ages = []
    payments = []
    with open("users.csv") as f:
        for line in csv.reader(f):
            uid, name, age, address_line, zip_code = line
            addr = Address(address_line, zip_code)
            users[int(uid)] = User(int(uid), name, int(age), addr, [])
            ages.append(int(age))
    with open("payments.csv") as f:
        for line in csv.reader(f):
            amount, timestamp, uid = line
            payment = Payment(
                DollarAmount(dollars=int(amount) // 100, cents=int(amount) % 100),
                time=datetime.datetime.fromisoformat(timestamp),
            )
            payments.append(payment_to_dollars(payment))
            users[int(uid)].payments.append(payment)
    return users, payments, ages


if __name__ == "__main__":
    t = time.perf_counter()
    users, payments, ages = load_data()
    print(f"Data loading: {time.perf_counter() - t:.3f}s")
    t = time.perf_counter()
    assert abs(average_age(ages) - 59.626) < 0.01
    assert abs(stddev_payment_amount(users, payments) - 288684.849) < 0.01
    assert abs(average_payment_amount(users, payments) - 499850.559) < 0.01
    print(f"Computation {time.perf_counter() - t:.3f}s")
