#pragma once
#include <cstdint>
#include <vector>
#include "E:/Code/custom_libraries/math.hpp"

enum NeuronType : uint8_t
{
    SENSOR,
    HIDDEN,
    MOTOR
};
struct NeuronGene
{
    NeuronType type;

    unsigned innovationNumber;
    unsigned layerIndex, neuronIndex;
};

struct SynapseGene
{
    unsigned startingNeuronID, endingNeuronID;
    float weight;
    bool enabled;

    inline unsigned innovation_number(void) const { return power(2, startingNeuronID) * power(3, endingNeuronID); }
};

struct Genome
{
    std::vector<NeuronGene> neuronGenome;
    std::vector<SynapseGene> synapseGenome;
};
