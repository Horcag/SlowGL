#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include "IModule.h"
#include <memory>
#include <vector>
#include <string>

namespace sgl {
    /**
     * @class ModuleManager
     * @brief Управляет модулями приложения
     *
     * Класс ModuleManager отвечает за хранение, регистрацию и активацию
     * модулей приложения. Он позволяет переключаться между различными
     * модулями и управляет их жизненным циклом.
     */
    class ModuleManager {
    public:
        /**
         * @brief Регистрирует новый модуль в менеджере
         * @param module Уникальный указатель на модуль для регистрации
         */
        void registerModule(std::unique_ptr<IModule> module);

        /**
         * @brief Устанавливает активный модуль по его имени
         * @param name Имя модуля для активации
         * @throws std::runtime_error если модуль с указанным именем не найден
         */
        void setActiveModule(const std::string&name);

        /**
         * @brief Возвращает указатель на активный модуль
         * @return Указатель на активный модуль или nullptr, если активного модуля нет
         */
        IModule* getActiveModule();

        /**
         * @brief Возвращает список всех зарегистрированных модулей
         * @return Константная ссылка на вектор с указателями на модули
         */
        [[nodiscard]] const std::vector<std::unique_ptr<IModule>>& getModules() const;

    private:
        std::vector<std::unique_ptr<IModule>> modules; ///< Список зарегистрированных модулей
        IModule* currentModule = nullptr; ///< Указатель на активный модуль
    };
}

#endif // MODULE_MANAGER_H
