#include "lib.hpp"
#include "types.hpp"
#include "utils.hpp"
#include "effects.hpp"
#include "instruments.hpp"
#include "parser.hpp"
#include <iostream>
#include <functional>

using namespace std;

int run_itmoloops(const string& inFile, const string& outFile) {
    // загружаем таблицу нот
    auto pitchTable = loadPitchTable("notes.txt");
    if (pitchTable.empty()) {
        cerr << "warning: cant load notes.txt, using default A4=440\n";
    }

    // парсим score
    ScoreData score;
    if (!parseScore(inFile, score)) {
        cerr << "cant open input\n";
        return 1;
    }

    if (score.patterns.find("main") == score.patterns.end()) {
        cerr << "no main pattern\n";
        return 1;
    }

    double totalLen = computePatternLen("main", score.patterns);
    double beatSec = 60.0 / score.bpm;

    // собираем ноты по инструментам
    map<string, vector<NoteEvent>> perInst;

    function<void(const string&, double)> expand = [&](const string& pname, double off) {
        auto& p = score. patterns[pname];
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
        if (score.instruments.find(kv.first) == score.instruments. end()) {
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