#pragma once
#include "Neuron.hpp"
#include "Gene.hpp"
#include "Network.hpp"
#include <algorithm>
#include <cmath>

// test
#include <fstream>
#include <iostream>
#include "E:/programming_tools/SFML-2.5.1/include/SFML/Graphics.hpp"

#define MUTATION_PROBABILITY 0.1f

// we dont need to think explicitly about bias neurons, NEAT will add them if necessary
class NEAT
{
private:
    // we need a container for all the neurons in the genePool sorted by their innovaion number
    // we need an std::vector because we need to access every neuron by its innovaion number which is index in this case
    std::vector<NeuronGene> m_neuronGenePool;

    std::vector<Genome> m_synapseGenePool;

    const unsigned m_numberOfInputs, m_numberOfOutputs, population;

public:
    NEAT(const unsigned _numberOfInputs, const unsigned _numberOfOutputs, const unsigned _population) : m_numberOfInputs(_numberOfInputs), m_numberOfOutputs(_numberOfOutputs), population(_population)
    {
        // start by creating empty genomes in the genepool AND
        // fully connect the input to the output layer for each starting brain
        for (unsigned individualIndex = 0; individualIndex < _population; ++individualIndex)
        {
            m_genePool.push_back(Genome());

            for (unsigned inputIndex = 0; inputIndex < _numberOfInputs; ++inputIndex)
            {
                for (unsigned outputIndex = 0; outputIndex < _numberOfOutputs; ++outputIndex)
                {
                    SynapseGene synapseGene(inputIndex, _numberOfInputs + outputIndex);
                    m_genePool.back().synapseGenome.push_back(synapseGene);
                }
            }
        }

        // push the obviously necessary neurons (input and output)
        // INPUT LAYER
        for (unsigned inputNeuronIndex = 0; inputNeuronIndex < _numberOfInputs; ++inputNeuronIndex)
        {
            // make the gene
            NeuronGene neuronGene(SENSOR, 0u, inputNeuronIndex, m_neuronPool.size());

            // make the neuron according to the gene and push it in the neuronPool
            auto neuronPointer = new Neuron(neuronGene);
            m_neuronPool.push_back(neuronPointer);

            // add this gene to every genome in the starting population
            for (auto &genome : m_genePool)
            {
                genome.neuronGenome.push_back(neuronGene);
            }
        }
        // OUTPUT LAYER
        for (unsigned outputNeuronIndex = 0; outputNeuronIndex < _numberOfInputs; ++outputNeuronIndex)
        {
            // make the gene
            NeuronGene neuronGene(MOTOR, 1u, outputNeuronIndex, m_neuronPool.size());

            // make the neuron according to the gene and push it in the neuronPool
            auto neuronPointer = new Neuron(neuronGene);
            m_neuronPool.push_back(neuronPointer);

            // add this gene to every genome in the starting population
            for (auto &genome : m_genePool)
            {
                genome.neuronGenome.push_back(neuronGene);
            }
        }
    }
    void play_generation(void)
    {
        for (auto &individualGenome : m_genePool)
        {
            individualGenome.score = 100;
            Network individualBrain(m_neuronPool, individualGenome);

            // this is some test code , subject to change later
            std::ifstream file;
            file.open("E:/Code/NEAT/xor.txt");

            float input1, input2, output;

            while (not file.eof())
            {
                file >> input1;
                file >> input2;
                file >> output;

                std::vector<float> inputs;
                inputs.push_back(input1);
                inputs.push_back(input2);

                std::vector<float> label;
                label.push_back(-1);
                label.push_back(-1);

                if (output == -1)
                    label[0] = 1;
                else
                    label[1] = 1;

                auto result = individualBrain.feed_forward(inputs);

                auto error = individualBrain.calculate_error(label);
                individualGenome.score -= error;
            }
        }
    }
    void print_best_scorer(sf::RenderWindow &_window)
    {
        // sort the genomes in descending order of their scores
        auto less = [](const Genome &A, const Genome &B)
        {
            return A.score > B.score;
        };
        std::sort(m_genePool.begin(), m_genePool.end(), less);

        auto &bestGenome = m_genePool.front();
        std::cout << "score: " << bestGenome.score << '\n';

        // show the network of best performer
        for (auto &gene : bestGenome.synapseGenome)
        {
            std::cout << '\n'
                      << gene.startingNeuronID << ' ' << gene.endingNeuronID << ' ' << gene.weight;
        }

        Network bestNetwork(m_neuronPool, bestGenome);
        bestNetwork.draw(_window);

        // show the total neuronPool
        // for (auto &neuron : m_neuronPool)
        // {
        //     std::cout
        //         << '(' << neuron->ID << ',' << unsigned(neuron->type) << ')' << ' ';
        // }
    }
    void mutate(Genome &_genome)
    {
        // choose what mutation to do (weighted probaility distribution) now it is 10 80 5 5
        auto randomByte = random_U8.generate();

        Network network(m_neuronPool, _genome);
        const auto &net = network.neuralNetwork;
        if (randomByte < UINT8_MAX * 0.1)
        {
            // add a new synapse (from a preceding layer to a following layer/ no feedback)

            // input or one of the hidden layers
            auto randomStartingLayerIndex = random_U32.generate(0, net.size() - 2);
            auto randomStartingNeuronIndex = random_U32.generate(0, net.at(randomStartingLayerIndex).size() - 1);
            auto randomStartingNeuronID = net.at(randomStartingLayerIndex).at(randomStartingNeuronIndex)->ID;

            // one of the hidden layers or output layer
            auto randomEndingLayerIndex = random_U32.generate(randomStartingLayerIndex + 1, net.size() - 1);
            auto randomEndingNeuronIndex = random_U32.generate(0, net.at(randomEndingLayerIndex).size() - 1);
            auto randomEndingNeuronID = net.at(randomEndingLayerIndex).at(randomEndingNeuronIndex)->ID;

            // check if the connection already exists, then only change its weight
            auto innovationNumber = power(2, randomStartingNeuronID) * power(3, randomEndingNeuronID);
            for (auto &synapseGene : _genome.synapseGenome)
            {
                if (synapseGene.innovation_number() == innovationNumber)
                {
                    synapseGene.change_weight();
                    return;
                }
            }

            // if the synapse does not exist then add it
            _genome.add_new_synapse(randomStartingNeuronID, randomEndingNeuronID);
        }
        else if (randomByte >= UINT8_MAX * 0.1 and randomByte < UINT8_MAX * 0.9)
        { // change the weight of a random synapse
            // choose which one to mutate
            auto randomSyanpseIndex = random_U32.generate(0, _genome.synapseGenome.size() - 1);
            _genome.change_weight(randomSyanpseIndex);
        }
        else if (randomByte >= UINT8_MAX * 0.9 and randomByte < UINT8_MAX )
        {
            // evolve a synapse (add a neuron in the middle of it)
            // choose which synapse to evolve
            auto randomSynapseIndex = random_U32.generate(0, _genome.synapseGenome.size() - 1);
            auto newLayerIndex = m_neuronPool.at(_genome.synapseGenome.at(randomSynapseIndex).startingNeuronID)->layerIndex + 1;

            // check if the layer already exists
            bool layerAlreadyExists = net.size() > newLayerIndex + 1u;
            unsigned newNeuronIndex = 0;
            if (layerAlreadyExists)
                newNeuronIndex = net.at(newLayerIndex).size();

            // add the neuron to the pool of neurons
            auto newNeuronID = m_neuronPool.size();
            NeuronGene neuronGene(HIDDEN, newLayerIndex, newNeuronIndex, newNeuronID);
            auto newNeuronPointer = new Neuron(neuronGene);
            m_neuronPool.push_back(newNeuronPointer);

            _genome.evolve_a_synapse(randomSynapseIndex, newNeuronID);
        }
        else if (randomByte >= UINT8_MAX * 0.95 and randomByte < UINT8_MAX)
        {
            // add a random bias type neuron in
            // input or one of the hidden layers
            auto randomStartingLayerIndex = random_U32.generate(0, net.size() - 2);
            auto nextLayerIndex = randomStartingLayerIndex + 1u;

            // add the neuron to the pool of neurons
            auto newNeuronID = m_neuronPool.size();
            NeuronGene neuronGene(BIAS, randomStartingLayerIndex, net.at(randomStartingLayerIndex).size(), newNeuronID);
            auto newNeuronPointer = new Neuron(neuronGene);
            m_neuronPool.push_back(newNeuronPointer);

            // add all synapses from it to all the neurons in the next layer
            for (auto &nextLayerNeuron : net.at(nextLayerIndex))
            {
                auto endingNeuronID = nextLayerNeuron->ID;
                SynapseGene newGene(newNeuronID, endingNeuronID);

                _genome.synapseGenome.push_back(newGene);
            }
        }
    }
    void repopulate(void)
    {
        // sort the genomes in descending order of their scores
        auto less = [](const Genome &A, const Genome &B)
        {
            return A.score > B.score;
        };
        std::sort(m_genePool.begin(), m_genePool.end(), less);

        // select the best sqrt(population) number of individuals to repopulate
        std::vector<Genome> selectedGenomes;
        for (unsigned individualIndex = 0; individualIndex < sqrt(population); ++individualIndex)
        {
            selectedGenomes.push_back(m_genePool.at(individualIndex));
        }

        // cross the genomes with each other to again produce population number of individuals
        m_genePool.clear();
        for (auto &father : selectedGenomes)
        {
            for (auto &mother : selectedGenomes)
            {
                auto child = father.cross(mother);

                // decide whether to mutate
                auto randomUnsigned = random_U32.generate();
                bool whetherToMutate = randomUnsigned < UINT32_MAX * MUTATION_PROBABILITY;

                if (whetherToMutate)
                    mutate(child);

                m_genePool.push_back(child);
            }
        }
    }
};
