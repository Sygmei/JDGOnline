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


#include <TGUI/ToolTip.hpp>
#include <TGUI/Loading/WidgetSaver.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Knob.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/SpinButton.hpp>
#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/to_string.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        #define SET_PROPERTY(property, value) node->propertyValuePairs[property] = std::make_shared<DataIO::ValueNode>(value)

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string emitLayout(Layout2d layout)
        {
            std::string str;
            str += "(";

            if (layout.x.getImpl()->operation == LayoutImpl::Operation::String)
                str += "\"" + layout.x.getImpl()->stringExpression + "\"";
            else
                str += to_string(layout.x.getValue());

            str += ", ";

            if (layout.y.getImpl()->operation == LayoutImpl::Operation::String)
                str += "\"" + layout.y.getImpl()->stringExpression + "\"";
            else
                str += to_string(layout.y.getValue());

            str += ")";
            return str;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveWidget(Widget::Ptr widget)
        {
            sf::String widgetName;
            if (widget->getParent())
                widgetName = widget->getParent()->getWidgetName(widget);

            auto node = std::make_shared<DataIO::Node>();
            if (widgetName.isEmpty())
                node->name = widget->getWidgetType();
            else
                node->name = widget->getWidgetType() + "." + Serializer::serialize(widgetName);

            if (!widget->isVisible())
                SET_PROPERTY("Visible", "false");
            if (!widget->isEnabled())
                SET_PROPERTY("Enabled", "false");
            if (widget->getPosition() != sf::Vector2f{})
                SET_PROPERTY("Position", emitLayout(widget->getPositionLayout()));
            if (widget->getSize() != sf::Vector2f{})
            {
                /// TODO: Fix Grid and Tab to no longer override the getSize function
                if (widget->getSize() != widget->getSizeLayout().getValue())
                    SET_PROPERTY("Size", emitLayout({widget->getSize()}));
                else
                    SET_PROPERTY("Size", emitLayout(widget->getSizeLayout()));
            }

            if (widget->getToolTip() != nullptr)
            {
                auto toolTipWidgetNode = WidgetSaver::getSaveFunction("widget")(widget->getToolTip());

                auto toolTipNode = std::make_shared<DataIO::Node>();
                toolTipNode->name = "ToolTip";
                toolTipNode->children.emplace_back(toolTipWidgetNode);

                toolTipNode->propertyValuePairs["TimeToDisplay"] = std::make_shared<DataIO::ValueNode>(to_string(ToolTip::getTimeToDisplay().asSeconds()));
                toolTipNode->propertyValuePairs["DistanceToMouse"] = std::make_shared<DataIO::ValueNode>("(" + to_string(ToolTip::getDistanceToMouse().x) + "," + to_string(ToolTip::getDistanceToMouse().y) + ")");

                node->children.emplace_back(toolTipNode);
            }

            /// TODO: Separate renderer section?
            if (!widget->getRenderer()->getPropertyValuePairs().empty())
            {
                node->children.emplace_back(std::make_shared<DataIO::Node>());
                node->children.back()->name = "Renderer";
                for (const auto& pair : widget->getRenderer()->getPropertyValuePairs())
                {
                    sf::String value = ObjectConverter{pair.second}.getString();
                    if (pair.second.getType() == ObjectConverter::Type::RendererData)
                    {
                        std::stringstream ss{value};
                        auto rendererRootNode = DataIO::parse(ss);
                        if (!rendererRootNode->children.empty())
                            node->children.back()->children.push_back(rendererRootNode->children[0]);
                        else
                            node->children.back()->children.push_back(rendererRootNode);

                        node->children.back()->children.back()->name = pair.first;
                    }
                    else
                        node->children.back()->propertyValuePairs[pair.first] = std::make_shared<DataIO::ValueNode>(value);
                }
            }

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveContainer(Widget::Ptr widget)
        {
            auto container = std::static_pointer_cast<Container>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(container);

            for (const auto& child : container->getWidgets())
            {
                auto& saveFunction = WidgetSaver::getSaveFunction(toLower(child->getWidgetType()));
                if (saveFunction)
                    node->children.emplace_back(saveFunction(child));
                else
                    throw Exception{"No save function exists for widget type '" + child->getWidgetType() + "'."};
            }

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveButton(Widget::Ptr widget)
        {
            auto button = std::static_pointer_cast<Button>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(button);

            if (!button->getText().isEmpty())
                SET_PROPERTY("Text", Serializer::serialize(button->getText()));

            SET_PROPERTY("TextSize", to_string(button->getTextSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveChatBox(Widget::Ptr widget)
        {
            auto chatBox = std::static_pointer_cast<ChatBox>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(chatBox);

            SET_PROPERTY("TextSize", to_string(chatBox->getTextSize()));
            SET_PROPERTY("TextColor", Serializer::serialize(chatBox->getTextColor()));

            if (chatBox->getLineLimit())
                SET_PROPERTY("LineLimit", to_string(chatBox->getLineLimit()));

            if (chatBox->getLinesStartFromTop())
                SET_PROPERTY("LinesStartFromTop", "true");
            else
                SET_PROPERTY("LinesStartFromTop", "false");

            if (chatBox->getNewLinesBelowOthers())
                SET_PROPERTY("NewLinesBelowOthers", "true");
            else
                SET_PROPERTY("NewLinesBelowOthers", "false");

            for (std::size_t i = 0; i < chatBox->getLineAmount(); ++i)
            {
                unsigned int lineTextSize = chatBox->getLineTextSize(i);
                sf::Color lineTextColor = chatBox->getLineColor(i);

                auto lineNode = std::make_shared<DataIO::Node>();
                lineNode->parent = node.get();
                lineNode->name = "Line";

                lineNode->propertyValuePairs["Text"] = std::make_shared<DataIO::ValueNode>(Serializer::serialize(chatBox->getLine(i)));
                if (lineTextSize != chatBox->getTextSize())
                    lineNode->propertyValuePairs["TextSize"] = std::make_shared<DataIO::ValueNode>(to_string(lineTextSize));
                if (lineTextColor != chatBox->getTextColor())
                    lineNode->propertyValuePairs["Color"] = std::make_shared<DataIO::ValueNode>(Serializer::serialize(lineTextColor));

                node->children.push_back(lineNode);
            }

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveChildWindow(Widget::Ptr widget)
        {
            auto childWindow = std::static_pointer_cast<ChildWindow>(widget);
            auto node = WidgetSaver::getSaveFunction("container")(childWindow);

            if (childWindow->getTitleAlignment() == ChildWindow::TitleAlignment::Left)
                SET_PROPERTY("TitleAlignment", "Left");
            else if (childWindow->getTitleAlignment() == ChildWindow::TitleAlignment::Center)
                SET_PROPERTY("TitleAlignment", "Center");
            else if (childWindow->getTitleAlignment() == ChildWindow::TitleAlignment::Right)
                SET_PROPERTY("TitleAlignment", "Right");

            if (childWindow->getTitle().getSize() > 0)
                SET_PROPERTY("Title", Serializer::serialize(childWindow->getTitle()));

            if (childWindow->isKeptInParent())
                SET_PROPERTY("KeepInParent", "true");

            if (childWindow->isResizable())
                SET_PROPERTY("Resizable", "true");

            std::string serializedTitleButtons;
            if (childWindow->getTitleButtons() & ChildWindow::TitleButton::Minimize)
                serializedTitleButtons += " | Minimize";
            if (childWindow->getTitleButtons() & ChildWindow::TitleButton::Maximize)
                serializedTitleButtons += " | Maximize";
            if (childWindow->getTitleButtons() & ChildWindow::TitleButton::Close)
                serializedTitleButtons += " | Close";

            if (!serializedTitleButtons.empty())
                serializedTitleButtons.erase(0, 3);
            else
                serializedTitleButtons = "None";

            SET_PROPERTY("TitleButtons", serializedTitleButtons);

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveComboBox(Widget::Ptr widget)
        {
            auto comboBox = std::static_pointer_cast<ComboBox>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(comboBox);

            if (comboBox->getItemCount() > 0)
            {
                auto items = comboBox->getItems();
                auto& ids = comboBox->getItemIds();

                std::string itemList = "[" + Serializer::serialize(items[0]);
                std::string itemIdList = "[" + Serializer::serialize(ids[0]);
                for (std::size_t i = 1; i < items.size(); ++i)
                {
                    itemList += ", " + Serializer::serialize(items[i]);
                    itemIdList += ", " + Serializer::serialize(ids[i]);
                }
                itemList += "]";
                itemIdList += "]";

                SET_PROPERTY("Items", itemList);
                SET_PROPERTY("ItemIds", itemIdList);
            }

            SET_PROPERTY("ItemsToDisplay", to_string(comboBox->getItemsToDisplay()));
            SET_PROPERTY("TextSize", to_string(comboBox->getTextSize()));
            SET_PROPERTY("MaximumItems", to_string(comboBox->getMaximumItems()));

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveEditBox(Widget::Ptr widget)
        {
            auto editBox = std::static_pointer_cast<EditBox>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(editBox);

            if (editBox->getAlignment() != EditBox::Alignment::Left)
            {
                if (editBox->getAlignment() == EditBox::Alignment::Center)
                    SET_PROPERTY("Alignment", "Center");
                else
                    SET_PROPERTY("Alignment", "Right");
            }

            if (editBox->getInputValidator() != ".*")
            {
                if (editBox->getInputValidator() == EditBox::Validator::Int)
                    SET_PROPERTY("InputValidator", "Int");
                else if (editBox->getInputValidator() == EditBox::Validator::UInt)
                    SET_PROPERTY("InputValidator", "UInt");
                else if (editBox->getInputValidator() == EditBox::Validator::Float)
                    SET_PROPERTY("InputValidator", "Float");
                else
                    SET_PROPERTY("InputValidator", Serializer::serialize(sf::String{editBox->getInputValidator()}));
            }

            if (!editBox->getText().isEmpty())
                SET_PROPERTY("Text", Serializer::serialize(editBox->getText()));
            if (!editBox->getDefaultText().isEmpty())
                SET_PROPERTY("DefaultText", Serializer::serialize(editBox->getDefaultText()));
            if (editBox->getPasswordCharacter() != '\0')
                SET_PROPERTY("PasswordCharacter", Serializer::serialize(sf::String(editBox->getPasswordCharacter())));
            if (editBox->getMaximumCharacters() != 0)
                SET_PROPERTY("MaximumCharacters", to_string(editBox->getMaximumCharacters()));
            if (editBox->isTextWidthLimited())
                SET_PROPERTY("TextWidthLimited", "true");

            SET_PROPERTY("TextSize", to_string(editBox->getTextSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveKnob(Widget::Ptr widget)
        {
            auto knob = std::static_pointer_cast<Knob>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(knob);

            if (knob->getClockwiseTurning())
                SET_PROPERTY("ClockwiseTurning", "true");
            else
                SET_PROPERTY("ClockwiseTurning", "false");

            SET_PROPERTY("StartRotation", to_string(knob->getStartRotation()));
            SET_PROPERTY("EndRotation", to_string(knob->getEndRotation()));
            SET_PROPERTY("Minimum", to_string(knob->getMinimum()));
            SET_PROPERTY("Maximum", to_string(knob->getMaximum()));
            SET_PROPERTY("Value", to_string(knob->getValue()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveLabel(Widget::Ptr widget)
        {
            auto label = std::static_pointer_cast<Label>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(label);

            if (label->getHorizontalAlignment() == Label::HorizontalAlignment::Center)
                SET_PROPERTY("HorizontalAlignment", "Center");
            else if (label->getHorizontalAlignment() == Label::HorizontalAlignment::Right)
                SET_PROPERTY("HorizontalAlignment", "Right");

            if (label->getVerticalAlignment() == Label::VerticalAlignment::Center)
                SET_PROPERTY("VerticalAlignment", "Center");
            else if (label->getVerticalAlignment() == Label::VerticalAlignment::Bottom)
                SET_PROPERTY("VerticalAlignment", "Bottom");

            if (!label->getText().isEmpty())
                SET_PROPERTY("Text", Serializer::serialize(label->getText()));
            if (label->getMaximumTextWidth() > 0)
                SET_PROPERTY("MaximumTextWidth", to_string(label->getMaximumTextWidth()));
            if (label->getAutoSize())
                SET_PROPERTY("AutoSize", "true");

            SET_PROPERTY("TextSize", to_string(label->getTextSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveListBox(Widget::Ptr widget)
        {
            auto listBox = std::static_pointer_cast<ListBox>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(listBox);

            if (listBox->getItemCount() > 0)
            {
                auto items = listBox->getItems();
                auto& ids = listBox->getItemIds();

                std::string itemList = "[" + Serializer::serialize(items[0]);
                std::string itemIdList = "[" + Serializer::serialize(ids[0]);
                for (std::size_t i = 1; i < items.size(); ++i)
                {
                    itemList += ", " + Serializer::serialize(items[i]);
                    itemIdList += ", " + Serializer::serialize(ids[i]);
                }
                itemList += "]";
                itemIdList += "]";

                SET_PROPERTY("Items", itemList);
                SET_PROPERTY("ItemIds", itemIdList);
            }

            if (!listBox->getAutoScroll())
                SET_PROPERTY("AutoScroll", "false");

            SET_PROPERTY("TextSize", to_string(listBox->getTextSize()));
            SET_PROPERTY("ItemHeight", to_string(listBox->getItemHeight()));
            SET_PROPERTY("MaximumItems", to_string(listBox->getMaximumItems()));

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveMenuBar(Widget::Ptr widget)
        {
            auto menuBar = std::static_pointer_cast<MenuBar>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(menuBar);

            std::map<sf::String, std::vector<sf::String>> menus = menuBar->getMenus();
            for (const auto& menu : menus)
            {
                auto menuNode = std::make_shared<DataIO::Node>();
                menuNode->parent = node.get();
                menuNode->name = "Menu";

                menuNode->propertyValuePairs["Name"] = std::make_shared<DataIO::ValueNode>(Serializer::serialize(menu.first));

                auto& items = menu.second;
                if (!items.empty())
                {
                    std::string itemList = "[" + Serializer::serialize(items[0]);
                    for (std::size_t i = 1; i < items.size(); ++i)
                        itemList += ", " + Serializer::serialize(items[i]);
                    itemList += "]";

                    menuNode->propertyValuePairs["Items"] = std::make_shared<DataIO::ValueNode>(itemList);
                }

                node->children.push_back(menuNode);
            }

            SET_PROPERTY("TextSize", to_string(menuBar->getTextSize()));
            SET_PROPERTY("MinimumSubMenuWidth", to_string(menuBar->getMinimumSubMenuWidth()));

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveMessageBox(Widget::Ptr widget)
        {
            auto messageBox = std::static_pointer_cast<MessageBox>(widget);
            auto node = WidgetSaver::getSaveFunction("childwindow")(messageBox);

            SET_PROPERTY("TextSize", to_string(messageBox->getTextSize()));

            // Label and buttons are saved indirectly by saving the child window

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> savePicture(Widget::Ptr widget)
        {
            auto picture = std::static_pointer_cast<Picture>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(picture);

            if (!picture->getLoadedFilename().isEmpty())
                SET_PROPERTY("Filename", Serializer::serialize(sf::String{picture->getLoadedFilename()}));
            if (picture->isSmooth())
                SET_PROPERTY("Smooth", "true");

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveProgressBar(Widget::Ptr widget)
        {
            auto progressBar = std::static_pointer_cast<ProgressBar>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(progressBar);

            if (!progressBar->getText().isEmpty())
                SET_PROPERTY("Text", Serializer::serialize(progressBar->getText()));

            if (progressBar->getFillDirection() != ProgressBar::FillDirection::LeftToRight)
            {
                if (progressBar->getFillDirection() == ProgressBar::FillDirection::RightToLeft)
                    SET_PROPERTY("FillDirection", "RightToLeft");
                else if (progressBar->getFillDirection() == ProgressBar::FillDirection::TopToBottom)
                    SET_PROPERTY("FillDirection", "TopToBottom");
                else if (progressBar->getFillDirection() == ProgressBar::FillDirection::BottomToTop)
                    SET_PROPERTY("FillDirection", "BottomToTop");
            }

            SET_PROPERTY("Minimum", to_string(progressBar->getMinimum()));
            SET_PROPERTY("Maximum", to_string(progressBar->getMaximum()));
            SET_PROPERTY("Value", to_string(progressBar->getValue()));
            SET_PROPERTY("TextSize", to_string(progressBar->getTextSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveRadioButton(Widget::Ptr widget)
        {
            auto radioButton = std::static_pointer_cast<RadioButton>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(radioButton);

            if (!radioButton->getText().isEmpty())
                SET_PROPERTY("Text", Serializer::serialize(radioButton->getText()));
            if (radioButton->isChecked())
                SET_PROPERTY("Checked", "true");
            if (!radioButton->isTextClickable())
                SET_PROPERTY("TextClickable", "false");

            SET_PROPERTY("TextSize", to_string(radioButton->getTextSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveScrollbar(Widget::Ptr widget)
        {
            auto scrollbar = std::static_pointer_cast<Scrollbar>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(scrollbar);

            if (scrollbar->getAutoHide())
                SET_PROPERTY("AutoHide", "true");
            else
                SET_PROPERTY("AutoHide", "false");

            SET_PROPERTY("LowValue", to_string(scrollbar->getLowValue()));
            SET_PROPERTY("Maximum", to_string(scrollbar->getMaximum()));
            SET_PROPERTY("Value", to_string(scrollbar->getValue()));
            SET_PROPERTY("ScrollAmount", to_string(scrollbar->getScrollAmount()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveSlider(Widget::Ptr widget)
        {
            auto slider = std::static_pointer_cast<Slider>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(slider);

            SET_PROPERTY("Minimum", to_string(slider->getMinimum()));
            SET_PROPERTY("Maximum", to_string(slider->getMaximum()));
            SET_PROPERTY("Value", to_string(slider->getValue()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveSpinButton(Widget::Ptr widget)
        {
            auto spinButton = std::static_pointer_cast<SpinButton>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(spinButton);

            if (spinButton->getVerticalScroll())
                SET_PROPERTY("VerticalScroll", "true");
            else
                SET_PROPERTY("VerticalScroll", "false");

            SET_PROPERTY("Minimum", to_string(spinButton->getMinimum()));
            SET_PROPERTY("Maximum", to_string(spinButton->getMaximum()));
            SET_PROPERTY("Value", to_string(spinButton->getValue()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveTabs(Widget::Ptr widget)
        {
            auto tabs = std::static_pointer_cast<Tabs>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(tabs);

            if (tabs->getTabsCount() > 0)
            {
                std::string tabList = "[" + Serializer::serialize(tabs->getText(0));
                for (std::size_t i = 1; i < tabs->getTabsCount(); ++i)
                    tabList += ", " + Serializer::serialize(tabs->getText(i));

                tabList += "]";
                SET_PROPERTY("Tabs", tabList);
            }

            if (tabs->getSelectedIndex() >= 0)
                SET_PROPERTY("Selected", to_string(tabs->getSelectedIndex()));

            if (tabs->getMaximumTabWidth() > 0)
                SET_PROPERTY("MaximumTabWidth", to_string(tabs->getMaximumTabWidth()));

            SET_PROPERTY("TextSize", to_string(tabs->getTextSize()));
            SET_PROPERTY("TabHeight", to_string(tabs->getTabHeight()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<DataIO::Node> saveTextBox(Widget::Ptr widget)
        {
            auto textBox = std::static_pointer_cast<TextBox>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(textBox);

            SET_PROPERTY("Text", Serializer::serialize(textBox->getText()));
            SET_PROPERTY("TextSize", to_string(textBox->getTextSize()));
            SET_PROPERTY("MaximumCharacters", to_string(textBox->getMaximumCharacters()));

            if (textBox->isReadOnly())
                SET_PROPERTY("ReadOnly", "true");

            if (!textBox->isVerticalScrollbarPresent())
                SET_PROPERTY("VerticalScrollbarPresent", "false");

            return node;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, WidgetSaver::SaveFunction> WidgetSaver::m_saveFunctions =
        {
            {"widget", saveWidget},
            {"container", saveContainer},
            {"button", saveButton},
            {"canvas", saveWidget},
            {"chatbox", saveChatBox},
            {"checkbox", saveRadioButton},
            {"childwindow", saveChildWindow},
            {"clickablewidget", saveWidget},
            {"combobox", saveComboBox},
            {"editbox", saveEditBox},
            {"group", saveContainer},
            {"knob", saveKnob},
            {"label", saveLabel},
            {"listbox", saveListBox},
            {"menubar", saveMenuBar},
            {"messagebox", saveMessageBox},
            {"panel", saveContainer},
            {"picture", savePicture},
            {"progressbar", saveProgressBar},
            {"radiobutton", saveRadioButton},
            {"radiobuttongroup", saveContainer},
            {"scrollbar", saveScrollbar},
            {"slider", saveSlider},
            {"spinbutton", saveSpinButton},
            {"tabs", saveTabs},
            {"textbox", saveTextBox}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetSaver::save(Container::Ptr widget, std::stringstream& stream)
    {
        auto node = std::make_shared<DataIO::Node>();
        for (const auto& child : widget->getWidgets())
        {
            auto& saveFunction = WidgetSaver::getSaveFunction(toLower(child->getWidgetType()));
            if (saveFunction)
                node->children.emplace_back(saveFunction(child));
            else
                throw Exception{"No save function exists for widget type '" + child->getWidgetType() + "'."};
        }

        DataIO::emit(node, stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetSaver::setSaveFunction(const std::string& type, const SaveFunction& saveFunction)
    {
        m_saveFunctions[toLower(type)] = saveFunction;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const WidgetSaver::SaveFunction& WidgetSaver::getSaveFunction(const std::string& type)
    {
        return m_saveFunctions[toLower(type)];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
