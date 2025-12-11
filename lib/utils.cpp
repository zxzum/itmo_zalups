#include "utils.hpp"
#include <fstream>
#include <cmath>
#include <cctype>
#include <sstream>

using namespace std;

map<string, double> loadPitchTable(const string& path) {
    map<string, double> table;
    ifstream f(path);
    if (!f) return table;

    string note;
    double freq;
    while (f >> note >> freq) {
        table[note] = freq;
    }
    return table;
}

double noteFreq(const string& p, const map<string, double>& table) {
    auto it = table.find(p);
    if (it == table.end()) return 440.0;
    return it->second;
}

vector<string> tokenize(const string& s) {
    vector<string> out;
    string cur;
    for (char c : s) {
        if (isspace((unsigned char)c)) {
            if (! cur.empty()) {
                out.push_back(cur);
                cur.clear();
            }
        } else {
            cur. push_back(c);
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

string cutComment(const string& s) {
    size_t p = s.find('#');
    if (p != string::npos) return s.substr(0, p);
    return s;
}

double envGain(int i, int total, int atk, int rel) {
    if (total <= 0) return 0;
    if (atk > 0 && i < atk) return (double)i / atk;
    if (rel > 0 && i > total - rel) return (double)(total - i) / rel;
    return 1.0;
}

// вспомогательные функции для чтения wav
static uint32_t rd32(const unsigned char* p) {
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

static uint16_t rd16(const unsigned char* p) {
    return p[0] | (p[1] << 8);
}

bool loadWav(const string& path, WavData& w) {
    ifstream f(path, ios::binary);
    if (!f) return false;

    vector<unsigned char> data((istreambuf_iterator<char>(f)), {});
    if (data.size() < 44) return false;
    if (string((char*)&data[0], 4) != "RIFF") return false;
    if (string((char*)&data[8], 4) != "WAVE") return false;

    size_t pos = 12;
    int channels = 1, bps = 16, sr = TARGET_SR;
    size_t dataPos = 0, dataSize = 0;

    while (pos + 8 <= data.size()) {
        string id((char*)&data[pos], 4);
        uint32_t sz = rd32(&data[pos + 4]);
        pos += 8;

        if (id == "fmt ") {
            if (sz < 16) return false;
            channels = rd16(&data[pos + 2]);
            sr = rd32(&data[pos + 4]);
            bps = rd16(&data[pos + 14]);
        } else if (id == "data") {
            dataPos = pos;
            dataSize = sz;
        }
        pos += sz;
    }

    if (dataPos == 0 || bps != 16) return false;

    const int16_t* ptr = (const int16_t*)(&data[dataPos]);
    size_t samples = dataSize / 2;
    w.sr = sr;
    w.mono.clear();
    w.mono.reserve(samples);

    for (size_t i = 0; i < samples; i++) {
        int16_t v = ptr[i * channels];
        w.mono.push_back(v / 32768.0f);
    }
    return true;
}

bool writeWav(const string& path, const vector<float>& mono) {
    ofstream f(path, ios::binary);
    if (!f) return false;

    uint32_t dataSize = mono.size() * 2;
    uint32_t riffSize = 36 + dataSize;

    f.write("RIFF", 4);
    f.write((char*)&riffSize, 4);
    f.write("WAVE", 4);
    f.write("fmt ", 4);

    uint32_t fmtSize = 16;
    f.write((char*)&fmtSize, 4);
    uint16_t audioFmt = 1;
    f.write((char*)&audioFmt, 2);
    uint16_t channels = 1;
    f.write((char*)&channels, 2);
    uint32_t sr = TARGET_SR;
    f.write((char*)&sr, 4);
    uint32_t byteRate = TARGET_SR * 2;
    f. write((char*)&byteRate, 4);
    uint16_t blockAlign = 2;
    f.write((char*)&blockAlign, 2);
    uint16_t bits = 16;
    f.write((char*)&bits, 2);
    f.write("data", 4);
    f.write((char*)&dataSize, 4);

    for (float v : mono) {
        float c = max(-1.0f, min(1.0f, v));
        int16_t s = (int16_t)round(c * 32767.0f);
        f.write((char*)&s, 2);
    }
    return true;
}