#include "ModuleManager.h"
#include <stdexcept>
#include <algorithm>

namespace sgl {
    void ModuleManager::registerModule(std::unique_ptr<IModule> module) {
        modules.push_back(std::move(module));

        // Если это первый модуль, автоматически делаем его активным
        if (modules.size() == 1 && currentModule == nullptr) {
            currentModule = modules.back().get();
            currentModule->init();
        }
    }

    void ModuleManager::setActiveModule(const std::string&name) {
        auto it = std::find_if(modules.begin(), modules.end(),
                               [&name](const auto&module) { return module->getName() == name; });

        if (it != modules.end()) {
            currentModule = it->get();
            currentModule->init();
        }
        else {
            throw std::runtime_error("Модуль не найден: " + name);
        }
    }

    IModule* ModuleManager::getActiveModule() const {
        return currentModule;
    }

    const std::vector<std::unique_ptr<IModule>>& ModuleManager::getModules() const {
        return modules;
    }
}
