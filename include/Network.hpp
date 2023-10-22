#pragma once
#include <vector>
#include "Neuron.hpp"
#include "Gene.hpp"
#include <algorithm>

// remember to check later if we really need those std::sorts
class Network
{
private:
    // we need a container for all the neurons sorted by their innovaion number
    // we need an std::vector because we need to access every neuron by its innovaion number which is index in this case
    std::vector<Neuron *> m_neuronPool;

private:
    typedef std::vector<Neuron *> Layer; // we may need to add a new neuron but that can be pushed from the back
    std::vector<Layer> m_neuralNetwork;  // we may need to insert a new layer in the middle, so we use list

public:
    // construct the net according to the genome passed as argument
    Network(const Genome &_genome)
    {
        // first make all the neurons needed
        for (auto &gene : _genome.neuronGenome)
        {
            m_neuronPool.push_back(new Neuron(gene));
        }
        // sort them in ascending order of innovation number
        std::sort(m_neuronPool.begin(), m_neuronPool.end(), [](const Neuron *A, const Neuron *B)
                  { return A->innovationNumber < B->innovationNumber; });

        // find out how many layers are needed
        const auto neuronPointer = *std::max_element(m_neuronPool.begin(), m_neuronPool.end(), [](const Neuron *A, const Neuron *B)
                                                     { return A->layerIndex > B->layerIndex; });
        const unsigned numberOfLayers = neuronPointer->layerIndex + 1;

        // Now start constructing the neuralNetwork
        // push the number of layers needed
        for (unsigned index = 0; index < numberOfLayers; ++index)
            m_neuralNetwork.push_back(Layer());

        // fill the network with neurons from the neuronPool
        for (auto &neuronPointer : m_neuronPool)
        {
            if (neuronPointer->type == SENSOR)
                m_neuralNetwork.front().push_back(neuronPointer);
            else if (neuronPointer->type == MOTOR)
                m_neuralNetwork.back().push_back(neuronPointer);
            else if (neuronPointer->type == HIDDEN)
            {
                m_neuralNetwork.at(neuronPointer->layerIndex).push_back(neuronPointer);
            }
        }

        // sort the input layer and output layer
        std::sort(m_neuralNetwork.front().begin(), m_neuralNetwork.front().end(), [](const Neuron *A, const Neuron *B)
                  { return A->neuronIndex < B->neuronIndex; });
        std::sort(m_neuralNetwork.back().begin(), m_neuralNetwork.back().end(), [](const Neuron *A, const Neuron *B)
                  { return A->neuronIndex < B->neuronIndex; });

        // add the synapses to their respective parent neurons
        for (auto &gene : _genome.synapseGenome)
        {
            auto endingNeuronPointer = m_neuronPool.at(gene.endingNeuronID);
            m_neuronPool.at(gene.startingNeuronID)->add_output_synapse(gene, endingNeuronPointer);
        }
    }

    unsigned feed_forward(const std::vector<float> &_inputs)
    {
        // feed the inputs in
        for (unsigned neuronIndex = 0; neuronIndex < m_neuralNetwork.front().size(); ++neuronIndex)
        {
            m_neuralNetwork.front().at(neuronIndex)->input = _inputs.at(neuronIndex);
        }

        // feed through the whole network
        for (auto &layer : m_neuralNetwork)
        {
            for (auto &neuronPointer : layer)
            {
                neuronPointer->feed_forward();
            }
        }

        // look at the final outputs and determine which one is the largest and return its index
        auto iterator = std::max_element(m_neuralNetwork.back().begin(), m_neuralNetwork.back().end(), [](const Neuron *A, const Neuron *B)
                                         { return A->get_output() > B->get_output(); });
        return iterator - m_neuralNetwork.back().begin();
    }

public:
    ~Network(void)
    {
        for (auto &neuronPointer : m_neuronPool)
            neuronPointer->~Neuron();
        delete this;
    }
};