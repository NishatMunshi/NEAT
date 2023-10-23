#pragma once
#include <vector>
#include "Neuron.hpp"
#include "Gene.hpp"
#include <set>
#include <map>
#include <cassert>

// TEST
#include "E:/programming_tools/SFML-2.5.1/include/SFML/Graphics.hpp"
#define WINDOW_DIMENSION 900

struct Network
{
public:
    typedef std::vector<Neuron *> Layer; // we may need to add a new neuron but that can be pushed from the back
    std::vector<Layer> neuralNetwork;    // we may need to insert a new layer in the middle, so we use list

    std::map<unsigned, Neuron *> IDtoPointerMap;

public:
    // construct the net by arranging the layers
    Network(const std::vector<NeuronGene> &_neuronGenePool, const Genome &_genome)
    {
        // push the number of layers needed (three - input, hidden, output)
        neuralNetwork.push_back(Layer()); // input
        neuralNetwork.push_back(Layer()); // hidden
        neuralNetwork.push_back(Layer()); // output

        // find which neurons are needed for this genome
        std::set<unsigned> neededNeuronIDs;
        for (const auto &gene : _genome.synapseGenome)
        {
            neededNeuronIDs.insert(gene.startingNeuronID);
            neededNeuronIDs.insert(gene.endingNeuronID);
        }

        for (const auto &neededNeuronID : neededNeuronIDs)
        {
            auto &neuronGene = _neuronGenePool.at(neededNeuronID);
            auto type = neuronGene.type;

            if (type == SENSOR)
            {
                auto neuronIndex = neuralNetwork.front().size();
                auto newNeuronPointer = new Neuron(type, neuronIndex);
                neuralNetwork.front().push_back(newNeuronPointer);

                // keep a record of it in the map
                IDtoPointerMap.insert(std::pair(neededNeuronID, newNeuronPointer));
            }
            else if (type == HIDDEN)
            {

                auto neuronIndex = neuralNetwork.at(1).size();
                auto newNeuronPointer = new Neuron(type, neuronIndex);
                neuralNetwork.at(1).push_back(newNeuronPointer);

                // keep a record of it in the map
                IDtoPointerMap.insert(std::pair(neededNeuronID, newNeuronPointer));
            }
            else if (type == MOTOR)
            {
                auto neuronIndex = neuralNetwork.back().size();
                auto newNeuronPointer = new Neuron(type, neuronIndex);
                neuralNetwork.back().push_back(newNeuronPointer);

                // keep a record of it in the map
                IDtoPointerMap.insert(std::pair(neededNeuronID, newNeuronPointer));
            }
        }

        // add the new synapses
        for (const auto &synapseGene : _genome.synapseGenome)
        {
            auto startingNeuronPointer = IDtoPointerMap[synapseGene.startingNeuronID];
            auto endingNeuronPointer = IDtoPointerMap[synapseGene.endingNeuronID];
            startingNeuronPointer->add_output_synapse(synapseGene, endingNeuronPointer);
        }
    }

    unsigned feed_forward(const std::vector<float> &_inputs)
    {
        assert(_inputs.size() == neuralNetwork.front().size());

        // feed the inputs in
        for (unsigned neuronIndex = 0; neuronIndex < neuralNetwork.front().size(); ++neuronIndex)
        {
            neuralNetwork.front().at(neuronIndex)->set_input(_inputs.at(neuronIndex));
        }

        // feed through the whole network
        for (auto &layer : neuralNetwork)
        {
            for (auto &neuronPointer : layer)
            {
                neuronPointer->feed_forward();
            }
        }

        // look at the final outputs and determine which one is the largest and return its index
        auto iterator = std::max_element(neuralNetwork.back().begin(), neuralNetwork.back().end(), [](const Neuron *A, const Neuron *B)
                                         { return A->get_output() < B->get_output(); });
        return iterator - neuralNetwork.back().begin();
    }

    // test
    float
    calculate_error(const std::vector<float> &_label)
    {
        float error = 0;
        for (unsigned index = 0; index < _label.size(); ++index)
        {
            auto err = neuralNetwork.back().at(index)->get_output() - _label.at(index);
            err *= err;
            error += err;
        }
        return error;
    }

public:
    ~Network(void) noexcept
    {
        for (const auto &[ID, neuronPointer] : IDtoPointerMap)
            neuronPointer->~Neuron();
    }

    // graphics
public:
    void draw(sf::RenderWindow &_window)
    {
        // find out how many layers there are which we have to account for
        auto numberOfLayers = neuralNetwork.size();
        float widthOfVerticalDivision = WINDOW_DIMENSION / (numberOfLayers + 1);

        for (unsigned layerIndex = 0; layerIndex < numberOfLayers - 1; ++layerIndex)
        {
            auto &thisLayer = neuralNetwork.at(layerIndex);
            auto &nextLayer = neuralNetwork.at(layerIndex + 1);

            auto thisPosX = widthOfVerticalDivision + widthOfVerticalDivision * layerIndex;
            auto nextPosX = widthOfVerticalDivision + thisPosX;

            // find out how many neurons there are which we have to account for
            auto numNeuronsInThisLayer = thisLayer.size();
            auto numNeuronsInNextLayer = nextLayer.size();

            float thisHorizontalDivisonWidth = WINDOW_DIMENSION / (numNeuronsInThisLayer + 1);
            float nextHorizontalDivisonWidth = WINDOW_DIMENSION / (numNeuronsInNextLayer + 1);

            for (auto &neuronPointer : thisLayer)
            {
                neuronPointer->draw(_window, thisPosX, thisHorizontalDivisonWidth, nextPosX, nextHorizontalDivisonWidth);
            }
        }
        // last layer
        auto &thisLayer = neuralNetwork.back();

        auto thisPosX = widthOfVerticalDivision + widthOfVerticalDivision * (neuralNetwork.size() - 1);

        // find out how many neurons there are which we have to account for
        auto numNeuronsInThisLayer = thisLayer.size();

        float thisHorizontalDivisonWidth = WINDOW_DIMENSION / (numNeuronsInThisLayer + 1);

        for (auto &neuronPointer : thisLayer)
        {
            neuronPointer->draw(_window, thisPosX, thisHorizontalDivisonWidth, 0, 0);
        }
    }
};