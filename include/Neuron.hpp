#pragma once
#include "Gene.hpp"
#include <vector>
#include <cmath>
// #include "E:/Code/custom_libraries/randomNumberGenerator.hpp"

class Neuron
{
protected:
    struct Synapse
    {
        Neuron *endingNeuronPointer;
        float weight;
        bool enabled;

        Synapse(const SynapseGene &_gene, Neuron *_endingNeuronPointer) : endingNeuronPointer(_endingNeuronPointer), weight(_gene.weight), enabled(_gene.enabled)
        {
        }
    };

protected:
    float m_output;
    std::vector<Synapse *> m_outputSynapsePointers;

protected:
    static inline float activation_function(const float _x) { return tanh(_x); }

public:
    float input;
    NeuronType type;

    // remember to chcek later if sorting is necessary, if not innovationNUmber and neuronIndex are not needed here
    unsigned innovationNumber;        // needed for sorting the neurons based on ID
    unsigned layerIndex, neuronIndex; // neuronIndex is needed for maintaining order in input and output layer

public:
    Neuron(const NeuronGene &_gene) : innovationNumber(_gene.innovationNumber), type(_gene.type), layerIndex(_gene.layerIndex), neuronIndex(_gene.neuronIndex)
    {
        input = 0.f;
        m_output = 1.f;
    }
    inline void add_output_synapse(const SynapseGene &_synapseGene, Neuron *_endingNeuronPointer)
    {
        m_outputSynapsePointers.push_back(new Synapse(_synapseGene, _endingNeuronPointer));
    }

    void feed_forward(void)
    {
        m_output = activation_function(input);
        input = 0.f;

        for (auto &synapse : m_outputSynapsePointers)
        {
            synapse->endingNeuronPointer->input += synapse->weight * m_output;
        }
    }
    inline float get_output(void) const { return m_output; };

public:
    ~Neuron(void)
    {
        for (auto &synapsePointer : m_outputSynapsePointers)
            delete synapsePointer;
        delete this;
    }
};
