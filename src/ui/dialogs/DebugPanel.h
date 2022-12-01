#pragma once

#include "common.h"
#include "ui/Window.h"

class DebugPanel : public Window {
public:
    explicit DebugPanel();

    void doWindowContents() override;
};
