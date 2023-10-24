#include "include/NEAT.hpp"
#include <iostream>
// #include "E:/programming_tools/SFML-2.5.1/include/SFML/Graphics.hpp"

int main()
{

    // init the window
    // sf::RenderWindow window(sf::VideoMode(900, 900), "NEAT");
    // window.setFramerateLimit(60);

    const unsigned population = 100;

    NEAT neat(2, 2, population);

    // while (window.isOpen())
    {
        // event check
        // sf::Event _event;
        // while (window.pollEvent(_event))
        // {
        //     if (_event.type == sf::Event::Closed)
        //         window.close();
        // }
     unsigned generation = 0u;
        while(true)
        {
            neat.play_generation();

            std::cout << "\n\ngeneration " << generation++ << ' ';

            // window.clear();
            neat.print_best_scorer();
            // window.display();

            neat.repopulate();
        }

        // std::getchar();
    }
    return 0;
}