#include <algorithm>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#pragma execution_character_set("utf-8")
#endif

// Інтерфейс спостерігача
class IObserver {
public:
    virtual void Update(float temperature) = 0;
    virtual ~IObserver() = default;
};

// Інтерфейс суб'єкта
class ISubject {
public:
    virtual void Attach(IObserver* observer) = 0;
    virtual void Detach(IObserver* observer) = 0;
    virtual void Notify() = 0;
    virtual ~ISubject() = default;
};

// SensorHub хаб який зберігає температуру і список спостерігачів.
class SensorHub : public ISubject {
private:
    std::vector<IObserver*> observers;
    float temperature = 0.0f;

public:
    void Attach(IObserver* observer) override {
        // Перевірка не дозволяє підписати той самий пристрій двічі.
        const auto found = std::find(observers.begin(), observers.end(), observer);
        if (found == observers.end()) {
            observers.push_back(observer);
        }
    }

    void Detach(IObserver* observer) override {
        const auto found = std::find(observers.begin(), observers.end(), observer);
        if (found != observers.end()) {
            observers.erase(found);
        }
    }

    void Notify() override {
        // Надсилаємо поточну температуру кожному підписаному пристрою.
        for (IObserver* observer : observers) {
            observer->Update(temperature);
        }
    }

    void SetTemperature(float newTemperature) {
        temperature = newTemperature;
        std::cout << "\nSensorHub: температура " << temperature << " C\n";
        Notify();
    }
};

// AirConditioner спостерігач що керує кондиціонером.
class AirConditioner : public IObserver {
public:
    void Update(float temperature) override {
        if (temperature > 25.0f) {
            std::cout << "AirConditioner: кондиціонер увімкнено.\n";
        }
        else {
            std::cout << "AirConditioner: кондиціонер вимкнено.\n";
        }
    }
};

// SmartDisplay спостерігач що показує температуру на дисплеї.
class SmartDisplay : public IObserver {
public:
    void Update(float temperature) override {
        std::cout << "SmartDisplay: поточна температура — "
            << temperature << " C.\n";
    }
};

int main() {
#ifdef _WIN32

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    SensorHub hub;
    AirConditioner airConditioner;
    SmartDisplay display;

    // Обидва пристрої підписуються на повідомлення центрального хаба.
    hub.Attach(&display);
    hub.Attach(&airConditioner);

    hub.SetTemperature(22.0f); // Дисплей покаже 22 C кондиціонер буде вимкнено.
    hub.SetTemperature(28.0f); // Дисплей покаже 28 C кондиціонер увімкнеться.

    // Від'єднаний дисплей більше не отримує повідомлень.
    std::cout << "\n--- Від'єднуємо SmartDisplay ---\n";
    hub.Detach(&display);
    hub.SetTemperature(30.0f); // Оновлення отримає лише кондиціонер.

    // Пристрій можна знову під'єднати під час роботи програми.
    std::cout << "\n--- Знову під'єднуємо SmartDisplay ---\n";
    hub.Attach(&display);
    hub.SetTemperature(20.0f);

    return 0;
}
