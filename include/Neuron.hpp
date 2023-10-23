#pragma once
#include "Gene.hpp"
#include <vector>
#include <cmath>

// debug
// #include <iostream>

// GRAPHICS
// #include "E:/programming_tools/SFML-2.5.1/include/SFML/Graphics.hpp"

class Neuron
{
protected:
    struct Synapse
    {
        Neuron *endingNeuronPointer;
        float weight;
        bool enabled;

        Synapse(const SynapseProperties &_properties, Neuron *_endingNeuronPointer) : endingNeuronPointer(_endingNeuronPointer), weight(_properties.weight), enabled(_properties.enabled) {}
    };

protected:
    float m_input;
    float m_output;
    std::vector<Synapse *> m_outputSynapsePointers;

protected:
    static inline float activation_function(const float _x) { return tanh(_x); }

public:
    NeuronType type;

    // remember to check later if sorting is necessary, if not neuronIndex is not needed here
    unsigned neuronIndex; // neuronIndex is needed for maintaining order in input and output layer

public:
    Neuron(const NeuronType _type, const unsigned _neuronIndex) : type(_type), neuronIndex(_neuronIndex)
    {
        m_input = 0.f;
        m_output = 1.f;
    }
    inline void add_output_synapse(const SynapseProperties &_properties, Neuron *_endingNeuronPointer)
    {
        m_outputSynapsePointers.push_back(new Synapse(_properties, _endingNeuronPointer));
    }

    void feed_forward(void)
    {
        m_output = activation_function(m_input);
        m_input = 0.f;

        for (auto &synapsePointer : m_outputSynapsePointers)
        {
            if (synapsePointer->enabled)
                synapsePointer->endingNeuronPointer->m_input += synapsePointer->weight * this->m_output;
        }
    }
    inline void set_input(const float _input) { m_input = _input; };
    inline float get_output(void) const { return m_output; }

public:
    ~Neuron(void) noexcept
    {
        for (const auto &synapsePointer : m_outputSynapsePointers)
            synapsePointer->~Synapse();
    }

    // graphics
// public:
//     void draw(sf::RenderWindow &_window, const float _thisPosX, const float _thisHorizontalDivisonWidth, const float _nextPosX, const float _nextHorizontalDivisonWidth)
//     {

//         // set the position on screen
//         auto thisPosY = _thisHorizontalDivisonWidth + _thisHorizontalDivisonWidth * neuronIndex;

//         for (auto &synapsePointer : m_outputSynapsePointers)
//         {
//             auto nextPosY = _nextHorizontalDivisonWidth + _nextHorizontalDivisonWidth * synapsePointer->endingNeuronPointer->neuronIndex;

//             sf::Vertex line[2];
//             line[0] = sf::Vertex(sf::Vector2f(_thisPosX, thisPosY) + sf::Vector2f(9.f, 9.f));
//             line[1] = sf::Vertex(sf::Vector2f(_nextPosX, nextPosY) + sf::Vector2f(9.f, 9.f));

//             sf::Color color;
//             if (synapsePointer->weight < 0)
//                 color = sf::Color(173, 216, 230, (m_output + 1) * 127);
//             else
//                 color = sf::Color(255, 255, 237, (m_output + 1) * 127);

//             // line[0].color = color;
//             line[1].color = color;

//             _window.draw(line, 2, sf::Lines);
//         }

//         sf::CircleShape circle;
//         circle.setRadius(10.f);
//         circle.setFillColor(sf::Color::White);
//         circle.setOutlineThickness(4.f);

//         // set the color
//         auto neuronOutput = m_output;
//         sf::Color color;
//         if (neuronOutput > 0)
//         {
//             color = sf::Color::Green;
//         }
//         else
//         {
//             color = sf::Color::Red;
//         }
//         circle.setOutlineColor(color);

//         circle.setPosition({_thisPosX, thisPosY});
//         _window.draw(circle);
    // }
};
