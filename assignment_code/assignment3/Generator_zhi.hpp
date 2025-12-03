#ifndef GENERATOR_ZHI_H_
#define GENERATOR_ZHI_H_

#include <stdexcept>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

#include "gloo/utils.hpp"

struct Replacement
{
   std::string replacement;
   double chance;
};

namespace GLOO
{
   class Generator
   {
   public:
      Generator(std::string axiom, std::map<std::string, std::vector<Replacement>> &prod_rules);

      void AddRule(const std::string &key, const Replacement &replace_to);
      std::string Generate(int num_steps);

   private:
      std::string SampleFromRuleVector(const std::vector<Replacement> &rules_vec, const std::string &original);
      std::string current_;
      std::map<std::string, std::vector<Replacement>> rules_;
   };
} // namespace GLOO

#endif