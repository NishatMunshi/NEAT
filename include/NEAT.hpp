#pragma once
#include "Network.hpp"

// test
#include <fstream>
#include <iostream>
// #include "E:/programming_tools/SFML-2.5.1/include/SFML/Graphics.hpp"

class NEAT
{
private:
    // we need a container for all the neurons in the genePool sorted by their innovaion number
    // we need an std::vector because we need to access every neuron by its innovaion number which is index in this case
    GenePool m_genePool;

    std::vector<Genome> m_individualGenomes;

    const unsigned m_numberOfInputs, m_numberOfOutputs, m_population;

    const float MUTATION_PROBABILITY = 0.1f;

public:
    NEAT(const unsigned _numberOfInputs, const unsigned _numberOfOutputs, const unsigned _population) : m_numberOfInputs(_numberOfInputs), m_numberOfOutputs(_numberOfOutputs), m_population(_population)
    {
        assert(_population % 4 == 0);
        // start by creating individuals in the genepool AND
        // fully connect the input to the output layer for each starting brain
        for (unsigned individualIndex = 0; individualIndex < _population; ++individualIndex)
        {
            m_individualGenomes.push_back(Genome());

            // push the obviously necessary neurons (input and output)
            // INPUT LAYER
            for (unsigned inputNeuronIndex = 0; inputNeuronIndex < _numberOfInputs; ++inputNeuronIndex)
            {
                // make the (id, neuronType pair) and push it in the neuronPool
                NeuronGene inputNeuronGene(inputNeuronIndex, SENSOR);
                m_individualGenomes.back().neuronGenome.insert(inputNeuronGene);
            }
            // OUTPUT LAYER
            for (unsigned outputNeuronIndex = 0; outputNeuronIndex < _numberOfOutputs; ++outputNeuronIndex)
            {
                // make the (id, neuronType pair) and push it in the neuronPool
                NeuronGene outputNeuronGene(_numberOfInputs + outputNeuronIndex, MOTOR);
                m_individualGenomes.back().neuronGenome.insert(outputNeuronGene);
            }

            // push all the synapses
            for (unsigned inputNeuronIndex = 0; inputNeuronIndex < _numberOfInputs; ++inputNeuronIndex)
            {
                for (unsigned outputNeuronIndex = 0; outputNeuronIndex < _numberOfOutputs; ++outputNeuronIndex)
                {
                    InnovationID id(inputNeuronIndex, _numberOfInputs + outputNeuronIndex);
                    SynapseProperties properties;
                    SynapseGene gene(id, properties);
                    m_individualGenomes.back().synapseGenome.insert(gene);
                }
            }
        }

        // keep a record of the starting genomes
        m_genePool.neuronGenePool = m_individualGenomes.front().neuronGenome;
        m_genePool.synapseGenePool = m_individualGenomes.front().synapseGenome;
    }

