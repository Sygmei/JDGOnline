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


#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static std::map<std::string, ObjectConverter> defaultRendererValues =
            {
                {"borders", Borders{2}},
                {"bordercolor", sf::Color::Black},
                {"textcolor", sf::Color::Black},
                {"selectedtextcolor", sf::Color::White},
                {"backgroundcolor", sf::Color::White},
                {"selectedbackgroundcolor", Color{0, 110, 255}},
                {"distancetoside", 1.f}
            };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Tabs()
    {
        m_type = "Tabs";
        m_callback.widgetType = "Tabs";

        addSignal<sf::String>("TabSelected");

        m_renderer = aurora::makeCopied<TabsRenderer>();
        setRenderer(RendererData::create(defaultRendererValues));

        setTabHeight(30);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Ptr Tabs::create()
    {
        return std::make_shared<Tabs>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Ptr Tabs::copy(Tabs::ConstPtr tabs)
    {
        if (tabs)
            return std::static_pointer_cast<Tabs>(tabs->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setSize(const Layout2d&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Tabs::add(const sf::String& text, bool selectTab)
    {
        // Use the insert function to put the tab in the right place
        insert(m_tabTexts.size(), text, selectTab);

        // Return the index of the new tab
        return m_tabTexts.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::insert(std::size_t index, const sf::String& text, bool selectTab)
    {
        // If the index is too high then just insert at the end
        if (index > m_tabWidth.size())
            index = m_tabWidth.size();

        // Create the new tab
        Text newTab;
        newTab.setFont(m_fontCached);
        newTab.setColor(m_textColorCached);
        newTab.setOpacity(m_opacityCached);
        newTab.setCharacterSize(getTextSize());
        newTab.setString(text);

        // Calculate the width of the tab
        float tabWidth = newTab.getSize().x + (2 * m_distanceToSideCached);
        if ((m_maximumTabWidth > 0) && (m_maximumTabWidth < tabWidth))
            tabWidth = m_maximumTabWidth;

        if (m_tabWidth.empty())
            m_width += tabWidth;
        else
            m_width += tabWidth + ((m_bordersCached.left + m_bordersCached.right) / 2.f);

        m_tabWidth.insert(m_tabWidth.begin() + index, tabWidth);
        m_tabTexts.insert(m_tabTexts.begin() + index, std::move(newTab));

        if (m_selectedTab >= static_cast<int>(index))
            m_selectedTab++;

        // If the tab has to be selected then do so
        if (selectTab)
            select(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tabs::getText(std::size_t index) const
    {
        if (index >= m_tabTexts.size())
            return "";
        else
            return m_tabTexts[index].getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::changeText(std::size_t index, const sf::String& text)
    {
        if (index >= m_tabTexts.size())
            return false;

        // Update the width
        {
            m_tabTexts[index].setString(text);

            float tabWidth = m_tabTexts[index].getSize().x + (2 * m_distanceToSideCached);
            if ((m_maximumTabWidth > 0) && (m_maximumTabWidth < tabWidth))
                tabWidth = m_maximumTabWidth;

            m_width -= m_tabWidth[index];
            m_width += tabWidth;
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::select(const sf::String& text)
    {
        for (unsigned int i = 0; i < m_tabTexts.size(); ++i)
        {
            if (m_tabTexts[i].getString() == text)
            {
                select(i);
                return;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::select(std::size_t index)
    {
        // If the index is too big then do nothing
        if (index > m_tabTexts.size() - 1)
            return;

        // Don't select a tab that is already selected
        if (m_selectedTab == static_cast<int>(index))
            return;

        if (m_selectedTab >= 0)
            m_tabTexts[m_selectedTab].setColor(m_textColorCached);

        // Select the tab
        m_selectedTab = static_cast<int>(index);
        m_tabTexts[m_selectedTab].setColor(m_selectedTextColorCached);

        // Send the callback
        m_callback.text = m_tabTexts[index].getString();
        sendSignal("TabSelected", m_tabTexts[index].getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::deselect()
    {
        if (m_selectedTab >= 0)
            m_tabTexts[m_selectedTab].setColor(m_textColorCached);

        m_selectedTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::remove(const sf::String& text)
    {
        for (unsigned int i = 0; i < m_tabTexts.size(); ++i)
        {
            if (m_tabTexts[i].getString() == text)
            {
                remove(i);
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::remove(std::size_t index)
    {
        // The index can't be too high
        if (index > m_tabTexts.size() - 1)
            return;

        if (m_tabWidth.size() > 1)
            m_width -= m_tabWidth[index] + ((m_bordersCached.left + m_bordersCached.right) / 2.0f);
        else
            m_width -= m_tabWidth[index];

        // Remove the tab
        m_tabTexts.erase(m_tabTexts.begin() + index);
        m_tabWidth.erase(m_tabWidth.begin() + index);

        // Check if the selected tab should change
        if (m_selectedTab == static_cast<int>(index))
            m_selectedTab = -1;
        else if (m_selectedTab > static_cast<int>(index))
            --m_selectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::removeAll()
    {
        m_tabTexts.clear();
        m_tabWidth.clear();
        m_selectedTab = -1;

        m_width = m_bordersCached.left + m_bordersCached.right;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tabs::getSelected() const
    {
        if (m_selectedTab >= 0)
            return m_tabTexts[m_selectedTab].getString();
        else
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Tabs::getSelectedIndex() const
    {
        return m_selectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTextSize(unsigned int size)
    {
        if ((size == 0) || (m_requestedTextSize != size))
        {
            m_requestedTextSize = size;

            if (size == 0)
            {
                m_textSize = Text::findBestTextSize(m_fontCached, (m_tabHeight - m_bordersCached.top - m_bordersCached.bottom) * 0.8f);
            }
            else // An exact size was given
                m_textSize = size;

            for (auto& text : m_tabTexts)
                text.setCharacterSize(m_textSize);

            recalculateTabsWidth();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tabs::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabHeight(float height)
    {
        m_tabHeight = height;

        // Recalculate the size when auto sizing
        if (m_requestedTextSize == 0)
            setTextSize(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Tabs::getTabHeight() const
    {
        return m_tabHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setMaximumTabWidth(float maximumWidth)
    {
        m_maximumTabWidth = maximumWidth;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Tabs::getMaximumTabWidth() const
    {
        return m_maximumTabWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Tabs::getTabsCount() const
    {
        return m_tabTexts.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::mouseOnWidget(sf::Vector2f pos) const
    {
        return sf::FloatRect{0, 0, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::leftMousePressed(sf::Vector2f pos)
    {
        float width = m_bordersCached.left / 2.f;

        // Loop through all tabs
        for (unsigned int i = 0; i < m_tabWidth.size(); ++i)
        {
            // Append the width of the tab
            width += (m_bordersCached.left / 2.f) + m_tabWidth[i] + (m_bordersCached.right / 2.0f);

            // If the mouse went down on this tab then select it
            if (pos.x < width)
            {
                select(i);
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::recalculateTabsWidth()
    {
        // First calculate the width of the tabs as if there aren't any borders
        m_width = 0;
        for (unsigned int i = 0; i < m_tabWidth.size(); ++i)
        {
            m_tabWidth[i] = m_tabTexts[i].getSize().x + (2 * m_distanceToSideCached);
            if ((m_maximumTabWidth > 0) && (m_maximumTabWidth < m_tabWidth[i]))
                m_tabWidth[i] = m_maximumTabWidth;

            m_width += m_tabWidth[i];
        }

        // Now add the borders to the tabs
        if (!m_tabWidth.empty())
            m_width += (m_tabWidth.size() + 1) * ((m_bordersCached.left + m_bordersCached.right) / 2.f);
        else
            m_width = m_bordersCached.left + m_bordersCached.right;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getRenderer()->getBorders();
            updateSize();
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getRenderer()->getTextColor();

            for (auto& tabText : m_tabTexts)
                tabText.setColor(m_textColorCached);

            if (m_selectedTab >= 0)
                m_tabTexts[m_selectedTab].setColor(m_selectedTextColorCached);
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getRenderer()->getSelectedTextColor();

            if (m_selectedTab >= 0)
                m_tabTexts[m_selectedTab].setColor(m_selectedTextColorCached);
        }
        else if (property == "texturetab")
        {
            m_spriteTab.setTexture(getRenderer()->getTextureTab());
        }
        else if (property == "textureselectedtab")
        {
            m_spriteSelectedTab.setTexture(getRenderer()->getTextureSelectedTab());
        }
        else if (property == "distancetoside")
        {
            m_distanceToSideCached = getRenderer()->getDistanceToSide();
            recalculateTabsWidth();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getRenderer()->getBackgroundColor();
        }
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getRenderer()->getBorderColor();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteTab.setOpacity(m_opacityCached);
            m_spriteSelectedTab.setOpacity(m_opacityCached);

            for (auto& tabText : m_tabTexts)
                tabText.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            for (auto& tab : m_tabTexts)
                tab.setFont(m_fontCached);

            // Recalculate the size when auto sizing
            if (m_requestedTextSize == 0)
                setTextSize(0);
            else
                recalculateTabsWidth();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        states.transform.translate({});

        // Draw the borders around the tabs
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.left, m_bordersCached.top});
        }

        float usableHeight = m_tabHeight - m_bordersCached.top - m_bordersCached.bottom;
        for (unsigned int i = 0; i < m_tabTexts.size(); ++i)
        {
            sf::RenderStates textStates = states;

            // Draw the background of the tab
            if (m_selectedTab == static_cast<int>(i))
            {
                if (m_spriteSelectedTab.isSet())
                {
                    Sprite spriteSelectedTab = m_spriteSelectedTab;
                    spriteSelectedTab.setSize({m_tabWidth[i], usableHeight});
                    spriteSelectedTab.draw(target, states);
                }
                else // No texture was loaded
                {
                    drawRectangleShape(target, states, {m_tabWidth[i], usableHeight}, m_selectedBackgroundColorCached);
                }
            }
            else // This tab is not selected
            {
                if (m_spriteTab.isSet())
                {
                    Sprite spriteTab = m_spriteTab;
                    spriteTab.setSize({m_tabWidth[i], usableHeight});
                    spriteTab.draw(target, states);
                }
                else // No texture was loaded
                {
                    drawRectangleShape(target, states, {m_tabWidth[i], usableHeight}, m_backgroundColorCached);
                }
            }

            // Apply clipping if required for the text in this tab
            std::unique_ptr<Clipping> clipping;
            float usableWidth = m_tabWidth[i] - (2 * m_distanceToSideCached);
            if (m_tabTexts[i].getSize().x > usableWidth)
                clipping = std::make_unique<Clipping>(target, textStates, sf::Vector2f{m_distanceToSideCached, 0}, sf::Vector2f{usableWidth, usableHeight});

            // Draw the text
            textStates.transform.translate({m_distanceToSideCached + ((usableWidth - m_tabTexts[i].getSize().x) / 2.f), ((usableHeight - m_tabTexts[i].getSize().y) / 2.f)});
            m_tabTexts[i].draw(target, textStates);

            // Draw the borders between the tabs
            states.transform.translate({m_tabWidth[i], 0});
            if ((m_bordersCached != Borders{0}) && (i < m_tabWidth.size() - 1))
            {
                drawRectangleShape(target, states, {(m_bordersCached.left + m_bordersCached.right) / 2.f, usableHeight}, m_borderColorCached);
                states.transform.translate({(m_bordersCached.left + m_bordersCached.right) / 2.f, 0});
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
