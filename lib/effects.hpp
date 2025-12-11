#pragma once
#include "types.hpp"
#include "utils.hpp"
#include <memory>
#include <vector>

class Effect {
public:
    virtual void apply(std::vector<float>& buf, double sr) = 0;
    virtual ~Effect() = default;
};

class GainEffect : public Effect {
    double gain;
public:
    GainEffect(double g) : gain(g) {}
    void apply(std::vector<float>& b, double) override;
};

class EchoEffect : public Effect {
    int delaySamples;
    double decay;
public:
    EchoEffect(double delay, double dec);
    void apply(std::vector<float>& b, double) override;
};

class TremoloEffect : public Effect {
    double freq, depth;
public:
    TremoloEffect(double f, double d) : freq(f), depth(d) {}
    void apply(std::vector<float>& b, double sr) override;
};

std::unique_ptr<Effect> makeEffect(const EffectCfg& c);