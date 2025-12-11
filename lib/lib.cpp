#include "lib.hpp"
#include "types.hpp"
#include "utils.hpp"
#include "effects.hpp"
#include "instruments.hpp"
#include "parser.hpp"
#include <iostream>
#include <functional>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>

using namespace std;
namespace fs = std::filesystem;

// Внутренняя функция для генерации
static int run_itmoloops_internal(ScoreData& score, const string& outFile, 
                                   const map<string, double>& pitchTable, 
                                   const string& baseDir) {

    if (score.patterns.find("main") == score.patterns.end()) {
        cerr << "no main pattern\n";
        return 1;
    }

    // Исправляем пути к sample файлам относительно baseDir
    if (!baseDir.empty()) {
        for (auto& kv : score.instruments) {
            auto& inst = kv.second;
            if (!inst.sample.empty() && inst.sample[0] != '/') {
                // Относительный путь - преобразуем в абсолютный
                fs::path samplePath = fs::path(baseDir) / inst.sample;
                inst.sample = samplePath.string();
            }
        }
    }

    double totalLen = computePatternLen("main", score.patterns);
    double beatSec = 60.0 / score.bpm;

    // собираем ноты по инструментам
    map<string, vector<NoteEvent>> perInst;

    function<void(const string&, double)> expand = [&](const string& pname, double off) {
        auto& p = score.patterns[pname];
        for (auto& a : p.acts) {
            if (a.isCall) {
                expand(a.call, off + a.note.startSec);
            } else {
                NoteEvent n = a.note;
                n.startSec += off;
                perInst[n.inst].push_back(n);
            }
        }
    };
    expand("main", 0);

    // рендерим каждый инструмент
    vector<vector<float>> buffers;
    for (auto& kv : perInst) {
        if (score.instruments.find(kv.first) == score.instruments.end()) {
            cerr << "unknown inst " << kv.first << "\n";
            continue;
        }
        auto inst = makeInst(score.instruments[kv.first], pitchTable);
        if (!inst) continue;
        buffers.push_back(inst->render(kv.second, totalLen));
    }

    // микшируем
    int totalSamp = (int)(totalLen * TARGET_SR) + 1;
    vector<float> mix(totalSamp, 0);
    for (auto& b : buffers) {
        for (size_t i = 0; i < b.size() && i < mix.size(); i++) {
            mix[i] += b[i];
        }
    }

    writeWav(outFile, mix);
    return 0;
}

// Базовая версия - использует текущую директорию
int run_itmoloops(const string& inFile, const string& outFile) {
    return run_itmoloops(inFile, outFile, ".");
}

// Версия с базовой директорией
int run_itmoloops(const string& inFile, const string& outFile, const string& baseDir) {
    // Загружаем таблицу нот из baseDir/notes.txt
    string notesPath = (fs::path(baseDir) / "notes.txt").string();
    auto pitchTable = loadPitchTable(notesPath);
    if (pitchTable.empty()) {
        cerr << "warning: cant load " << notesPath << ", using default A4=440\n";
    }

    // Парсим score
    ScoreData score;
    if (!parseScore(inFile, score)) {
        cerr << "cant open input\n";
        return 1;
    }

    return run_itmoloops_internal(score, outFile, pitchTable, baseDir);
}

// Генерация из строки
int run_itmoloops_from_string(const string& scoreContent, 
                               const string& outFile,
                               const string& baseDir) {
    // Загружаем таблицу нот
    string notesPath = (fs::path(baseDir) / "notes.txt").string();
    auto pitchTable = loadPitchTable(notesPath);
    if (pitchTable.empty()) {
        cerr << "warning: cant load " << notesPath << ", using default A4=440\n";
    }

    // Парсим score из строки (сохраняем во временный файл с уникальным именем)
    fs::path tempDir = fs::temp_directory_path();
    string tempPath = (tempDir / ("itmoloops_" + to_string(time(nullptr)) + ".txt")).string();
    {
        ofstream f(tempPath);
        if (!f) {
            cerr << "cant create temp file\n";
            return 1;
        }
        f << scoreContent;
    }

    ScoreData score;
    if (!parseScore(tempPath, score)) {
        cerr << "cant parse score content\n";
        fs::remove(tempPath);
        return 1;
    }

    fs::remove(tempPath);
    return run_itmoloops_internal(score, outFile, pitchTable, baseDir);
}