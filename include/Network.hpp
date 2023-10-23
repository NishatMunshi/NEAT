#pragma once
#include <vector>
#include "Neuron.hpp"
#include <unordered_map>
#include <map>
#include "Gene.hpp"
#include <cassert>

// TEST
// #include "E:/programming_tools/SFML-2.5.1/include/SFML/Graphics.hpp"
#define WINDOW_DIMENSION 900

struct Network
{
private:
    typedef std::vector<Neuron *> Layer; // we may need to add a new neuron but that can be pushed from the back
    std::vector<Layer> m_neuralNetwork;  // we may need to insert a new layer in the middle, so we use list

    // this unordered map acts as a lookup table
    std::unordered_map<ID, Neuron *> m_IDToPointerMap;

public:
    // construct the net by arranging the layers
    Network(const Genome &_individualGenome)
    {
        // push the number of layers needed (three - input, hidden, output)
        m_neuralNetwork.push_back(Layer()); // input
        m_neuralNetwork.push_back(Layer()); // hidden
        m_neuralNetwork.push_back(Layer()); // output

        for (const auto &neuronGene : _individualGenome.neuronGenome)
        {
            const auto &neuronType = neuronGene.second;

            if (neuronType == SENSOR)
            {
                auto neuronIndex = m_neuralNetwork.front().size();
                auto newNeuronPointer = new Neuron(neuronType, neuronIndex);
                m_neuralNetwork.front().push_back(newNeuronPointer);

                // keep a record of it in the map
                m_IDToPointerMap.insert(std::pair(neuronGene.first, newNeuronPointer));
            }
            else if (neuronType == HIDDEN)
            {

                auto neuronIndex = m_neuralNetwork.at(1).size();
                auto newNeuronPointer = new Neuron(neuronType, neuronIndex);
                m_neuralNetwork.at(1).push_back(newNeuronPointer);

                // keep a record of it in the map
                m_IDToPointerMap.insert(std::pair(neuronGene.first, newNeuronPointer));
            }
            else if (neuronType == MOTOR)
            {
                auto neuronIndex = m_neuralNetwork.back().size();
                auto newNeuronPointer = new Neuron(neuronType, neuronIndex);
                m_neuralNetwork.back().push_back(newNeuronPointer);

                // keep a record of it in the map
                m_IDToPointerMap.insert(std::pair(neuronGene.first, newNeuronPointer));
            }
        }

        // add the new synapses
        for (const auto &synapseGene : _individualGenome.synapseGenome)
        {
            auto startingNeuronPointer = m_IDToPointerMap[synapseGene.first.startingNeuronID];
            auto endingNeuronPointer = m_IDToPointerMap[synapseGene.first.endingNeuronID];
            startingNeuronPointer->add_output_synapse(synapseGene.second, endingNeuronPointer);
        }
    }

    unsigned feed_forward(const std::vector<float> &_inputs)
    {
        assert(_inputs.size() == m_neuralNetwork.front().size());

        // feed the inputs in
        for (unsigned neuronIndex = 0; neuronIndex < m_neuralNetwork.front().size(); ++neuronIndex)
        {
            m_neuralNetwork.front().at(neuronIndex)->set_input(_inputs.at(neuronIndex));
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
                                         { return A->get_output() < B->get_output(); });
        return iterator - m_neuralNetwork.back().begin();
    }

    // test
    float calculate_error(const std::vector<float> &_label)
    {
        float error = 0;
        for (unsigned index = 0; index < _label.size(); ++index)
        {
            auto del = m_neuralNetwork.back().at(index)->get_output() - _label.at(index);
            del *= del;
            error += del;
        }
        return error;
    }

public:
    auto &read_IDToPointerMap(void) const { return m_IDToPointerMap; }
    ~Network(void) noexcept
    {
        for (const auto &[ID, neuronPointer] : m_IDToPointerMap)
            neuronPointer->~Neuron();
    }

    // graphics
// public:
//     void draw(sf::RenderWindow &_window)
//     {
//         // find out how many layers there are which we have to account for
//         auto numberOfLayers = m_neuralNetwork.size();
//         float widthOfVerticalDivision = WINDOW_DIMENSION / (numberOfLayers + 1);

//         for (unsigned layerIndex = 0; layerIndex < numberOfLayers - 1; ++layerIndex)
//         {
//             auto &thisLayer = m_neuralNetwork.at(layerIndex);
//             auto &nextLayer = m_neuralNetwork.at(layerIndex + 1);

//             auto thisPosX = widthOfVerticalDivision + widthOfVerticalDivision * layerIndex;
//             auto nextPosX = widthOfVerticalDivision + thisPosX;

//             // find out how many neurons there are which we have to account for
//             auto numNeuronsInThisLayer = thisLayer.size();
//             auto numNeuronsInNextLayer = nextLayer.size();

//             float thisHorizontalDivisonWidth = WINDOW_DIMENSION / (numNeuronsInThisLayer + 1);
//             float nextHorizontalDivisonWidth = WINDOW_DIMENSION / (numNeuronsInNextLayer + 1);

//             for (auto &neuronPointer : thisLayer)
//             {
//                 neuronPointer->draw(_window, thisPosX, thisHorizontalDivisonWidth, nextPosX, nextHorizontalDivisonWidth);
//             }
//         }
//         // last layer
//         auto &thisLayer = m_neuralNetwork.back();

//         auto thisPosX = widthOfVerticalDivision + widthOfVerticalDivision * (m_neuralNetwork.size() - 1);

//         // find out how many neurons there are which we have to account for
//         auto numNeuronsInThisLayer = thisLayer.size();

//         float thisHorizontalDivisonWidth = WINDOW_DIMENSION / (numNeuronsInThisLayer + 1);

//         for (auto &neuronPointer : thisLayer)
//         {
//             neuronPointer->draw(_window, thisPosX, thisHorizontalDivisonWidth, 0, 0);
//         }
//     }
};