#pragma once

#include "common.h"
#include "ISerialisable.h"

enum class SerialiseMode {
    Saving,
    Loading
};

class SaveManager {
public:
    SaveManager();
    ~SaveManager();

    void newGame();
    void saveGame(std::string saveFilePath);
    void loadGame(std::string saveFilePath);

    // For custom serialisers
    json* getCurrentSerialiseNode();
    void setCurrentSerialiseNode(json* node);

    template<typename T>
    void SerialiseValue(const std::string& label, T& value);
    template<typename T>
    void SerialiseValue(const std::string& label, T& get, std::function<void(T)> set);
    template<typename T>
    void SerialiseValue(const std::string& label, T&& get, std::function<void(T)> set);
    template<typename T>
    void SerialiseValue(const std::string& label, std::function<T()> get, std::function<void(T)> set);
    template<typename T>
    void SerialiseDeep(const std::string& label, T value);
//    template<typename T>
//    void SerialiseCollectionDeep(const std::string& label, T& collection);

    SerialiseMode mode;
private:
    json save;
    json* curr;
};

template<typename T>
void SaveManager::SerialiseValue(const std::string& label, T& value) {
    if (mode == SerialiseMode::Saving)
        (*curr)[label] = value;

    if (mode == SerialiseMode::Loading)
        value = curr->at(label).get_to(value);
}

template<typename T>
void SaveManager::SerialiseValue(const std::string& label, T&& get, std::function<void(T)> set) {
    if (mode == SerialiseMode::Saving)
        (*curr)[label] = get;

    if (mode == SerialiseMode::Loading)
        set(curr->at(label).get<T>());
}

template<typename T>
void SaveManager::SerialiseValue(const std::string& label, T& get, std::function<void(T)> set) {
    if (mode == SerialiseMode::Saving)
        (*curr)[label] = get;

    if (mode == SerialiseMode::Loading)
        set(curr->at(label).get<T>());
}

template<typename T>
void SaveManager::SerialiseValue(const std::string& label, std::function<T()> get, std::function<void(T)> set) {
    if (mode == SerialiseMode::Saving)
        (*curr)[label] = get();

    if (mode == SerialiseMode::Loading)
        set(curr->at(label).get<T>());
}

template<typename T>
void SaveManager::SerialiseDeep(const std::string& label, T value) {
    auto parent = curr;
    if (mode == SerialiseMode::Saving) {
        json j{};
        curr = &j;
        value->serialise();
        (*parent)[label] = j;
    }
    if (mode == SerialiseMode::Loading) {
        curr = &parent->at(label);
        value->serialise();
    }
    curr = parent;
}

//template<typename T>
//void SaveManager::SerialiseCollectionDeep(const std::string& label, T& collection) {
//    json saveData{};
//
//    auto parent = curr;
//    for (auto& val : collection) {
//        json j{};
//        curr = &j;
//        val->serialise();
//        saveData.push_back(j);
//    }
//    curr = parent;
//
//    (*curr)[label] = saveData;
//}
