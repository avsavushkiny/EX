#include "consoleManager.h"
#include "exForm.h"
#include "taskDispatcher.h"

class ConsoleManager
{
private:
    static ConsoleManager *instance;
    std::map<String, ConsoleCommand> commands;
    bool isInteractiveMode = false;
    String currentPrompt = "Sozvezdiye> ";
    unsigned long lastCommandTime = 0;
    std::vector<String> commandHistory;
    int historyIndex = -1;

    // Буферы для данных
    std::map<String, FormData> formRegistry;
    std::vector<String> dataBuffers;

    ConsoleManager()
    {
        registerDefaultCommands();
    }

public:
    static ConsoleManager *getInstance()
    {
        if (!instance)
        {
            instance = new ConsoleManager();
        }
        return instance;
    }

    // Основной цикл обработки
    void process()
    {
        if (Serial.available() > 0)
        {
            String input = Serial.readStringUntil('\n');
            input.trim();

            if (input.length() > 0)
            {
                processCommand(input);
                lastCommandTime = millis();
            }
        }
    }

    // Регистрация команды
    void registerCommand(const ConsoleCommand &cmd)
    {
        commands[cmd.name] = cmd;
        for (const auto &alias : cmd.aliases)
        {
            commands[alias] = cmd;
        }
    }

    // Обработка команды
    void processCommand(const String &input)
    {
        // История команд
        commandHistory.push_back(input);
        historyIndex = commandHistory.size() - 1;

        // Парсинг аргументов
        std::vector<String> args = parseArguments(input);
        if (args.empty())
            return;

        String cmdName = args[0];
        cmdName.toLowerCase();
        args.erase(args.begin());

        // Поиск команды
        auto it = commands.find(cmdName);
        if (it != commands.end())
        {
            ConsoleCommand &cmd = it->second;

            // Проверка аргументов
            if (cmd.minArgs >= 0 && (int)args.size() < cmd.minArgs)
            {
                Serial.println("Error: Too few arguments");
                Serial.print("Usage: " + cmd.name + " " + cmd.description);
                return;
            }
            if (cmd.maxArgs >= 0 && (int)args.size() > cmd.maxArgs)
            {
                Serial.println("Error: Too many arguments");
                return;
            }

            // Выполнение
            try
            {
                cmd.handler(args);
            }
            catch (...)
            {
                Serial.println("Error executing command");
            }
        }
        else
        {
            Serial.println("Unknown command. Type 'help' for list of commands");
        }
    }

private:
    // Парсинг аргументов с поддержкой кавычек
    std::vector<String> parseArguments(const String &input)
    {
        std::vector<String> args;
        String currentArg = "";
        bool inQuotes = false;

        for (int i = 0; i < input.length(); i++)
        {
            char c = input[i];

            if (c == '"')
            {
                inQuotes = !inQuotes;
            }
            else if (c == ' ' && !inQuotes)
            {
                if (currentArg.length() > 0)
                {
                    args.push_back(currentArg);
                    currentArg = "";
                }
            }
            else
            {
                currentArg += c;
            }
        }

        if (currentArg.length() > 0)
        {
            args.push_back(currentArg);
        }

        return args;
    }

