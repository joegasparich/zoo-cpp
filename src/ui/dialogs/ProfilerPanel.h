#pragma once

#include "common.h"
#include "ui/Window.h"

class ProfilerPanel : public Window {
public:
    explicit ProfilerPanel();

    void doWindowContents() override;
};
