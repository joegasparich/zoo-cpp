#pragma once

#include "Tool.h"
#include "ToolManager.h"
#include "world/BiomeGrid.h"

class BiomeTool : public Tool {
public:
    BiomeTool(ToolManager &toolManager);
    ~BiomeTool();

    void set() override;
    void unset() override;
    void postUpdate() override;

    std::string getName() override;
    ToolType getType() override;

    void setBiome(Biome biome);
    Biome getBiome();
private:
    void update() override;

    std::string m_panelId;
    Biome m_currentBiome;
};
