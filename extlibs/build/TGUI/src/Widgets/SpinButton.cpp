/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Widgets/SpinButton.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static std::map<std::string, ObjectConverter> defaultRendererValues =
            {
                {"borders", Borders{2}},
                {"bordercolor", sf::Color::Black},
                {"backgroundcolor", Color{245, 245, 245}},
                {"backgroundcolorhover", sf::Color::White},
                {"arrowcolor", Color{60, 60, 60}},
                {"arrowcolorhover", sf::Color::Black},
                {"spacebetweenarrows", 2.f}
            };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::SpinButton()
    {
        m_callback.widgetType = "SpinButton";
        m_type = "SpinButton";

        addSignal<int>("ValueChanged");

        m_renderer = aurora::makeCopied<SpinButtonRenderer>();
        setRenderer(RendererData::create(defaultRendererValues));

        setSize(20, 42);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::Ptr SpinButton::create(int minimum, int maximum)
    {
        auto spinButton = std::make_shared<SpinButton>();

        spinButton->setMinimum(minimum);
        spinButton->setMaximum(maximum);

        return spinButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::Ptr SpinButton::copy(SpinButton::ConstPtr spinButton)
    {
        if (spinButton)
            return std::static_pointer_cast<SpinButton>(spinButton->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        sf::Vector2f arrowSize = getArrowSize();
        m_spriteArrowUp.setSize(arrowSize);
        m_spriteArrowUpHover.setSize(arrowSize);
        m_spriteArrowDown.setSize(arrowSize);
        m_spriteArrowDownHover.setSize(arrowSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMinimum(int minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_minimum > m_maximum)
            setMaximum(m_minimum);

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            setValue(m_minimum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int SpinButton::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMaximum(int maximum)
    {
        m_maximum = maximum;

        // The maximum can never be below the minimum
        if (m_maximum < m_minimum)
            setMinimum(m_maximum);

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            setValue(m_maximum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int SpinButton::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setValue(int value)
    {
        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_value != value)
        {
            m_value = value;

            m_callback.value = m_value;
            sendSignal("ValueChanged", value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int SpinButton::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setVerticalScroll(bool verticalScroll)
    {
        m_verticalScroll = verticalScroll;

        if (verticalScroll)
        {
            m_spriteArrowUp.setRotation(0);
            m_spriteArrowUpHover.setRotation(0);
            m_spriteArrowDown.setRotation(0);
            m_spriteArrowDownHover.setRotation(0);
        }
        else
        {
            m_spriteArrowUp.setRotation(-90);
            m_spriteArrowUpHover.setRotation(-90);
            m_spriteArrowDown.setRotation(-90);
            m_spriteArrowDownHover.setRotation(-90);
        }

        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinButton::getVerticalScroll() const
    {
        return m_verticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMousePressed(sf::Vector2f pos)
    {
        ClickableWidget::leftMousePressed(pos);

        // Check if the mouse is on top of the upper/right arrow
        if (m_verticalScroll)
        {
            if (sf::FloatRect{0, 0, getSize().x, getSize().y / 2.0f}.contains(pos))
                m_mouseDownOnTopArrow = true;
            else
                m_mouseDownOnTopArrow = false;
        }
        else
        {
            if (sf::FloatRect{0, 0, getSize().x / 2.0f, getSize().y}.contains(pos))
                m_mouseDownOnTopArrow = false;
            else
                m_mouseDownOnTopArrow = true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMouseReleased(sf::Vector2f pos)
    {
        // Check if the mouse went down on the spin button
        if (m_mouseDown)
        {
            // Check if the arrow went down on the top/right arrow
            if (m_mouseDownOnTopArrow)
            {
                // Check if the mouse went up on the same arrow
                if ((m_verticalScroll && (sf::FloatRect{0, 0, getSize().x, getSize().y / 2.f}.contains(pos)))
                 || (!m_verticalScroll && (!sf::FloatRect{0, 0, getSize().x / 2.f, getSize().y}.contains(pos))))
                {
                    // Increment the value
                    if (m_value < m_maximum)
                        setValue(m_value + 1);
                    else
                        return;
                }
                else
                    return;
            }
            else // The mouse went down on the bottom/left arrow
            {
                // Check if the mouse went up on the same arrow
                if ((m_verticalScroll && (!sf::FloatRect{0, 0, getSize().x, getSize().y / 2.f}.contains(pos)))
                 || (!m_verticalScroll && (sf::FloatRect{0, 0, getSize().x / 2.f, getSize().y}.contains(pos))))
                {
                    // Decrement the value
                    if (m_value > m_minimum)
                        setValue(m_value - 1);
                    else
                        return;
                }
                else
                    return;
            }
        }

        ClickableWidget::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::mouseMoved(sf::Vector2f pos)
    {
        // Check if the mouse is on top of the upper/right arrow
        if (m_verticalScroll)
        {
            if (sf::FloatRect{0, 0, getSize().x, getSize().y / 2.0f}.contains(pos))
                m_mouseHoverOnTopArrow = true;
            else
                m_mouseHoverOnTopArrow = false;
        }
        else
        {
            if (sf::FloatRect{0, 0, getSize().x / 2.0f, getSize().y}.contains(pos))
                m_mouseHoverOnTopArrow = true;
            else
                m_mouseHoverOnTopArrow = false;
        }

        if (!m_mouseHover)
            mouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::widgetFocused()
    {
        // A spin button can't be focused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getRenderer()->getBorders();
            updateSize();
        }
        else if (property == "spacebetweenarrows")
        {
            m_spaceBetweenArrowsCached = getRenderer()->getSpaceBetweenArrows();
            updateSize();
        }
        else if (property == "texturearrowup")
        {
            m_spriteArrowUp.setTexture(getRenderer()->getTextureArrowUp());
        }
        else if (property == "texturearrowuphover")
        {
            m_spriteArrowUpHover.setTexture(getRenderer()->getTextureArrowUpHover());
        }
        else if (property == "texturearrowdown")
        {
            m_spriteArrowDown.setTexture(getRenderer()->getTextureArrowDown());
        }
        else if (property == "texturearrowdownhover")
        {
            m_spriteArrowDownHover.setTexture(getRenderer()->getTextureArrowDownHover());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getRenderer()->getBackgroundColorHover();
        }
        else if (property == "arrowcolor")
        {
            m_arrowColorCached = getRenderer()->getArrowColor();
        }
        else if (property == "arrowcolorhover")
        {
            m_arrowColorHoverCached = getRenderer()->getArrowColorHover();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteArrowUp.setOpacity(m_opacityCached);
            m_spriteArrowUpHover.setOpacity(m_opacityCached);
            m_spriteArrowDown.setOpacity(m_opacityCached);
            m_spriteArrowDownHover.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f SpinButton::getArrowSize() const
    {
        if (m_verticalScroll)
            return {getSize().x - m_bordersCached.left - m_bordersCached.right, (getSize().y - m_bordersCached.top - m_bordersCached.bottom - m_spaceBetweenArrowsCached) / 2.0f};
        else
            return {getSize().y - m_bordersCached.top - m_bordersCached.bottom, (getSize().x - m_bordersCached.left - m_bordersCached.right - m_spaceBetweenArrowsCached) / 2.0f};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.left, m_bordersCached.top});
        }

        sf::Vector2f arrowSize = getArrowSize();

        // Draw the top/left arrow
        if (m_spriteArrowUp.isSet() && m_spriteArrowDown.isSet())
        {
            if (m_mouseHover && m_mouseHoverOnTopArrow && m_spriteArrowUpHover.isSet())
                m_spriteArrowUpHover.draw(target, states);
            else
                m_spriteArrowUp.draw(target, states);
        }
        else
        {
            sf::ConvexShape arrow{3};
            sf::RectangleShape arrowBack;

            if (m_verticalScroll)
            {
                arrowBack.setSize(arrowSize);

                arrow.setPoint(0, {arrowBack.getSize().x / 5, arrowBack.getSize().y * 4/5});
                arrow.setPoint(1, {arrowBack.getSize().x / 2, arrowBack.getSize().y / 5});
                arrow.setPoint(2, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y * 4/5});
            }
            else // Spin button lies horizontal
            {
                arrowBack.setSize({arrowSize.y, arrowSize.x});

                arrow.setPoint(0, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y / 5});
                arrow.setPoint(1, {arrowBack.getSize().x / 5, arrowBack.getSize().y / 2});
                arrow.setPoint(2, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y * 4/5});
            }

            if (m_mouseHover && m_mouseHoverOnTopArrow && m_backgroundColorHoverCached.isSet())
                arrowBack.setFillColor(Color::calcColorOpacity(m_backgroundColorHoverCached, m_opacityCached));
            else
                arrowBack.setFillColor(Color::calcColorOpacity(m_backgroundColorCached, m_opacityCached));

            if (m_mouseHover && m_mouseHoverOnTopArrow && m_arrowColorHoverCached.isSet())
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorHoverCached, m_opacityCached));
            else
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorCached, m_opacityCached));

            target.draw(arrowBack, states);
            target.draw(arrow, states);
        }

        // Draw the space between the arrows (if there is space)
        if (m_verticalScroll)
        {
            states.transform.translate({0, arrowSize.y});

            if (m_spaceBetweenArrowsCached > 0)
            {
                drawRectangleShape(target, states, {arrowSize.x, m_spaceBetweenArrowsCached}, m_borderColorCached);
                states.transform.translate({0, m_spaceBetweenArrowsCached});
            }
        }
        else // Horizontal orientation
        {
            states.transform.translate({arrowSize.y, 0});

            if (m_spaceBetweenArrowsCached > 0)
            {
                drawRectangleShape(target, states, {m_spaceBetweenArrowsCached, arrowSize.x}, m_borderColorCached);
                states.transform.translate({m_spaceBetweenArrowsCached, 0});
            }
        }

        // Draw the bottom/right arrow
        if (m_spriteArrowUp.isSet() && m_spriteArrowDown.isSet())
        {
            if (m_mouseHover && !m_mouseHoverOnTopArrow && m_spriteArrowDownHover.isSet())
                m_spriteArrowDownHover.draw(target, states);
            else
                m_spriteArrowDown.draw(target, states);
        }
        else // There are no images
        {
            sf::ConvexShape arrow{3};
            sf::RectangleShape arrowBack;

            if (m_verticalScroll)
            {
                arrowBack.setSize(arrowSize);

                arrow.setPoint(0, {arrowBack.getSize().x / 5, arrowBack.getSize().y / 5});
                arrow.setPoint(1, {arrowBack.getSize().x / 2, arrowBack.getSize().y * 4/5});
                arrow.setPoint(2, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y / 5});
            }
            else // Spin button lies horizontal
            {
                arrowBack.setSize({arrowSize.y, arrowSize.x});

                arrow.setPoint(0, {arrowBack.getSize().x / 5, arrowBack.getSize().y / 5});
                arrow.setPoint(1, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y / 2});
                arrow.setPoint(2, {arrowBack.getSize().x / 5, arrowBack.getSize().y * 4/5});
            }

            if (m_mouseHover && !m_mouseHoverOnTopArrow && m_backgroundColorHoverCached.isSet())
                arrowBack.setFillColor(Color::calcColorOpacity(m_backgroundColorHoverCached, m_opacityCached));
            else
                arrowBack.setFillColor(Color::calcColorOpacity(m_backgroundColorCached, m_opacityCached));

            if (m_mouseHover && !m_mouseHoverOnTopArrow && m_arrowColorHoverCached.isSet())
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorHoverCached, m_opacityCached));
            else
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorCached, m_opacityCached));

            target.draw(arrowBack, states);
            target.draw(arrow, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
