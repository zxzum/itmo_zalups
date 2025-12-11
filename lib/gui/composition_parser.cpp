#include "composition_parser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace itmoloops {
namespace gui {

std::string CompositionParser::Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::vector<std::string> CompositionParser::SplitLines(const std::string& content) {
    std::vector<std::string> lines;
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::vector<std::string> CompositionParser::Split(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::istringstream stream(str);
    std::string token;
    while (std::getline(stream, token, delim)) {
        token = Trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

Composition CompositionParser::Parse(const std::string& content) {
    Composition comp;
    comp.bpm = 120; // default
    
    auto lines = SplitLines(content);
    
    size_t i = 0;
    while (i < lines.size()) {
        std::string line = Trim(lines[i]);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            i++;
            continue;
        }
        
        // Parse BPM
        if (line.find("bpm") == 0) {
            auto tokens = Split(line, ' ');
            if (tokens.size() >= 2) {
                try {
                    comp.bpm = std::stoi(tokens[1]);
                } catch (...) {}
            }
            i++;
            continue;
        }
        
        // Parse instrument
        if (line.find("instrument") == 0) {
            Instrument inst;
            auto tokens = Split(line, ' ');
            if (tokens.size() >= 3) {
                inst.name = tokens[1];
                inst.type = tokens[2];
            }
            
            i++;
            while (i < lines.size()) {
                line = Trim(lines[i]);
                if (line == "end") {
                    i++;
                    break;
                }
                
                if (line.find("effect") == 0) {
                    Effect effect;
                    auto etokens = Split(line, ' ');
                    if (etokens.size() >= 2) {
                        effect.type = etokens[1];
                        for (size_t j = 2; j < etokens.size(); ++j) {
                            auto kv = Split(etokens[j], '=');
                            if (kv.size() == 2) {
                                effect.params[kv[0]] = kv[1];
                            }
                        }
                    }
                    inst.effects.push_back(effect);
                } else if (line.find('=') != std::string::npos) {
                    auto kv = Split(line, '=');
                    if (kv.size() == 2) {
                        inst.params[kv[0]] = kv[1];
                    }
                }
                i++;
            }
            
            comp.instruments.push_back(inst);
            continue;
        }
        
        // Parse pattern
        if (line.find("pattern") == 0) {
            Pattern pattern;
            auto tokens = Split(line, ' ');
            if (tokens.size() >= 3) {
                pattern.name = tokens[1];
                if (tokens.size() >= 4 && tokens[2] == "resolution") {
                    try {
                        pattern.resolution = std::stoi(tokens[3]);
                    } catch (...) {
                        pattern.resolution = 8;
                    }
                } else {
                    try {
                        pattern.resolution = std::stoi(tokens[2]);
                    } catch (...) {
                        pattern.resolution = 8;
                    }
                }
            }
            
            i++;
            while (i < lines.size()) {
                line = Trim(lines[i]);
                if (line == "end") {
                    i++;
                    break;
                }
                
                // Parse note or pattern reference
                if (!line.empty() && !line.empty() && line[0] == '@') {
                    pattern.pattern_refs.push_back(line);
                } else {
                    auto tokens = Split(line, ' ');
                    if (tokens.size() >= 5) {
                        Note note;
                        try {
                            note.start = std::stoi(tokens[0]);
                            note.instrument = tokens[1];
                            note.pitch = tokens[2];
                            note.duration = std::stoi(tokens[3]);
                            note.velocity = std::stoi(tokens[4]);
                            pattern.notes.push_back(note);
                        } catch (...) {}
                    }
                }
                i++;
            }
            
            comp.patterns.push_back(pattern);
            continue;
        }
        
        i++;
    }
    
    return comp;
}

} // namespace gui
} // namespace itmoloops
