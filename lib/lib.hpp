#pragma once
#include <string>

// Базовая функция генерации из файла
int run_itmoloops(const std::string& inFile, const std::string& outFile);

// Генерация с указанием базовой директории для поиска notes.txt и samples
int run_itmoloops(const std::string& inFile, const std::string& outFile, const std::string& baseDir);

// Генерация из строки (содержимого score) с указанием базовой директории
int run_itmoloops_from_string(const std::string& scoreContent, 
                               const std::string& outFile,
                               const std::string& baseDir);