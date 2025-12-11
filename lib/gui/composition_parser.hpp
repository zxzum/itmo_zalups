#pragma once

#include <string>
#include <vector>
#include <map>

namespace itmoloops {
namespace gui {

// Simple structure to hold parsed composition data for display
struct Note {
    int start;
    std::string instrument;
    std::string pitch;
    int duration;
    int velocity;
};

struct Pattern {
    std::string name;
    int resolution;
    std::vector<Note> notes;
    std::vector<std::string> pattern_refs;
};

struct Effect {
    std::string type;
    std::map<std::string, std::string> params;
};

struct Instrument {
    std::string name;
    std::string type;
    std::map<std::string, std::string> params;
    std::vector<Effect> effects;
};

struct Composition {
    int bpm;
    std::vector<Instrument> instruments;
    std::vector<Pattern> patterns;
    
    void Clear() {
        bpm = 120;
        instruments.clear();
        patterns.clear();
    }
};

// Simple parser to extract basic composition structure from text
class CompositionParser {
public:
    static Composition Parse(const std::string& content);
    
private:
    static std::vector<std::string> SplitLines(const std::string& content);
    static std::string Trim(const std::string& str);
    static std::vector<std::string> Split(const std::string& str, char delim);
};

} // namespace gui
} // namespace itmoloops
