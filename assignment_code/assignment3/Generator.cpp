#include "Generator.hpp"


//Add a token for split and a parameter that describes how likely the copy is to be the same as the original
//s = split, rotating 45 deg and adding a duplicate at 90 deg, then add branch, l = leaf, r = turn right 15 deg, f = turn left 15 deg, b = branch
namespace GLOO{
Generator::Generator(int num_steps): num_steps_(num_steps){
    tokens_.push_back("b");
    leaf_map_["s"] = 0.5;
    leaf_map_["l"] = 0.5;
    leaf_map_["r"] = 0.5;
    leaf_map_["f"] = 0.5;
    leaf_map_["b"] = 0.5;

    stem_map_["s"] = 0.5;
    stem_map_["l"] = 0.5;
    stem_map_["r"] = 0.5;
    stem_map_["f"] = 0.5;
    stem_map_["b"] = 0.5;

    std::cout<<leaf_map_.size()<<std::endl;

}
std::vector<std::string> Generator::Generate(){
    for(int i = 0; i < num_steps_; i++){
        std::map<std::string, float> prob;
        for(int s = tokens_.size(); s > 0; s--){
            if(tokens_[s] == "l"){
                prob = leaf_map_;
                break;
            }else if(tokens_[s] == "b"){
                prob = stem_map_;
                break;
            }else{
                continue;
            }
        }
        std::string token = SampleFromMap(prob);
        tokens_.push_back(std::move(token));
    }
    return tokens_;
}
std::string Generator::SampleFromMap(std::map<std::string, float>& probs) {
    double total = 2.5;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, total);

    double r = dist(gen);

    double cumulative = 0.0;
    for (auto& k : probs){
        std::cout<<k.first<<k.second<<std::endl;
        cumulative += k.second;
        if (r <= cumulative) {
            return std::move(k.first);
        }else{
            return "b";
        }
    }
}
}