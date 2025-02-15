#include "DebugView.h"

sgl::DebugView::DebugView(sf::Vector2f size, float scrollSpeed) : view({0,0}, size), screenSize(size), dragging(false), scrollSpeed(scrollSpeed)
{

}

void sgl::DebugView::ProcessEvent(const sf::Event &event)
{
    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        if(dragging){
            view.setCenter(prevPos - transform(sf::Vector2f(mouseMoved->position)));
        }
    }
    else if (const auto* mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseButtonPressed->button == sf::Mouse::Button::Left)
        {
            dragging = true;
            prevPos = view.getCenter() + transform(sf::Vector2f(mouseButtonPressed->position));
        }
    }
    else if (const auto* mouseButtonReleased = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (mouseButtonReleased->button == sf::Mouse::Button::Left)
        {
            dragging = false;
        }
    }
    else if (const auto* mouseButtonReleased = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        view.zoom(1.f - mouseButtonReleased->delta * scrollSpeed);
    }
    else if (const auto* resized = event.getIf<sf::Event::Resized>())
    {
        screenSize = sf::Vector2f(resized->size);
    }
}

void sgl::DebugView::ResetView()
{
    view.setCenter({0,0});
    view.setSize(screenSize);
}

const sf::View& sgl::DebugView::getView()
{
    return view;
}

sf::Vector2f sgl::DebugView::transform(sf::Vector2f vector)
{
    sf::Transform viewTransform = view.getInverseTransform();
    return viewTransform.transformPoint(vector).componentWiseDiv(screenSize * 0.5f).componentWiseMul({1.f, -1.f});
}
