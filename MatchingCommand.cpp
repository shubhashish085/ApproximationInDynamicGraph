#include "MatchingCommand.h"

MatchingCommand::MatchingCommand(const int argc, char **argv) : CommandParser(argc, argv) {
    // Initialize options value
    options_key[OptionKeyword::DataGraphFile] = "-d";
    options_key[OptionKeyword::OutputFilePath] = "-o";
    options_key[OptionKeyword::AlgorithmSerial] = "-a";
    options_key[OptionKeyword::MemoryBudget] = "-m";
    options_key[OptionKeyword::SamplingProbability] = "-s";
    processOptions();
};

void MatchingCommand::processOptions() {

    options_value[OptionKeyword::DataGraphFile] = getCommandOption(options_key[OptionKeyword::DataGraphFile]);
    options_value[OptionKeyword::OutputFilePath] = getCommandOption(options_key[OptionKeyword::OutputFilePath]);
    options_value[OptionKeyword::AlgorithmSerial] = getCommandOption(options_key[OptionKeyword::AlgorithmSerial]);
    options_value[OptionKeyword::MemoryBudget] = getCommandOption(options_key[OptionKeyword::MemoryBudget]);
    options_value[OptionKeyword::SamplingProbability] =  getCommandOption(options_key[OptionKeyword::SamplingProbability]);
}