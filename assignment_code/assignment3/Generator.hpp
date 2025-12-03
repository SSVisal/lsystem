#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <stdexcept>
#include <iostream>
#include <map>
#include <random>
#include <string>

#include "gloo/utils.hpp"

namespace GLOO {
class Generator {
 public:
    Generator(int num_steps);
    std::vector<std::string> Generate();
    std::string SampleFromMap(std::map<std::string, float>& probs);
 private:
    std::map<std::string, float> leaf_map_;
    std::map<std::string, float> stem_map_;
    int num_steps_;
    std::vector<std::string> tokens_;
};
}  // namespace GLOO

#endif