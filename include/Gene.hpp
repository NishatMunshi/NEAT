#pragma once
#include <map>
#include "../libraries/randomNumberGenerator.hpp"
#include <unordered_map>

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
    InnovationID(void) : startingNeuronID(0u), endingNeuronID(0u) {}

    inline bool operator==(const InnovationID &_other) const
    {
        return this->startingNeuronID == _other.startingNeuronID and this->endingNeuronID == _other.endingNeuronID;
    }
};

// custom hash function for innovationID
template <>
struct std::hash<InnovationID>
{
    inline std::size_t operator()(const InnovationID &_id) const
    {
        return ((std::hash<unsigned>()(_id.startingNeuronID) xor (std::hash<unsigned>()(_id.endingNeuronID) << 1)) >> 1);
    }
};
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
    SynapseProperties(const float _weight) : weight(_weight)
    {
        enabled = true;
    }
};


typedef std::pair<InnovationID, SynapseProperties> SynapseGene;
typedef std::unordered_map<InnovationID, SynapseProperties> SynapseGenome;
struct GenePool
{
    NeuronGenome neuronGenePool;
    SynapseGenome synapseGenePool;
};

struct Genome
{
    NeuronGenome neuronGenome;
    SynapseGenome synapseGenome;

    float score; // denotes how well this brain performed
    unsigned numberOfCorrectAnswers;

    Genome cross(const Genome &_other)
    {
        Genome child = *this;
        auto copyOfOtherNeuronGenome = _other.neuronGenome;
        child.neuronGenome.merge(copyOfOtherNeuronGenome);

        // loop through all the synapseGenes of _other and randomly flip some genes if flippable
        for (const auto &otherGene : _other.synapseGenome)
        {
            bool matched = child.synapseGenome.count(otherGene.first);
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
