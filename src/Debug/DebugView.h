#include <SFML/Graphics.hpp>

namespace sgl {

    class DebugView {
    private:
        sf::View view;
        sf::Vector2f prevPos;
        sf::Vector2f screenSize;
        bool dragging;
        float scrollSpeed;
        
    public:
        DebugView(sf::Vector2f size, float scrollSpeed);
        void ProcessEvent(const sf::Event& event);
        void ResetView();
        const sf::View& getView();
    
    private:
        sf::Vector2f transform(sf::Vector2f vector);
    };

}