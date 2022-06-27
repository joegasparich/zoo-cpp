#include "PathGrid.h"
#include "Zoo.h"
#include "constants/assets.h"
#include "Messenger.h"

PathGrid::PathGrid(unsigned int rows, unsigned int cols) : m_rows(rows), m_cols(cols) {}

void PathGrid::setup() {
    // Grid
    for (unsigned int i = 0; i < m_cols; i++) {
        m_grid.emplace_back();
        for (unsigned int j = 0; j < m_rows; j++) {
            m_grid.at(i).push_back({
                nullptr,
                {i, j},
                false,
                false
            });
        }
    }

    // Temp
    m_grid.at(3).at(4) = Path{
        &Registry::getPath(PATH_DIRT),
        {3.0f, 4.0f},
        true,
        false
    };

    // Rendering
    m_va = std::make_unique<VertexArray>();
    m_layout = std::make_unique<VertexBufferLayout>();
    m_layout->pushFloat(3); // pos
    m_layout->pushFloat(3); // texCoord
    m_layout->pushFloat(3); // colour
    m_shader = std::make_unique<Shader>("./shaders/ArrayTextureVertex.shader", "./shaders/ArrayTextureFragment.shader");
    m_textureArray = std::make_unique<ArrayTexture>(80, 32, MAX_PATHS);

    auto paths = Registry::getAllPaths();

    for(auto path : paths) {
        m_textureArray->pushTexture(*AssetManager::getSpriteSheet(path->spriteSheetPath)->m_image);
    }

    regenerateMesh();

    m_elevationListenerHandle = Messenger::on(EventType::ElevationUpdated, [this](json& data) {
        regenerateMesh();
    });

    m_isSetup = true;
}

void PathGrid::cleanup() {
    m_grid.clear();
    m_cols = 0;
    m_rows = 0;

    Messenger::unsubscribe(EventType::ElevationUpdated, m_elevationListenerHandle);

    m_isSetup = false;
}

void PathGrid::render() {
    m_va->bind();
    m_ib->bind();
    m_shader->bind();
    m_textureArray->bind();

    auto mvp = Renderer::getMVPMatrix(glm::vec2{0.0f, 0.0f}, 0.0f, 0.0f, glm::vec3{1.0f}, true);

    m_shader->setUniformMat4f("u_MVP", mvp);
    m_shader->setUniform1i("u_Textures", 0);

    Renderer::draw(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, nullptr);
}

// TODO: Chunk it?
void PathGrid::regenerateMesh() {
    float textureIndex = 0.0f;

    std::vector<ArrayTextureVertex> vertices{};
    std::vector<std::array<unsigned int, 6>> indices{};

    for (unsigned int i = 0; i < m_cols; i++) {
        for (unsigned int j = 0; j < m_rows; j++) {
            auto& path = m_grid.at(i).at(j);
            if (path.exists) {
                auto [spriteIndex, elevation] = PathGrid::getSpriteInfo(path);
                auto pos = glm::vec2{(float)i, (float)j};
                pos -= glm::vec2{0.0f, 1.0f + elevation}; // Offset cell size
                auto texCoord = AssetManager::getSpriteSheet(path.data->spriteSheetPath)->getTexCoords(spriteIndex);
                auto depth = Renderer::getDepth(pos.y);

                vertices.push_back({glm::vec3{pos + glm::vec2{0.0f, 0.0f}, depth}, {texCoord[0], textureIndex}, glm::vec3{1.0f}});
                vertices.push_back({glm::vec3{pos + glm::vec2{1.0f, 0.0f}, depth}, {texCoord[1], textureIndex}, glm::vec3{1.0f}});
                vertices.push_back({glm::vec3{pos + glm::vec2{1.0f, 2.0f}, depth}, {texCoord[2], textureIndex}, glm::vec3{1.0f}});
                vertices.push_back({glm::vec3{pos + glm::vec2{0.0f, 2.0f}, depth}, {texCoord[3], textureIndex}, glm::vec3{1.0f}});
                unsigned int base = indices.size() * 4;
                indices.push_back({
                    base, base + 1, base + 2,
                    base + 2, base + 3, base
                });
            }
        }
    }

    m_vb = std::make_unique<VertexBuffer>(&vertices[0], vertices.size() * sizeof(ArrayTextureVertex));
    m_va->addBuffer(*m_vb, *m_layout);
    m_ib = std::make_unique<IndexBuffer>(&indices[0], (unsigned int)indices.size() * 6);
    m_numIndices = indices.size() * 6;
}

Path *PathGrid::getPathAtTile(glm::ivec2 tilePos) {
    return &m_grid.at(tilePos.x).at(tilePos.y);
}

PathSpriteInfo PathGrid::getSpriteInfo(Path &path) {
    auto nw = Zoo::zoo->m_world->m_elevationGrid->getElevationAtPos(path.pos);
    auto ne = Zoo::zoo->m_world->m_elevationGrid->getElevationAtPos({path.pos.x + 1.0f, path.pos.y});
    auto sw = Zoo::zoo->m_world->m_elevationGrid->getElevationAtPos({path.pos.x, path.pos.y + 1.0f});
    auto se = Zoo::zoo->m_world->m_elevationGrid->getElevationAtPos({path.pos.x + 1.0f, path.pos.y + 1.0f});
    auto elevation = std::min({nw, ne, sw, se});

    if (nw == ne && nw == sw && nw == se) return {(unsigned int)PathSpriteIndex::Flat, elevation};
    if (nw == ne && nw > sw && sw == se) return {(unsigned int)PathSpriteIndex::HillNorth, elevation};
    if (nw == ne && nw < sw && sw == se) return {(unsigned int)PathSpriteIndex::HillSouth, elevation};
    if (nw == sw && nw < ne && ne == se) return {(unsigned int)PathSpriteIndex::HillEast, elevation};
    if (nw == sw && nw > ne && ne == se) return {(unsigned int)PathSpriteIndex::HillWest, elevation};

    return {(unsigned int)PathSpriteIndex::Flat, 0.0f};
}
