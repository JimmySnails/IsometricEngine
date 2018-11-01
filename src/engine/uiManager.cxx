#include "uiManager.hxx"

#include "textureManager.hxx"
#include "engine.hxx"

#include "basics/resources.hxx"

#include "../ThirdParty/json.hxx"

void UIManager::init()
{
  nlohmann::json uiLayout = Resources::getUILayoutJSONObject();

  for (const auto &it : uiLayout.items())
  {
    std::string groupID;
    groupID = it.key();

    bool visible = false;

    for (size_t id = 0; id < uiLayout[it.key()].size(); id++)
    {

      if (!uiLayout[it.key()][id]["groupVisibility"].is_null())
      {
        visible = uiLayout[it.key()][id]["groupVisibility"].get<bool>();
      }

      if (!uiLayout[it.key()][id]["Type"].is_null())
      {
        bool toggleButton = false;
        std::string actionID;
        std::string parentOf;
        std::string tooltipText;
        std::string text;

        SDL_Rect elementRect = {0, 0, 0, 0};
        int spriteID = -1;

        // Each element must have x and y values
        elementRect.x = uiLayout[it.key()][id]["Position_x"].get<int>();
        elementRect.y = uiLayout[it.key()][id]["Position_y"].get<int>();

        if (!uiLayout[it.key()][id]["Width"].is_null())
        {
          elementRect.w = uiLayout[it.key()][id]["Width"].get<int>();
        }
        if (!uiLayout[it.key()][id]["Height"].is_null())
        {
          elementRect.h = uiLayout[it.key()][id]["Height"].get<int>();
        }
        if (!uiLayout[it.key()][id]["SpriteID"].is_null())
        {
          spriteID = uiLayout[it.key()][id]["SpriteID"].get<int>();
        }
        if (!uiLayout[it.key()][id]["TooltipText"].is_null())
        {
          tooltipText = uiLayout[it.key()][id]["TooltipText"].get<std::string>();
        }
        if (!uiLayout[it.key()][id]["ToggleButton"].is_null())
        {
          toggleButton = uiLayout[it.key()][id]["ToggleButton"].get<bool>();
        }
        if (!uiLayout[it.key()][id]["ParentOfGroup"].is_null())
        {
          parentOf = uiLayout[it.key()][id]["ParentOfGroup"].get<std::string>();
        }
        if (!uiLayout[it.key()][id]["Action"].is_null())
        {
          actionID = uiLayout[it.key()][id]["Action"].get<std::string>();
        }
        if (!uiLayout[it.key()][id]["Text"].is_null())
        {
          text = uiLayout[it.key()][id]["Text"].get<std::string>();
        }

        std::shared_ptr<UiElement> uiElement;
        // Create the ui elements
        if (uiLayout[it.key()][id]["Type"] == "ImageButton")
        {
          uiElement = std::make_shared<Button>(elementRect);
          uiElement->setSpriteID(spriteID);
        }
        if (uiLayout[it.key()][id]["Type"] == "TextButton")
        {
          uiElement = std::make_shared<Button>(elementRect);
          uiElement->setText(text);
        }
        if (uiLayout[it.key()][id]["Type"] == "Text")
        {
          uiElement = std::make_shared<Text>(elementRect);
          uiElement->setText(text);
        }
        if (uiLayout[it.key()][id]["Type"] == "Frame")
        {
          uiElement = std::make_shared<Frame>(elementRect);
        }
        if (uiLayout[it.key()][id]["Type"] == "Checkbox")
        {
          uiElement = std::make_shared<Checkbox>(elementRect);
        }
        if (uiLayout[it.key()][id]["Type"] == "ComboBox")
        {
          uiElement = std::make_shared<ComboBox>(elementRect);
          uiElement->setText(text);
        }

        uiElement->setVisibility(visible);
        uiElement->setTooltipText(tooltipText);
        uiElement->setActionID(actionID);
        uiElement->setParentID(parentOf);
        uiElement->setGroupID(groupID);
        uiElement->setToggleButton(toggleButton);

        if (parentOf != "")
        {
          uiElement->registerToggleUIFunction(Signal::slot(this, &UIManager::toggleGroupVisibility));
        }
        if (actionID == "RaiseTerrain")
        {
          uiElement->registerCallbackFunction([]() {
            Resources::getTerrainEditMode() == Resources::TERRAIN_RAISE
                ? Resources::setTerrainEditMode(Resources::NO_TERRAIN_EDIT)
                : Resources::setTerrainEditMode(Resources::TERRAIN_RAISE);
          });
        }
        else if (actionID == "LowerTerrain")
        {
          uiElement->registerCallbackFunction([]() {
            Resources::getTerrainEditMode() == Resources::TERRAIN_LOWER
                ? Resources::setTerrainEditMode(Resources::NO_TERRAIN_EDIT)
                : Resources::setTerrainEditMode(Resources::TERRAIN_LOWER);
          });
        }
        else if (actionID == "QuitGame")
        {
          uiElement->registerCallbackFunction(Signal::slot(Engine::Instance(), &Engine::quitGame));
        }

        // store the element in a vector
        _uiElements.emplace_back(uiElement);
      }
    }
  }
  _tooltip->setVisibility(false);
}

void UIManager::setFPSCounterText(const std::string &fps) { _fpsCounter->setText(fps); }

void UIManager::drawUI()
{
  for (const auto &it : _uiElements)
  {
    if (it->isVisible())
    {
      it->draw();
    }
  }

  if (_showDebugMenu)
  {
    _fpsCounter->draw();
  }
  _tooltip->draw();
}

void UIManager::addToGroup(int groupID, std::shared_ptr<UiElement> uiElement) { _group[groupID] = std::move(uiElement); }

void UIManager::toggleGroupVisibility(const std::string &groupID)
{
  for (const std::shared_ptr<UiElement> &it : _uiElements)
  {
    if (it->getGroupID() == groupID)
    {
      it->setVisibility(!it->isVisible());
    }
  }
}

void UIManager::startTooltip(SDL_Event &event, const std::string &tooltipText)
{
  _tooltip->setText(tooltipText);

  _tooltip->setPosition(event.button.x - _tooltip->getUiElementRect().w / 2, event.button.y - _tooltip->getUiElementRect().h);

  _tooltip->startTimer();
}

void UIManager::stopTooltip() { _tooltip->reset(); }