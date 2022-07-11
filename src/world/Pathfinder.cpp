#include "Pathfinder.h"
#include "util/util.h"

Pathfinder::Pathfinder(unsigned int width, unsigned int height) : m_cols{width}, m_rows{height} {
    auto grid = std::make_unique<std::vector<std::vector<Tile>>>();

    // Generate nodes
    for (int i = 0; i < width; i++) {
        grid->emplace_back();
        for (int j = 0; j < height; j++) {
            grid->at(i).emplace_back();
        }
    }

    m_tileGrid = std::move(grid);
    m_isSetup = true;

    // Populate connections
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            for (int d = 0; d < 8; d++) {
                setAccessibility({i, j}, (Direction) d, true);
            }
        }
    }
}

std::vector<glm::ivec2> Pathfinder::reconstructPath(const std::vector<std::vector<Node>> &cellDetails, const glm::ivec2 &dest) {
    std::vector<glm::ivec2> path;

    path.push_back(dest);
    auto [x, y] = dest;
    while(cellDetails[x][y].parent != glm::ivec2{ x, y }) {
        auto nextNode = cellDetails[x][y].parent;
        path.push_back(nextNode);
        x = nextNode.x;
        y = nextNode.y;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

// https://www.geeksforgeeks.org/a-search-algorithm/
std::vector<glm::ivec2> Pathfinder::getPath(glm::ivec2 from, glm::ivec2 to) {
    if (!isAccessible(to)) return {};
    if (from == to) return {};

    bool closedList[m_cols][m_rows];
    memset(closedList, false, sizeof(closedList));

    std::vector<std::vector<Node>> cellDetails{
        m_cols,
        std::vector<Node>(m_rows)
    };

    auto [x, y] = from;
    cellDetails[x][y].fCost = 0.0;
    cellDetails[x][y].gCost = 0.0;
    cellDetails[x][y].hCost = 0.0;
    cellDetails[x][y].parent = from;

    std::priority_queue<Tuple, std::vector<Tuple>, std::greater<>> openList;

    openList.emplace(0.0, x, y);

    while (!openList.empty()) {
        const Tuple& p = openList.top();
        x = get<1>(p);
        y = get<2>(p);

        openList.pop();
        closedList[x][y] = true;

        for (auto neighbour : getNeighbours({ x, y })) {
            auto [nx, ny] = neighbour;

            if (neighbour == to) {
                // Destination found!
                cellDetails[nx][ny].parent = { x, y };
                std::cout << "Found route to dest!" << std::endl;
                return reconstructPath(cellDetails, to);
            }

            // If the successor is already on the closed list then ignore it
            if (closedList[nx][ny]) continue;

            auto gNew = cellDetails[x][y].gCost + 1.0;
            auto hNew = calculateHValue(neighbour, to);
            auto fNew = gNew + hNew;

            // If it isn’t on the open list, add it to the open list. Make the current square the parent of this
            // square. Record the f, g, and h costs of the square cell
            //             OR
            // If it is on the open list already, check to see if this path to that square is better, using
            // 'f' cost as the measure.
            if (cellDetails[nx][ny].fCost != -1.0 && cellDetails[nx][ny].fCost <= fNew) continue;

            openList.emplace(fNew, nx, ny);
            // Update the details of this cell
            cellDetails[nx][ny].gCost = gNew;
            cellDetails[nx][ny].fCost = fNew;
            cellDetails[nx][ny].hCost = hNew;
            cellDetails[nx][ny].parent = {x, y};
        }
    }

    std::cout << "Failed to find route to dest " << vecToString(to) << std::endl;
    return {};
}

void Pathfinder::setAccessibility(glm::ivec2 tile, Direction direction, bool accessible) {
    if (!m_isSetup) return;

//    auto x = tile.x, y = tile.y;
//    auto width = m_tileGrid->size();
//    auto height = m_tileGrid->at(0).size();
//    auto node = m_tileGrid->at(x).at(y);

    m_tileGrid->at(tile.x).at(tile.y).connections[(int)direction] = accessible;

//    if (accessible) {
//        switch (direction) {
//            case Direction::N:  if (y > 0)                          node.connections[(int)direction] = &m_tileGrid->at(x).at(y - 1);
//            case Direction::NE: if (x < width-1 && y > 0)           node.connections[(int)direction] = &m_tileGrid->at(x + 1).at(y - 1);
//            case Direction::E:  if (x < width-1)                    node.connections[(int)direction] = &m_tileGrid->at(x + 1).at(y);
//            case Direction::SE: if (x < width-1 && y < height-1)    node.connections[(int)direction] = &m_tileGrid->at(x + 1).at(y + 1);
//            case Direction::S:  if (y < height-1)                   node.connections[(int)direction] = &m_tileGrid->at(x).at(y + 1);
//            case Direction::SW: if (x > 0 && y < height-1)          node.connections[(int)direction] = &m_tileGrid->at(x - 1).at(y + 1);
//            case Direction::W:  if (x > 0)                          node.connections[(int)direction] = &m_tileGrid->at(x - 1).at(y);
//            case Direction::NW: if (x > 0 && y > 0)                 node.connections[(int)direction] = &m_tileGrid->at(x - 1).at(y - 1);
//        }
//    } else {
//        node.connections[(int)direction] = nullptr;
//    }
}

bool Pathfinder::isAccessible(glm::ivec2 tilePos) {
    auto [x, y] = tilePos;
    auto width = m_tileGrid->size();
    auto height = m_tileGrid->at(0).size();
    auto tile = m_tileGrid->at(x).at(y);

    if (!tile.accessible) return false;

    // Make sure at least one direction is accessible
    if (y > 0                       && m_tileGrid->at(x)    .at(y - 1).connections[(int)Direction::S])  return true;
    if (x < width-1 && y > 0        && m_tileGrid->at(x + 1).at(y - 1).connections[(int)Direction::SW]) return true;
    if (x < width-1                 && m_tileGrid->at(x + 1).at(y)    .connections[(int)Direction::W])  return true;
    if (x < width-1 && y < height-1 && m_tileGrid->at(x + 1).at(y + 1).connections[(int)Direction::NW]) return true;
    if (y < height-1                && m_tileGrid->at(x)    .at(y + 1).connections[(int)Direction::N])  return true;
    if (x > 0 && y < height-1       && m_tileGrid->at(x - 1).at(y + 1).connections[(int)Direction::NE]) return true;
    if (x > 0                       && m_tileGrid->at(x - 1).at(y)    .connections[(int)Direction::E])  return true;
    if (x > 0 && y > 0              && m_tileGrid->at(x - 1).at(y - 1).connections[(int)Direction::SE]) return true;
}

std::vector<glm::ivec2> Pathfinder::getNeighbours(glm::ivec2 tilePos) {
    auto [x, y] = tilePos;
    auto width = m_tileGrid->size();
    auto height = m_tileGrid->at(0).size();
    auto tile = m_tileGrid->at(x).at(y);

    std::vector<glm::ivec2> connections;

    if (y > 0                       && tile.connections[0]) connections.emplace_back(x, y - 1);
    if (x < width-1 && y > 0        && tile.connections[1]) connections.emplace_back(x + 1, y - 1);
    if (x < width-1                 && tile.connections[2]) connections.emplace_back(x + 1, y);
    if (x < width-1 && y < height-1 && tile.connections[3]) connections.emplace_back(x + 1, y + 1);
    if (y < height-1                && tile.connections[4]) connections.emplace_back(x, y + 1);
    if (x > 0 && y < height-1       && tile.connections[5]) connections.emplace_back(x - 1, y + 1);
    if (x > 0                       && tile.connections[6]) connections.emplace_back(x - 1, y);
    if (x > 0 && y > 0              && tile.connections[7]) connections.emplace_back(x - 1, y - 1);

    return connections;
}

double Pathfinder::calculateHValue(glm::ivec2 a, glm::ivec2 b) {
    return sqrt(pow((a.x - b.x), 2.0) + pow((a.y - b.y), 2.0));
}
