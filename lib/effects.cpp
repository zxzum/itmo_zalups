#include "effects.hpp"
#include <cmath>

using namespace std;

void GainEffect::apply(vector<float>& b, double) {
    for (auto& x : b) x *= gain;
}

EchoEffect::EchoEffect(double delay, double dec)
    : delaySamples((int)(delay * TARGET_SR)), decay(dec) {}

void EchoEffect:: apply(vector<float>& b, double) {
    for (size_t i = delaySamples; i < b.size(); i++) {
        b[i] += decay * b[i - delaySamples];
    }
}

void TremoloEffect::apply(vector<float>& b, double sr) {
    for (size_t i = 0; i < b.size(); i++) {
        double t = i / sr;
        double m = 1 - depth + depth * sin(2 * PIv * freq * t);
        b[i] *= m;
    }
}

unique_ptr<Effect> makeEffect(const EffectCfg& c) {
    if (c. type == "gain") return make_unique<GainEffect>(c. a);
    if (c.type == "echo") return make_unique<EchoEffect>(c. a, c.b);
    if (c.type == "tremolo") return make_unique<TremoloEffect>(c. a, c.b);
    return nullptr;
}