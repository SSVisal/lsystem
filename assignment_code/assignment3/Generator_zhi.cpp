#include "Generator_zhi.hpp"

/*
    Based on: https://github.com/abiusx/L3D:
    + = turn right
    - = turn left
    & = pitch down
    ^ = pitch up
    < or \ = roll left
    > or / = roll right
    | = turn 180 degree
    f or F = draw branch (and go forward)
    g = go forward (no drawing)
    [ = save state (i.e. start branch)
    ] = restore state (i.e. end branch and go back to where it started)
    * : draw leaf
    // it has more for thickness and stuff but i choose to ignore
    
    where turn, pitch, roll are in terms of different axis

*/

namespace GLOO{
Generator::Generator(std::string axiom, std::map<std::string, std::vector<Replacement>>& prod_rules)
    : current_(axiom), rules_(prod_rules) {
}

void Generator::AddRule(const std::string& key, const Replacement& replace_to){
    auto it = rules_.find(key);

    // no rule, make
    if (it == rules_.end()) {
        std::vector<Replacement> rules_vec = {replace_to}; 
        rules_[key] = rules_vec;
    }
    // already rules
    else {
        rules_[key].push_back(replace_to);
    }
}

std::string Generator::Generate(int num_steps){
    for (int i = 0; i < num_steps; i++) {
        std::string next;

        for (char c : current_) {
            std::string key(1, c);
            auto it = rules_.find(key);

            // no rule, keep
            if (it == rules_.end()) {
                next.push_back(c);
                continue;
            }

            // sample from rules
            const auto& c_rules = it->second;
            next += SampleFromRuleVector(c_rules, key);
        }

        current_ = next;
    }
    return current_;
}

std::string Generator::SampleFromRuleVector(const std::vector<Replacement>& rules_vec, const std::string& original) {
    double total = 0.0;
    for (const auto& r : rules_vec) total += r.chance;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, total);

    double r = dist(gen);

    double cumulative = 0.0;
    for (const auto& rule_item : rules_vec){
        cumulative += rule_item.chance;
        if (r <= cumulative) {
            return rule_item.replacement;
        }
    }

    return original;
}
}  // namespace GLOO