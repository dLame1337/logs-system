#pragma once
#include "nstd/array.h"
#include "nstd/list.h"
#include "nstd/unordered_map.h"

void nstd_test() {
    std::cout << "==== Тест для nstd::array ====\n";
    nstd::array<int> arr;
    arr.push_back(10);
    arr.push_back(20);
    arr.push_back(30);
    std::cout << "розмір массиву: " << arr.size() << "\nзміст: ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "\n\n";

    // тест копіювання массиву
    nstd::array<int> arr_copy(arr);
    arr_copy[1] = 99;
    std::cout << "після змін скопійованого массиву:\n";
    std::cout << "початковий массив: ";
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\nскопійований массив: ";
    for (const auto& elem : arr_copy) {
        std::cout << elem << " ";
    }
    std::cout << "\n\n";

    std::cout << "==== тест для nstd::list ====\n";
    nstd::list<int, int> my_list(1, 100);
    my_list.insert(2, 200);
    my_list.insert(3, 300);
    my_list.upsert(2, 250);
    std::cout << "розмір списку: " << my_list.size() << std::endl;
    std::cout << "вміст списку:\n";
    for (const auto& it : my_list) {
        std::cout << "ключ: " << it.key_ << ", значення: " << it.value_ << "\n";
    }

    // тест видалення елементу
    my_list.earse(1);
    std::cout << "після видалення ключа 1:\n";
    for (const auto& it : my_list) {
        std::cout << "ключ: " << it.key_ << ", значення: " << it.value_ << "\n";
    }

    // тест пошуку елементу
    int* foundValue = my_list.search(2);
    if (foundValue) {
        std::cout << "знайдений ключ 2, значення: " << *foundValue << "\n";
    }
    else {
        std::cout << "ключ 2 не знайдений.\n";
    }
    std::cout << "\n";

    std::cout << "==== тест для nstd::unordered_map ====\n";

    nstd::unordered_map<int, std::string> my_map;
    my_map.insert({ 1, "one" });
    my_map.insert({ 2, "two" });
    my_map.insert({ 3, "three" });
    my_map[4] = "four";
    my_map[2] = "TWO";

    std::cout << "вміст мапи:\n";
    for (auto it = my_map.begin(); it != my_map.end(); ++it) {
        std::cout << "ключ: " << it->key_ << ", значення: " << it->value_ << "\n";
    }

    std::cout << "чи має ключ 3: " << (my_map.contains(3) ? "true" : "false") << "\n";
    std::cout << "чи має ключ 5: " << (my_map.contains(5) ? "true" : "false") << "\n";

    // тест видалення елементу
    my_map.erase(1);
    std::cout << "після видалення ключа 1:\n";
    for (auto it = my_map.begin(); it != my_map.end(); ++it) {
        std::cout << "ключ: " << it->key_ << ", значення: " << it->value_ << "\n";
    }

    my_map.clear();
    std::cout << "розмір мапи після очистки: " << my_map.size() << "\n";
}
