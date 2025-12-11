#pragma once
#include "types.hpp"
#include <map>
#include <string>
#include <vector>

const int TARGET_SR = 44100;
const double PIv = 3.14159265358979323846;

// загрузка таблицы нот из файла
std:: map<std::string, double> loadPitchTable(const std::string& path);

// получить частоту ноты
double noteFreq(const std:: string& p, const std::map<std::string, double>& table);

// разбить строку на токены
std::vector<std::string> tokenize(const std::string& s);

// убрать комментарий
std::string cutComment(const std:: string& s);

// огибающая громкости
double envGain(int i, int total, int atk, int rel);

// работа с wav
bool loadWav(const std::string& path, WavData& w);
bool writeWav(const std:: string& path, const std::vector<float>& mono);