    void play_generation(void)
    {
        for (auto &individualGenome : m_individualGenomes)
        {
            individualGenome.score = 1000;
            individualGenome.numberOfCorrectAnswers = 0u;

            Network individualBrain(individualGenome);

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

                auto resultIndex = individualBrain.feed_forward(inputs);
                if (resultIndex == std::max_element(label.begin(), label.end()) - label.begin())
                {
                    ++individualGenome.numberOfCorrectAnswers;
                }
                auto error = individualBrain.calculate_error(label);
                individualGenome.score -= error;
            }
        }
    }
    void print_best_scorer()
    {
        // sort the genomes in descending order of their scores
        auto less = [](const Genome &A, const Genome &B)
        {
            return A.score > B.score;
        };
        std::sort(m_individualGenomes.begin(), m_individualGenomes.end(), less);

        auto &bestGenome = m_individualGenomes.front();
        std::cout << "score: " << bestGenome.score << '\n';

        std::cout << "number of neurons used: " << bestGenome.neuronGenome.size() << '\n'
                  << "number of synapses used: " << bestGenome.synapseGenome.size() << '\n'
                  << "number of correct answers: " << bestGenome.numberOfCorrectAnswers << '\n';
        // show the network of best performer
        // for (const auto &gene : bestGenome.synapseGenome)
        // {
        //     std::cout << '\n'
        //               << gene.first.startingNeuronID << ' ' << gene.first.endingNeuronID << ' ' << gene.second.weight;
        // }

        // Network bestNetwork(m_neuronPool, bestGenome);
        // bestNetwork.draw(_window);

        // show the total neuronPool
        // for (auto &neuron : m_neuronPool)
        // {
        //     std::cout
        //         << '(' << neuron->ID << ',' << unsigned(neuron->type) << ')' << ' ';
        // }
    }
    void mutate(Genome &_genome)
    {
        // choose what mutation to do (weighted probaility distribution) now it is 10 10 80
        auto randomUnsigned = random_U32.generate();

        if (randomUnsigned < UINT32_MAX * 0.1)
        {
            // add a new synapse

            // make a list of available neuron ids to choose from
            std::vector<ID> availableIDs;
            for (const auto &gene : _genome.neuronGenome)
            {
                availableIDs.push_back(gene.first);
            }
            const auto startingIndex = random_U32.generate(0, availableIDs.size() - 1);
            const auto startingNeuronID = availableIDs.at(startingIndex);

            const auto endingIndex = random_U32.generate(0, availableIDs.size() - 1);
            const auto endingNeuronID = availableIDs.at(endingIndex);

            InnovationID newInnovationID(startingNeuronID, endingNeuronID);
            SynapseProperties properties;

            // add it to both containers if not already there
            _genome.synapseGenome.insert_or_assign(newInnovationID, properties);
            m_genePool.synapseGenePool.insert_or_assign(newInnovationID, properties);
        }
        else if (randomUnsigned >= UINT32_MAX * 0.1 and randomUnsigned < UINT32_MAX * 0.2)
        {
            // evolve a synapse (add a neuron in the middle of it)
            // choose which synapse to evolve
            std::vector<InnovationID> availableInnnovationIDs;
            for (const auto &gene : _genome.synapseGenome)
            {
                availableInnnovationIDs.push_back(gene.first);
            }

            auto randomIndex = random_U32.generate(0, availableInnnovationIDs.size() - 1);
            auto oldInnovationID = availableInnnovationIDs.at(randomIndex);

            // disable it in both containers
            _genome.synapseGenome.at(oldInnovationID).enabled = false;
            m_genePool.synapseGenePool.at(oldInnovationID).enabled = false;

            // make a new neuron gene
            auto newNeuronID = m_genePool.neuronGenePool.size();

            // add it to both containers
            _genome.neuronGenome.insert_or_assign(newNeuronID, HIDDEN);
            m_genePool.neuronGenePool.insert_or_assign(newNeuronID, HIDDEN);

            // make two new synapse genes
            InnovationID newInnovationID1(oldInnovationID.startingNeuronID, newNeuronID);
            SynapseProperties properties1(_genome.synapseGenome.at(oldInnovationID).weight);

            InnovationID newInnovationID2(newNeuronID, oldInnovationID.endingNeuronID);
            SynapseProperties properties2(1.f);

            // push them in both containers
            _genome.synapseGenome.insert_or_assign(newInnovationID1, properties1);
            _genome.synapseGenome.insert_or_assign(newInnovationID2, properties2);
            m_genePool.synapseGenePool.insert_or_assign(newInnovationID1, properties1);
            m_genePool.synapseGenePool.insert_or_assign(newInnovationID2, properties2);
        }

        else if (randomUnsigned >= UINT32_MAX * 0.2 and randomUnsigned < UINT32_MAX)
        {
            // change the weight of a random synapse
            // choose which one to mutate
            std::vector<InnovationID> availableInnnovationIDs;
            for (const auto &gene : _genome.synapseGenome)
            {
                availableInnnovationIDs.push_back(gene.first);
            }
            auto randomIndex = random_U32.generate(0, availableInnnovationIDs.size() - 1);
            auto innovationID = availableInnnovationIDs.at(randomIndex);

            SynapseProperties properties;
            _genome.synapseGenome.at(innovationID) = properties;
        }
    }
    void repopulate(void)
    {
        // sort the genomes in descending order of their scores
        auto less = [](const Genome &A, const Genome &B)
        {
            return A.score > B.score;
        };
        std::sort(m_individualGenomes.begin(), m_individualGenomes.end(), less);

        // select the best m_population/2 number of individuals to repopulate
        std::vector<Genome> selectedGenomes;
        for (unsigned individualIndex = 0; individualIndex < m_population / 2; ++individualIndex)
        {
            selectedGenomes.push_back(m_individualGenomes.at(individualIndex));
        }

        // cross the genomes with each other to again produce m_population number of individuals
        m_individualGenomes.clear();
        for (unsigned index = 0; index < selectedGenomes.size(); index += 2)
        {
            auto &father = selectedGenomes.at(index);
            auto &mother = selectedGenomes.at(index + 1);

            // each couple produces four children
            Genome children[4];
            bool whetherToMutate[4];

            for (unsigned childIndex = 0; childIndex < 4u; ++childIndex)
            {
                children[childIndex] = father.cross(mother);

                // decide whether to mutate this child
                auto random = random_U32.generate();
                whetherToMutate[childIndex] = random < UINT32_MAX * MUTATION_PROBABILITY;
                if (whetherToMutate[childIndex])
                    mutate(children[childIndex]);

                // push the child to the m_individualGenePool
                m_individualGenomes.push_back(children[childIndex]);
            }
        }
    }
};