    // Регистрация стандартных команд
    void registerDefaultCommands()
    {
        // HELP
        registerCommand({"help",
                         "Show available commands",
                         CommandType::SYSTEM,
                         [this](const std::vector<String> &args)
                         {
                             Serial.println("\n=== AVAILABLE COMMANDS ===");
                             Serial.println("");

                             // Группировка по типам
                             std::map<CommandType, std::vector<ConsoleCommand>> grouped;
                             for (auto &pair : commands)
                             {
                                 // Пропускаем алиасы
                                 if (pair.second.name != pair.first)
                                     continue;
                                 grouped[pair.second.type].push_back(pair.second);
                             }

                             for (auto &group : grouped)
                             {
                                 String typeName;
                                 switch (group.first)
                                 {
                                 case CommandType::SYSTEM:
                                     typeName = "SYSTEM";
                                     break;
                                 case CommandType::FORM:
                                     typeName = "FORM";
                                     break;
                                 case CommandType::TASK:
                                     typeName = "TASK";
                                     break;
                                 case CommandType::DEBUG:
                                     typeName = "DEBUG";
                                     break;
                                 case CommandType::UI:
                                     typeName = "UI";
                                     break;
                                 case CommandType::DATA:
                                     typeName = "DATA";
                                     break;
                                 case CommandType::WIFI:
                                     typeName = "WIFI";
                                     break;
                                 case CommandType::PLOTTER:
                                     typeName = "PLOTTER";
                                     break;
                                 case CommandType::TEST:
                                     typeName = "TEST";
                                     break;
                                 }
                                 Serial.println("\n[" + typeName + "]");
                                 for (auto &cmd : group.second)
                                 {
                                     String aliases = "";
                                     for (const auto &alias : cmd.aliases)
                                     {
                                         aliases += " [" + alias + "]";
                                     }
                                     Serial.println("  " + cmd.name + aliases + " - " + cmd.description);
                                 }
                             }
                             Serial.println("");
                         },
                         {"?", "h"}});

        // SYSTEM - Информация о системе
        registerCommand({"sysinfo",
                         "Display system information",
                         CommandType::SYSTEM,
                         [](const std::vector<String> &args)
                         {
                             Serial.println("\n=== SYSTEM INFORMATION ===");
                             Serial.println("Platform: Sozvezdiye OS");
                             Serial.println("Version: " + String(_VERSION_CORE));
                             Serial.println("Description: " + String(_DESCRIPTION));
                             Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
                             Serial.println("Total Heap: " + String(ESP.getHeapSize()) + " bytes");
                             Serial.println("CPU Freq: " + String(ESP.getCpuFreqMHz()) + " MHz");
                             Serial.println("Uptime: " + String(millis() / 1000) + " sec");
                             Serial.println("Forms in stack: " + String(formsStack.size()));
                             Serial.println("Minimized forms: " + String(minimizedForms.size()));
                             Serial.println("");
                         },
                         {"info", "status"}});

        // SYSTEM - Перезагрузка
        registerCommand({"reboot",
                         "Reboot the system",
                         CommandType::SYSTEM,
                         [](const std::vector<String> &args)
                         {
                             Serial.println("Rebooting...");
                             delay(500);
                             ESP.restart();
                         },
                         {"reset", "restart"}});

        // FORM - Список форм
        registerCommand({"forms",
                         "List all forms in stack",
                         CommandType::FORM,
                         [](const std::vector<String> &args)
                         {
                             Serial.println("\n=== FORMS IN STACK ===");
                             if (formsStack.empty())
                             {
                                 Serial.println("No forms in stack");
                             }
                             else
                             {
                                 // Создаем копию стека для отображения
                                 std::stack<exForm *> tempStack = formsStack;
                                 std::vector<exForm *> formList;
                                 while (!tempStack.empty())
                                 {
                                     formList.push_back(tempStack.top());
                                     tempStack.pop();
                                 }

                                 // Отображаем сверху вниз
                                 for (int i = formList.size() - 1; i >= 0; i--)
                                 {
                                     exForm *form = formList[i];
                                     bool minimized = isFormMinimized(form);
                                     String status = minimized ? "[MINIMIZED]" : "[ACTIVE]";
                                     Serial.println("  " + String(i) + ". " + form->title + " " + status);
                                 }
                             }
                             Serial.println("");
                         },
                         {"flist", "formlist"}});

        // FORM - Свернуть форму
        registerCommand({"minimize",
                         "Minimize form by index or title\n  usage: minimize [index|title]",
                         CommandType::FORM,
                         [](const std::vector<String> &args)
                         {
                             if (args.empty())
                             {
                                 Serial.println("Error: Specify form index or title");
                                 return;
                             }

                             // Создаем список форм
                             std::stack<exForm *> tempStack = formsStack;
                             std::vector<exForm *> formList;
                             while (!tempStack.empty())
                             {
                                 formList.push_back(tempStack.top());
                                 tempStack.pop();
                             }

                             for (int i = formList.size() - 1; i >= 0; i--)
                             {
                                 exForm *form = formList[i];
                                 String identifier = args[0];
                                 bool match = false;

                                 if (isDigit(identifier[0]))
                                 {
                                     int idx = identifier.toInt();
                                     if (idx == i)
                                         match = true;
                                 }
                                 else
                                 {
                                     if (form->title.indexOf(identifier) >= 0)
                                         match = true;
                                 }

                                 if (match && !isFormMinimized(form))
                                 {
                                     minimizeForm(form);
                                     Serial.println("Minimized: " + form->title);
                                     return;
                                 }
                             }
                             Serial.println("Form not found or already minimized");
                         },
                         {"min", "collapse"}});

        // FORM - Восстановить форму
        registerCommand({"restore",
                         "Restore minimized form by index or title\n  usage: restore [index|title]",
                         CommandType::FORM,
                         [](const std::vector<String> &args)
                         {
                             if (args.empty())
                             {
                                 Serial.println("Error: Specify form index or title");
                                 return;
                             }

                             String identifier = args[0];
                             std::vector<exForm *> minimizedCopy = minimizedForms;

                             for (int i = 0; i < (int)minimizedCopy.size(); i++)
                             {
                                 exForm *form = minimizedCopy[i];
                                 bool match = false;

                                 if (isDigit(identifier[0]))
                                 {
                                     int idx = identifier.toInt();
                                     if (idx == i)
                                         match = true;
                                 }
                                 else
                                 {
                                     if (form->title.indexOf(identifier) >= 0)
                                         match = true;
                                 }

                                 if (match)
                                 {
                                     restoreForm(form);
                                     Serial.println("Restored: " + form->title);
                                     return;
                                 }
                             }
                             Serial.println("Minimized form not found");
                         },
                         {"res", "show"}});

        // FORM - Закрыть форму
        registerCommand({"close",
                         "Close form by index or title\n  usage: close [index|title]",
                         CommandType::FORM,
                         [](const std::vector<String> &args)
                         {
                             if (args.empty())
                             {
                                 Serial.println("Error: Specify form index or title");
                                 return;
                             }

                             std::stack<exForm *> tempStack = formsStack;
                             std::vector<exForm *> formList;
                             while (!tempStack.empty())
                             {
                                 formList.push_back(tempStack.top());
                                 tempStack.pop();
                             }

                             String identifier = args[0];
                             int idx = -1;

                             if (isDigit(identifier[0]))
                             {
                                 idx = identifier.toInt();
                             }
                             else
                             {
                                 for (int i = formList.size() - 1; i >= 0; i--)
                                 {
                                     if (formList[i]->title.indexOf(identifier) >= 0)
                                     {
                                         idx = i;
                                         break;
                                     }
                                 }
                             }

                             if (idx >= 0 && idx < (int)formList.size())
                             {
                                 // Удаляем форму из стека
                                 std::stack<exForm *> newStack;
                                 while (!formsStack.empty())
                                 {
                                     exForm *top = formsStack.top();
                                     formsStack.pop();
                                     if (top != formList[idx])
                                     {
                                         newStack.push(top);
                                     }
                                 }
                                 while (!newStack.empty())
                                 {
                                     formsStack.push(newStack.top());
                                     newStack.pop();
                                 }

                                 // Удаляем из свернутых
                                 auto it = std::find(minimizedForms.begin(), minimizedForms.end(), formList[idx]);
                                 if (it != minimizedForms.end())
                                 {
                                     minimizedForms.erase(it);
                                 }

                                 delete formList[idx];
                                 Serial.println("Form closed");
                             }
                             else
                             {
                                 Serial.println("Form not found");
                             }
                         },
                         {"kill", "delete"}});

        // TASK - Список задач
        registerCommand({"tasks",
                         "List all system tasks",
                         CommandType::TASK,
                         [](const std::vector<String> &args)
                         {
                             Serial.println("\n=== SYSTEM TASKS ===");
                             bool showAll = (args.size() > 0 && args[0] == "-a");

                             int activeCount = 0;
                             for (const auto &task : tasks)
                             {
                                 if (task.activ || showAll)
                                 {
                                     String status = task.activ ? "[ACTIVE]" : "[INACTIVE]";
                                     String typeStr;
                                     switch (task.type)
                                     {
                                     case SYSTEM:
                                         typeStr = "SYS";
                                         break;
                                     case DESKTOP:
                                         typeStr = "DESK";
                                         break;
                                     case USER:
                                         typeStr = "USER";
                                         break;
                                     }
                                     String priorityStr;
                                     switch (task.priority)
                                     {
                                     case PRIORITY_LOW:
                                         priorityStr = "LOW";
                                         break;
                                     case PRIORITY_NORMAL:
                                         priorityStr = "NORM";
                                         break;
                                     case PRIORITY_HIGH:
                                         priorityStr = "HIGH";
                                         break;
                                     case PRIORITY_CRITICAL:
                                         priorityStr = "CRIT";
                                         break;
                                     }

                                     Serial.println("  " + task.name +
                                                    " [" + typeStr + "][" + priorityStr + "] " + status +
                                                    " interval: " + String(task.interval));
                                     if (task.activ)
                                         activeCount++;
                                 }
                             }
                             Serial.println("Active tasks: " + String(activeCount) + "/" + String(tasks.size()));
                             Serial.println("");
                         },
                         {"tsk", "tasklist"}});

        // TASK - Запустить задачу
        registerCommand({"start",
                         "Start a task by name\n  usage: start [task_name]",
                         CommandType::TASK,
                         [](const std::vector<String> &args)
                         {
                             if (args.empty())
                             {
                                 Serial.println("Error: Specify task name");
                                 return;
                             }

                             String taskName = args[0];
                             if (_TD.runTask(taskName))
                             {
                                 Serial.println("Task started: " + taskName);
                             }
                             else
                             {
                                 Serial.println("Task not found or already active");
                             }
                         },
                         {"run", "exec"}});

        // TASK - Остановить задачу
        registerCommand({"stop",
                         "Stop a task by name\n  usage: stop [task_name]",
                         CommandType::TASK,
                         [](const std::vector<String> &args)
                         {
                             if (args.empty())
                             {
                                 Serial.println("Error: Specify task name");
                                 return;
                             }

                             String taskName = args[0];
                             if (_TD.removeTask(taskName))
                             {
                                 Serial.println("Task stopped: " + taskName);
                             }
                             else
                             {
                                 Serial.println("Task not found or already stopped");
                             }
                         },
                         {"killtask", "end"}});

        // TASK - CPU нагрузка
        registerCommand({"cpu",
                         "Display CPU load information",
                         CommandType::TASK,
                         [](const std::vector<String> &args)
                         {
                             int load = _TD.getCPULoad();
                             Serial.println("\n=== CPU LOAD ===");
                             Serial.println("Current load: " + String(load) + "%");

                             // Графическая полоса
                             String bar = "[";
                             int filled = load / 5;
                             for (int i = 0; i < 20; i++)
                             {
                                 bar += (i < filled) ? "#" : " ";
                             }
                             bar += "]";
                             Serial.println(bar);
                             Serial.println("");
                         },
                         {"load", "performance"}});

        // DATA - Вывести данные формы
        registerCommand({"dump",
                         "Dump form elements data\n  usage: dump [form_index|title]",
                         CommandType::DATA,
                         [](const std::vector<String> &args)
                         {
                             if (args.empty())
                             {
                                 Serial.println("Error: Specify form");
                                 return;
                             }

                             // Находим форму
                             std::stack<exForm *> tempStack = formsStack;
                             std::vector<exForm *> formList;
                             while (!tempStack.empty())
                             {
                                 formList.push_back(tempStack.top());
                                 tempStack.pop();
                             }

                             String identifier = args[0];
                             exForm *targetForm = nullptr;

                             if (isDigit(identifier[0]))
                             {
                                 int idx = identifier.toInt();
                                 if (idx < (int)formList.size())
                                 {
                                     targetForm = formList[idx];
                                 }
                             }
                             else
                             {
                                 for (auto form : formList)
                                 {
                                     if (form->title.indexOf(identifier) >= 0)
                                     {
                                         targetForm = form;
                                         break;
                                     }
                                 }
                             }

                             if (!targetForm)
                             {
                                 Serial.println("Form not found");
                                 return;
                             }

                             Serial.println("\n=== FORM DATA: " + targetForm->title + " ===");
                             Serial.println("");

                             // Здесь нужно получить доступ к элементам формы

                             Serial.println("(Feature: Dump form elements)");
                             Serial.println("");
                         },
                         {"dumpform", "inspect"}});

        // DATA - JSON вывод
        registerCommand({"json",
                         "Output data in JSON format\n  usage: json [form_index|title]",
                         CommandType::DATA,
                         [](const std::vector<String> &args)
                         {
                             // Реализация JSON вывода
                             Serial.println("{");
                             Serial.println("  \"system\": \"Sozvezdiye\",");
                             Serial.println("  \"version\": \"" + String(_VERSION_CORE) + "\",");
                             Serial.println("  \"timestamp\": " + String(millis()) + ",");
                             Serial.println("  \"forms_count\": " + String(formsStack.size()) + ",");
                             Serial.println("  \"tasks_count\": " + String(tasks.size()) + ",");
                             Serial.println("  \"cpu_load\": " + String(_TD.getCPULoad()) + ",");
                             Serial.println("  \"free_heap\": " + String(ESP.getFreeHeap()));
                             Serial.println("}");
                         },
                         {"export", "out"}});

        // WIFI - Статус WiFi
        registerCommand({"wifi",
                         "Show WiFi status and networks\n  usage: wifi [status|scan|connect]",
                         CommandType::WIFI,
                         [](const std::vector<String> &args)
                         {
                             if (args.empty() || args[0] == "status")
                             {
                                 Serial.println("\n=== WiFi STATUS ===");
                                 Serial.println("Status: " + String(wifiManager.getStatusString()));
                                 Serial.println("SSID: " + String(WiFi.SSID()));
                                 Serial.println("IP: " + WiFi.localIP().toString());
                                 Serial.println("RSSI: " + String(WiFi.RSSI()) + " dBm");
                                 Serial.println("");
                             }
                             else if (args[0] == "scan")
                             {
                                 Serial.println("Scanning networks...");
                                 int n = WiFi.scanNetworks();
                                 if (n == 0)
                                 {
                                     Serial.println("No networks found");
                                 }
                                 else
                                 {
                                     Serial.println("Found " + String(n) + " networks:");
                                     for (int i = 0; i < n; i++)
                                     {
                                         Serial.print("  " + String(i) + ". " + WiFi.SSID(i));
                                         Serial.println(" [" + String(WiFi.RSSI(i)) + "dBm] " +
                                                        (WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "secured"));
                                     }
                                 }
                                 WiFi.scanDelete();
                             }
                             else if (args[0] == "connect" && args.size() >= 2)
                             {
                                 String ssid = args[1];
                                 String pass = (args.size() >= 3) ? args[2] : "";
                                 Serial.println("Connecting to: " + ssid);
                                 if (wifiManager.connect(ssid, pass))
                                 {
                                     Serial.println("Connected!");
                                     Serial.println("IP: " + WiFi.localIP().toString());
                                 }
                                 else
                                 {
                                     Serial.println("Connection failed!");
                                 }
                             }
                             else
                             {
                                 Serial.println("Usage: wifi [status|scan|connect SSID [PASS]]");
                             }
                         },
                         {"wlan", "network"}});

        // PLOTTER - Управление плоттером
        registerCommand({"plot",
                         "Plotter commands\n  usage: plot [clear|data|range]",
                         CommandType::PLOTTER,
                         [](const std::vector<String> &args)
                         {
                             if (args.empty())
                             {
                                 Serial.println("Usage: plot [clear|data|range min max]");
                                 return;
                             }

                             // Поиск плоттера в активных формах
                             // Упрощенная версия:

                             if (args[0] == "clear")
                             {
                                 Serial.println("Clearing plotter data...");
                                 // Нужно найти активный плоттер и очистить
                             }
                             else if (args[0] == "data")
                             {
                                 // Генерация тестовых данных
                                 Serial.println("Generating test data...");
                                 // Можно добавить тестовые данные в плоттер
                             }
                             else if (args[0] == "range" && args.size() >= 3)
                             {
                                 float min = args[1].toFloat();
                                 float max = args[2].toFloat();
                                 Serial.println("Setting Y range: " + String(min) + " - " + String(max));
                             }
                             else
                             {
                                 Serial.println("Unknown plot command");
                             }
                         },
                         {"plotter", "graph"}});

        // UI - Управление UI
        registerCommand({"ui",
                         "UI controls\n  usage: ui [refresh|clear]",
                         CommandType::UI,
                         [](const std::vector<String> &args)
                         {
                             if (args.empty() || args[0] == "refresh")
                             {
                                 Serial.println("Refreshing UI...");
                                 // Обновить отображение
                                 if (!formsStack.empty())
                                 {
                                     formsStack.top()->showForm();
                                 }
                             }
                             else if (args[0] == "clear")
                             {
                                 Serial.println("Clearing screen...");
                                 _GGL.gray.drawFillFrame(0, 0, 256, 160, _GGL.gray.BLACK, _GGL.gray.WHITE);
                             }
                             else if (args[0] == "theme")
                             {
                                 // TODO: Смена темы
                             }
                             else
                             {
                                 Serial.println("Usage: ui [refresh|clear]");
                             }
                         },
                         {"screen", "display"}});

        // DEBUG - Тестовые команды
        registerCommand({"test",
                         "Run diagnostic tests\n  usage: test [all|memory|forms]",
                         CommandType::TEST,
                         [](const std::vector<String> &args)
                         {
                             String testType = args.empty() ? "all" : args[0];

                             if (testType == "all" || testType == "memory")
                             {
                                 Serial.println("\n=== MEMORY TEST ===");
                                 Serial.println("Free heap: " + String(ESP.getFreeHeap()) + " bytes");
                                 Serial.println("Heap fragmentation: " + String(ESP.getHeapFragmentation()) + "%");
                                 Serial.println("Max free block: " + String(ESP.getMaxAllocHeap()) + " bytes");
                             }

                             if (testType == "all" || testType == "forms")
                             {
                                 Serial.println("\n=== FORMS TEST ===");
                                 Serial.println("Forms in stack: " + String(formsStack.size()));
                                 Serial.println("Minimized forms: " + String(minimizedForms.size()));

                                 // Проверка ссылок
                                 if (!formsStack.empty())
                                 {
                                     std::stack<exForm *> tempStack = formsStack;
                                     int count = 0;
                                     while (!tempStack.empty())
                                     {
                                         exForm *form = tempStack.top();
                                         tempStack.pop();
                                         if (form != nullptr)
                                         {
                                             count++;
                                         }
                                     }
                                     Serial.println("Valid forms: " + String(count));
                                 }
                             }

                             if (testType == "all" || testType == "tasks")
                             {
                                 Serial.println("\n=== TASKS TEST ===");
                                 int active = 0;
                                 for (const auto &task : tasks)
                                 {
                                     if (task.activ)
                                         active++;
                                 }
                                 Serial.println("Active tasks: " + String(active) + "/" + String(tasks.size()));
                                 Serial.println("CPU Load: " + String(_TD.getCPULoad()) + "%");
                             }

                             Serial.println("");
                         },
                         {"diagnostic", "check"}});

        // DEBUG - Трассировка
        registerCommand({"trace",
                         "Enable/disable debug tracing\n  usage: trace [on|off]",
                         CommandType::DEBUG,
                         [](const std::vector<String> &args)
                         {
                             if (args.empty())
                             {
                                 Serial.println("Trace is " + String(DEBUG_TASK_DISPATCHER ? "ON" : "OFF"));
                                 return;
                             }

                             if (args[0] == "on")
                             {
                                 // Включение отладки
                                 Serial.println("Debug tracing enabled");
                             }
                             else if (args[0] == "off")
                             {
                                 Serial.println("Debug tracing disabled");
                             }
                             else
                             {
                                 Serial.println("Usage: trace [on|off]");
                             }
                         },
                         {"debug", "log"}});

        // CLEAR - Очистка терминала
        registerCommand({"clear",
                         "Clear terminal screen",
                         CommandType::SYSTEM,
                         [](const std::vector<String> &args)
                         {
                             Serial.print("\033[2J\033[H"); // ANSI clear
                             Serial.println("");
                         },
                         {"cls", "clean"}});
    }

    bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }
};

// Глобальный экземпляр
ConsoleManager *ConsoleManager::instance = nullptr;