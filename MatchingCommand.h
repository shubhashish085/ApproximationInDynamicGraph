#ifndef ADG_MATCHINGCOMMAND_H
#define ADG_MATCHINGCOMMAND_H

#include "CommandParser.h"
#include <map>
#include <iostream>
enum OptionKeyword {
    DataGraphFile = 0,      // -d, The data graph file path, compulsive parameter
    OutputFilePath = 1,      // -output, the output file path, compulsive parameter
    AlgorithmSerial = 2,
    MemoryBudget = 3,
    SamplingProbability = 4
};

class MatchingCommand : public CommandParser{
private:
    std::map<OptionKeyword, std::string> options_key;
    std::map<OptionKeyword, std::string> options_value;

private:
    void processOptions();

public:
    MatchingCommand(int argc, char **argv);

    std::string getDataGraphFilePath() {
        return options_value[OptionKeyword::DataGraphFile];
    }

    std::string getOutputFilePath(){
        return options_value[OptionKeyword::OutputFilePath];
    }

    std::string getAlgorithmSerial(){
        return options_value[OptionKeyword::AlgorithmSerial];
    }

    std::string getMemoryBudget(){
        return options_value[OptionKeyword::MemoryBudget];
    }

    std::string getSamplingProbability(){
        return options_value[OptionKeyword::SamplingProbability];
    }

};

#endif