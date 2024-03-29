#pragma once

#include "Tool.h"
#include "ToolManager.h"
#include "world/BiomeGrid.h"

class BiomeTool : public Tool {
public:
    explicit BiomeTool(ToolManager &toolManager);
    ~BiomeTool() override;

    void set() override;
    void update() override;
    void onInput(InputEvent* event) override;
    void onGUI() override;

    std::string getName() override;
    ToolType getType() override;

    void setBiome(Biome biome);
    Biome getBiome();
private:
    Biome currentBiome;
    bool dragging;
    std::unique_ptr<std::map<std::string, json>> oldChunkData;
};
