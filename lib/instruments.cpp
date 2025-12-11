#include "instruments.hpp"
#include "utils.hpp"
#include <cmath>

using namespace std;

double Instrument::getFreq(const string& pitch) {
    return noteFreq(pitch, *pitchTable);
}

void Instrument::applyEffects(vector<float>& buf) {
    for (auto& e : fx) e->apply(buf, TARGET_SR);
}

// Sampler
SamplerInst::SamplerInst(const InstrumentCfg& c) {
    cfg = c;
    loadWav(cfg. sample, wav);
}

vector<float> SamplerInst::render(const vector<NoteEvent>& notes, double totalSec) {
    int totalSamp = (int)(totalSec * TARGET_SR) + 1;
    vector<float> out(totalSamp, 0);
    double rootF = getFreq(cfg. rootNote);

    for (auto& n : notes) {
        double tf = getFreq(n. pitch);
        double speed = tf / rootF;
        double step = speed * ((double)wav.sr / TARGET_SR);

        int atkS = (int)(cfg.attack * TARGET_SR);
        int relS = (int)(cfg.release * TARGET_SR);
        int start = (int)(n.startSec * TARGET_SR);
        int len = (int)(n.durSec * TARGET_SR);

        for (int i = 0; i < len && start + i < (int)out.size(); i++) {
            double pos = i * step;

            // зацикливание
            if (cfg.loopA >= 0 && cfg.loopB > cfg.loopA) {
                if (pos >= cfg.loopB) {
                    double span = cfg.loopB - cfg.loopA;
                    pos = cfg.loopA + fmod(pos - cfg.loopA, span);
                }
            } else if (pos >= (int)wav.mono.size()) {
                break;
            }

            // линейная интерполяция
            int p0 = (int)pos;
            double frac = pos - p0;
            float s0 = (p0 < (int)wav.mono.size()) ? wav.mono[p0] : 0;
            float s1 = (p0 + 1 < (int)wav.mono.size()) ? wav.mono[p0 + 1] : s0;
            float smp = s0 * (1 - frac) + s1 * frac;

            double env = envGain(i, len, atkS, relS);
            out[start + i] += smp * (n.vel / 100.0) * env;
        }
    }
    applyEffects(out);
    return out;
}

// осцилляторы
vector<float> OscBase::render(const vector<NoteEvent>& notes, double totalSec) {
    int totalSamp = (int)(totalSec * TARGET_SR) + 1;
    vector<float> out(totalSamp, 0);
    int atkS = (int)(cfg.attack * TARGET_SR);
    int relS = (int)(cfg.release * TARGET_SR);

    for (auto& n : notes) {
        double f = getFreq(n.pitch);
        int start = (int)(n.startSec * TARGET_SR);
        int len = (int)(n.durSec * TARGET_SR);

        for (int i = 0; i < len && start + i < (int)out.size(); i++) {
            double t = (double)i / TARGET_SR;
            double env = envGain(i, len, atkS, relS);
            out[start + i] += wave(t, f) * (n.vel / 100.0) * env;
        }
    }
    applyEffects(out);
    return out;
}

double SineInst:: wave(double t, double f) {
    return sin(2 * PIv * f * t);
}

double TriangleInst:: wave(double t, double f) {
    double x = fmod(t * f, 1.0);
    return x < 0.5 ? (4 * x - 1) : (-4 * x + 3);
}

double SquareInst::wave(double t, double f) {
    double x = fmod(t * f, 1.0);
    double d = cfg.duty / 100.0;
    return x < d ? 1.0 :  -1.0;
}

unique_ptr<Instrument> makeInst(const InstrumentCfg& c,
                                 const map<string, double>& pitchTable) {
    unique_ptr<Instrument> p;

    if (c.type == "sampler") p = make_unique<SamplerInst>(c);
    else if (c. type == "sine") p = make_unique<SineInst>();
    else if (c.type == "triangle") p = make_unique<TriangleInst>();
    else if (c.type == "square") p = make_unique<SquareInst>();
    else return nullptr;

    p->cfg = c;
    p->pitchTable = &pitchTable;

    for (auto& e :  c.effects) {
        auto eff = makeEffect(e);
        if (eff) p->fx.push_back(move(eff));
    }
    return p;
}