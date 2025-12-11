#pragma once
#include "types.hpp"
#include "effects.hpp"
#include <memory>
#include <vector>
#include <map>

class Instrument {
public:
    InstrumentCfg cfg;
    std::vector<std::unique_ptr<Effect>> fx;
    const std::map<std::string, double>* pitchTable = nullptr;

    virtual std::vector<float> render(const std::vector<NoteEvent>& notes, double totalSec) = 0;
    virtual ~Instrument() = default;

protected:
    double getFreq(const std:: string& pitch);
    void applyEffects(std::vector<float>& buf);
};

class SamplerInst : public Instrument {
    WavData wav;
public:
    SamplerInst(const InstrumentCfg& c);
    std::vector<float> render(const std::vector<NoteEvent>& notes, double totalSec) override;
};

class OscBase :  public Instrument {
protected:
    virtual double wave(double t, double freq) = 0;
public:
    std::vector<float> render(const std:: vector<NoteEvent>& notes, double totalSec) override;
};

class SineInst : public OscBase {
protected:
    double wave(double t, double f) override;
};

class TriangleInst :  public OscBase {
protected:
    double wave(double t, double f) override;
};

class SquareInst : public OscBase {
protected:
    double wave(double t, double f) override;
};

std::unique_ptr<Instrument> makeInst(const InstrumentCfg& c,
                                      const std::map<std::string, double>& pitchTable);