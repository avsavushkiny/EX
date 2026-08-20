#pragma once

#include <Arduino.h>
#include <vector>
#include <functional>
#include <map>
#include <sstream>

// Типы команд
enum class CommandType
{
    SYSTEM,  // Системные команды
    FORM,    // Управление формами
    TASK,    // Управление задачами
    DEBUG,   // Отладочные команды
    UI,      // UI управление
    DATA,    // Данные и вывод
    WIFI,    // WiFi управление
    PLOTTER, // Управление плоттером
    TEST     // Тестовые команды
};

// Структура команды
struct ConsoleCommand
{
    String name;
    String description;
    CommandType type;
    std::function<void(const std::vector<String> &args)> handler;
    std::vector<String> aliases;
    int minArgs = 0;
    int maxArgs = -1; // -1 = неограниченно
};

// Структура для хранения данных форм и элементов
struct FormData
{
    String name;
    void *pointer;
    std::vector<String> properties;
    bool isActive;
    bool isMinimized;
    int zOrder;
};

// Структура для вывода данных
struct DataOutput
{
    String format;
    std::map<String, String> fields;
};