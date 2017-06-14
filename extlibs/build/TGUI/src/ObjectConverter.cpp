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


#include <TGUI/ObjectConverter.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Loading/Deserializer.hpp>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ObjectConverter::getString()
    {
        assert(m_type != Type::None);

        if (m_serialized)
            return m_string;

        m_string = Serializer::serialize(ObjectConverter{*this});
        m_serialized = true;
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Font& ObjectConverter::getFont()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Font || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Font(m_string);
            m_type = Type::Font;
        }

        return m_value.as<Font>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& ObjectConverter::getColor()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Color || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Color(m_string);
            m_type = Type::Color;
        }

        return m_value.as<Color>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ObjectConverter::getNumber()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Number || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Number, m_string).getNumber();
            m_type = Type::Number;
        }

        return m_value.as<float>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Outline& ObjectConverter::getOutline()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Outline || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Outline, m_string).getOutline();
            m_type = Type::Outline;
        }

        return m_value.as<Outline>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& ObjectConverter::getTexture()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Texture || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Texture, m_string).getTexture();
            m_type = Type::Texture;
        }

        return m_value.as<Texture>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TextStyle& ObjectConverter::getTextStyle()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::TextStyle || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::TextStyle, m_string).getTextStyle();
            m_type = Type::TextStyle;
        }

        return m_value.as<TextStyle>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::shared_ptr<RendererData>& ObjectConverter::getRenderer()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::RendererData || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::RendererData, m_string).getRenderer();
            m_type = Type::RendererData;
        }

        return m_value.as<std::shared_ptr<RendererData>>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter::Type ObjectConverter::getType() const
    {
        return m_type;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
