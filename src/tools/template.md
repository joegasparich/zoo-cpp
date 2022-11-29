[Tool.h](Tool.h)

```cpp
enum class ToolType {
    ...
    New
};
```

Header

```cpp
#pragma once

#include "Tool.h"

class NewTool : public Tool {
public:
    NewTool(ToolManager& toolManager);
    ~NewTool();

    void set() override;
    void update() override;
    void onInput(InputEvent* event) override;

    std::string getName() override;
    ToolType getType() override;

private:
};
```

Source

```cpp
#include "NewTool.h"
#include "ToolManager.h"
#include "ToolGhost.h"

NewTool::NewTool (ToolManager& toolManager) : Tool(toolManager) {}
NewTool::~NewTool () = default;

void NewTool::set () {
    toolManager.ghost->type = GhostType::Circle;
}

void NewTool::update () {
    toolManager.ghost->canPlace = true;
}

void NewTool::onInput (InputEvent* event) {
    if (event->mouseButtonDown == MOUSE_BUTTON_LEFT) {
        if (!toolManager.ghost->canPlace) return;

        // Do something

        event->consume();
    }
}

std::string NewTool::getName () {
    return "New Tool";
}

ToolType NewTool::getType () {
    return ToolType::New;
}
```

[ToolManager.cpp](ToolManager.cpp)

```cpp
    switch(type) {
        ...
        case ToolType::New:
            activeTool = std::make_unique<NewTool>(*this);
            break;
        ...
```

[Toolbar.cpp](../ui/dialogs/Toolbar.cpp)

```cpp
    ...
    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "New"))
        m_toolManager->setTool(ToolType::New);
    curX += BUTTON_WIDTH + GAP_SMALL;
    ...
```