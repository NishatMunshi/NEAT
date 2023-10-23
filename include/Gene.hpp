#pragma once
#include <cstdint>
#include <utility>
#include <map>
#include "E:/Code/custom_libraries/randomNumberGenerator.hpp"
#include "E:/Code/custom_libraries/math.hpp"
#include "Neuron.hpp"
#include <unordered_map>
#include <set>

typedef unsigned ID;
enum NeuronType : uint8_t
{
    SENSOR,
    HIDDEN,
    MOTOR
};
typedef std::pair<ID, NeuronType> NeuronGene;
typedef std::map<ID, NeuronType> NeuronGenome;

struct InnovationID
{
    unsigned startingNeuronID;
    unsigned endingNeuronID;

    InnovationID(const unsigned _startingNeuronID, const unsigned _endingNeuronID) : startingNeuronID(_startingNeuronID), endingNeuronID(_endingNeuronID) {}

}; // startingNeuronID, endingNeuronID
struct SynapseProperties
{
    float weight;
    bool enabled;

    SynapseProperties(void)
    {
        float randomWeight = float(random_16.generate()) / INT16_MIN;
        weight = randomWeight;

        enabled = true;
    }
};
typedef std::pair<InnovationID, SynapseProperties> SynapseGene;
typedef std::map<InnovationID, SynapseProperties> SynapseGenome;
struct GenePool
{
    NeuronGenome neuronGenePool;
    SynapseGenome synapseGenePool;
};

struct Genome
{
    NeuronGenome neuronGenome;
    SynapseGenome synapseGenome;

    float score = 0; // denotes how well this brain performed

    Genome cross(const Genome &_other)
    {
        Genome child = *this;

        // loop through all the synapseGenes of _other and randomly flip some genes if flippable
        for (auto &otherGene : _other.synapseGenome)
        {
            bool matched = child.synapseGenome.contains(otherGene.first);
            if (matched)
            {
                // decide whether to flip
                bool flip = random_bool.generate();
                if (flip)
                    child.synapseGenome.at(otherGene.first) = otherGene.second;
            }
            else
            {
                child.synapseGenome.insert(otherGene);
            }
        }
        return child;
    }

    // mutations
public:
    // inline void add_new_synapse(const unsigned _startingNeuronID, const unsigned _endingNeuronID)
    // {
    //     SynapseGene newGene(_startingNeuronID, _endingNeuronID);
    //     synapseGenome.push_back(newGene);
    // }
    // inline void change_weight(const unsigned _indexOfSynapseToEvolve)
    // {
    //     synapseGenome.at(_indexOfSynapseToEvolve).change_weight();
    // }
    // void evolve_a_synapse(const unsigned _indexOfSynapseToEvolve, const unsigned _newNeuronsID)
    // {
    //     auto &geneToEvolve = synapseGenome.at(_indexOfSynapseToEvolve);
    //     geneToEvolve.enabled = false;

    //     auto startingNeuronID = geneToEvolve.startingNeuronID;
    //     auto endingNeuronID = geneToEvolve.endingNeuronID;

    //     // neuron we are adding
    //     auto newEndingNeuronID = _newNeuronsID;
    //     auto newStartingNeuronID = _newNeuronsID;

    //     SynapseGene newGene1(startingNeuronID, newEndingNeuronID);
    //     SynapseGene newGene2(newStartingNeuronID, endingNeuronID);

    //     synapseGenome.push_back(newGene1);
    //     synapseGenome.push_back(newGene2);
    // }
};

// typedefs
