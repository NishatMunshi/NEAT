#pragma once
#include <cstdint>
#include "E:/Code/custom_libraries/randomNumberGenerator.hpp"
#include "E:/Code/custom_libraries/math.hpp"
#include "Neuron.hpp"

enum NeuronType : uint8_t
{
    SENSOR,
    HIDDEN,
    MOTOR
};
struct NeuronGene
{
    NeuronType type;

    NeuronGene(const NeuronType &_type) : type(_type) {}
};
struct SynapseGene
{
    unsigned startingNeuronID, endingNeuronID;
    float weight;
    bool enabled;

    SynapseGene(const unsigned _startingNeuronID, const unsigned _endingNeuronID) : startingNeuronID(_startingNeuronID), endingNeuronID(_endingNeuronID)
    {
        // generate a random weight between -1 and 1
        auto randomFloat = (float)random_16.generate() / INT16_MIN;
        weight = randomFloat;

        // each new synapseGene starts enabled
        enabled = true;
    }

    inline unsigned innovation_number(void) const { return power(2, startingNeuronID) * power(3, endingNeuronID) + unsigned(enabled); }

    inline void change_weight(void)
    {
        auto randomWeight = (float)random_32.generate() / INT32_MIN;
        weight = randomWeight;
    }
};
struct Genome
{
    std::vector<SynapseGene> synapseGenome;

    float score = 0; // denotes how well this brain performed

    Genome cross(const Genome &_other)
    {
        Genome child = *this;

        // loop through all the synapseGenes of _other and randomly flip some genes if flippable
        for (auto &otherGene : _other.synapseGenome)
        {
            bool matched = false;
            for (auto &thisGene : child.synapseGenome)
            {
                // keep a record whether we matched this gene
                matched = thisGene.innovation_number() == otherGene.innovation_number();
                if (matched)
                {

                    // decide whether to flip
                    bool flip = random_bool.generate();
                    if (flip)
                        thisGene = otherGene;

                    break;
                }
            }
            // if otherGene's innovationNumber is different, it means it is a new Gene and therefore it must be pushed into child's genome
            if (not matched)
                child.synapseGenome.push_back(otherGene);
        }

        return child;
    }

    // mutations
    inline void add_new_synapse(const unsigned _startingNeuronID, const unsigned _endingNeuronID)
    {
        SynapseGene newGene(_startingNeuronID, _endingNeuronID);
        synapseGenome.push_back(newGene);
    }
    inline void change_weight(const unsigned _indexOfSynapseToEvolve)
    {
        synapseGenome.at(_indexOfSynapseToEvolve).change_weight();
    }
    void evolve_a_synapse(const unsigned _indexOfSynapseToEvolve, const unsigned _newNeuronsID)
    {
        auto &geneToEvolve = synapseGenome.at(_indexOfSynapseToEvolve);
        geneToEvolve.enabled = false;

        auto startingNeuronID = geneToEvolve.startingNeuronID;
        auto endingNeuronID = geneToEvolve.endingNeuronID;

        // neuron we are adding
        auto newEndingNeuronID = _newNeuronsID;
        auto newStartingNeuronID = _newNeuronsID;

        SynapseGene newGene1(startingNeuronID, newEndingNeuronID);
        SynapseGene newGene2(newStartingNeuronID, endingNeuronID);

        synapseGenome.push_back(newGene1);
        synapseGenome.push_back(newGene2);
    }
};

// typedefs
