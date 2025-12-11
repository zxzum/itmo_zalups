#include "parser.hpp"
#include "utils.hpp"
#include <fstream>
#include <set>

using namespace std;

// вычисление длины паттерна
static double computeLen(const string& pname, map<string, Pattern>& pats, set<string>& vis) {
    if (vis.count(pname)) return 0;
    vis.insert(pname);

    auto& p = pats[pname];
    double len = 0;

    for (auto& a :  p.acts) {
        double off = a.note.startSec;
        double end;
        if (a.isCall) {
            end = off + computeLen(a. call, pats, vis);
        } else {
            end = off + a.note.durSec;
        }
        if (end > len) len = end;
    }

    p.lenSec = len;
    vis.erase(pname);
    return len;
}

double computePatternLen(const string& pname, map<string, Pattern>& pats) {
    set<string> vis;
    return computeLen(pname, pats, vis);
}

bool parseScore(const string& path, ScoreData& score) {
    ifstream f(path);
    if (!f) return false;

    string line;
    InstrumentCfg curInst;
    Pattern curPat;
    enum { NONE, INST, PAT } mode = NONE;

    while (getline(f, line)) {
        line = cutComment(line);
        if (line.find_first_not_of(" \t\r\n") == string::npos) continue;

        auto t = tokenize(line);
        if (t.empty()) continue;

        if (mode == NONE) {
            if (t[0] == "bpm" && t. size() >= 2) {
                score.bpm = stod(t[1]);
            } else if (t[0] == "instrument" && t. size() >= 3) {
                mode = INST;
                curInst = InstrumentCfg();
                curInst.name = t[1];
                curInst.type = t[2];
            } else if (t[0] == "pattern" && t. size() >= 3) {
                mode = PAT;
                curPat = Pattern();
                curPat.name = t[1];
                if (t. size() >= 4 && t[2] == "resolution") {
                    curPat.res = stoi(t[3]);
                } else {
                    curPat.res = stoi(t[2]);
                }
            }
        } else if (mode == INST) {
            if (t[0] == "end") {
                score.instruments[curInst.name] = curInst;
                mode = NONE;
            } else if (t[0] == "effect" && t.size() >= 2) {
                EffectCfg e;
                e. type = t[1];
                for (size_t i = 2; i < t.size(); i++) {
                    auto p = t[i]. find('=');
                    if (p != string::npos) {
                        string k = t[i]. substr(0, p);
                        string v = t[i].substr(p + 1);
                        if (k == "gain" || k == "delay" || k == "freq") e.a = stod(v);
                        if (k == "decay" || k == "depth") e.b = stod(v);
                    }
                }
                curInst.effects. push_back(e);
            } else {
                for (auto& kv : t) {
                    auto p = kv. find('=');
                    if (p == string::npos) continue;
                    string k = kv.substr(0, p);
                    string v = kv.substr(p + 1);

                    if (k == "sample") curInst.sample = v;
                    else if (k == "root") curInst.rootNote = v;
                    else if (k == "loop") {
                        auto c = v.find(',');
                        if (c != string:: npos) {
                            curInst.loopA = stoi(v.substr(0, c));
                            curInst.loopB = stoi(v. substr(c + 1));
                        }
                    }
                    else if (k == "attack") curInst.attack = stod(v);
                    else if (k == "release") curInst.release = stod(v);
                    else if (k == "duty") curInst.duty = stod(v);
                }
            }
        } else if (mode == PAT) {
            if (t[0] == "end") {
                score.patterns[curPat.name] = curPat;
                mode = NONE;
            } else if (t. size() >= 2 && ! t[1].empty() && t[1][0] == '@') {
                // вызов паттерна
                double unit = (60.0 / score.bpm) / curPat.res;
                PatternAction a;
                a. isCall = true;
                a.call = t[1]. substr(1);
                a.note. startSec = stoi(t[0]) * unit;
                curPat.acts. push_back(a);
            } else if (t.size() >= 5) {
                // нота
                double unit = (60.0 / score.bpm) / curPat.res;
                PatternAction a;
                a.isCall = false;
                a. note.startSec = stoi(t[0]) * unit;
                a. note.inst = t[1];
                a.note.pitch = t[2];
                a.note.durSec = stoi(t[3]) * unit;
                a.note.vel = stoi(t[4]);
                curPat.acts.push_back(a);
            }
        }
    }
    return true;
}