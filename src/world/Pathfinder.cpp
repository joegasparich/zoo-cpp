#include "Pathfinder.h"
#include "Root.h"
#include "util/util.h"
#include "Debug.h"
#include "util/uuid.h"

Pathfinder::Pathfinder(unsigned int width, unsigned int height) : cols{width}, rows{height} {
    TraceLog(LOG_TRACE, "Setting up pathfinder");

    auto grid = std::make_unique<std::vector<std::vector<Tile>>>();

    // Generate nodes
    for (int i = 0; i < width; i++) {
        grid->emplace_back();
        for (int j = 0; j < height; j++) {
            grid->at(i).emplace_back();
        }
    }

    tileGrid = std::move(grid);
    pathRequests = std::make_unique<std::map<std::string, std::future<path>>>();
    isSetup = true;

    // Populate connections
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            for (int d = 0; d < 8; d++) {
                setAccessibility({i, j}, (Direction) d, true);
            }
        }
    }
}

path Pathfinder::reconstructPath(const std::vector<std::vector<Node>> &cellDetails, const Cell &dest) {
    path path;

    path.push_back(dest);
    auto [x, y] = dest;
    while(cellDetails[x][y].parent != Cell{ x, y }) {
        auto nextNode = cellDetails[x][y].parent;
        path.push_back(nextNode);
        x = nextNode.x;
        y = nextNode.y;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

// https://www.geeksforgeeks.org/a-search-algorithm/
path Pathfinder::getPath(Cell from, Cell to) {
    assert(isSetup);
    if (!isTileInGrid(from) || !isTileInGrid(to)) return {};
    if (!isAccessible(to)) return {};
    if (from == to) return {};

    bool closedList[cols][rows];
    memset(closedList, false, sizeof(closedList));

    std::vector<std::vector<Node>> cellDetails{
        cols,
        std::vector<Node>(rows)
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
                return reconstructPath(cellDetails, to);
            }

            // If the successor is already on the closed list then ignore it
            if (closedList[nx][ny]) continue;

            auto gNew = cellDetails[x][y].gCost + Root::zoo()->world->getTileWalkability({x, y});
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

    TraceLog(LOG_TRACE, "Failed to find route to dest: %s", to.toString().c_str());
    return {};
}

std::string Pathfinder::requestAsyncPath(Cell from, Cell to) {
    assert(isSetup);
    auto handle = uuid::generate();
    // TODO: Do we need to lock the tileGrid?
    pathRequests->insert({handle, std::async(std::launch::async, &Pathfinder::getPath, this, from, to)});

    return handle;
}

bool Pathfinder::asyncPathExists(const std::string& handle) {
    assert(isSetup);
    return pathRequests->contains(handle);
}

bool Pathfinder::asyncPathReady(const std::string& handle) {
    assert(isSetup);
    if (!asyncPathExists(handle)) return false;

    return pathRequests->at(handle).wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

path Pathfinder::getAsyncPath(const std::string& handle) {
    assert(isSetup);
    if (!asyncPathExists(handle)) return {};

    auto path = pathRequests->at(handle).get();
    pathRequests->erase(handle);

    return path;
}

void Pathfinder::setAccessibility(Cell tile, bool accessible) {
    assert(isSetup);

    tileGrid->at(tile.x).at(tile.y).accessible = accessible;
}

void Pathfinder::setAccessibility(Cell tile, Direction direction, bool accessible) {
    assert(isSetup);
    if (!isTileInGrid(tile)) return;

    tileGrid->at(tile.x).at(tile.y).connections[int(direction)] = accessible;
}

bool Pathfinder::isAccessible(Cell tilePos) {
    assert(isSetup);
    auto [x, y] = tilePos;
    auto width = tileGrid->size();
    auto height = tileGrid->at(0).size();
    auto tile = tileGrid->at(x).at(y);

    if (!tile.accessible) return false;
    if (!Root::zoo()->world->getTileWalkability(tilePos)) return false;

    // Make sure at least one direction is accessible
    if (y > 0 &&                       tileGrid->at(x)    .at(y - 1).connections[int(Direction::S)])  return true;
    if (x < width-1 && y > 0 &&        tileGrid->at(x + 1).at(y - 1).connections[int(Direction::SW)]) return true;
    if (x < width-1 &&                 tileGrid->at(x + 1).at(y)    .connections[int(Direction::W)])  return true;
    if (x < width-1 && y < height-1 && tileGrid->at(x + 1).at(y + 1).connections[int(Direction::NW)]) return true;
    if (y < height-1 &&                tileGrid->at(x)    .at(y + 1).connections[int(Direction::N)])  return true;
    if (x > 0 && y < height-1 &&       tileGrid->at(x - 1).at(y + 1).connections[int(Direction::NE)]) return true;
    if (x > 0 &&                       tileGrid->at(x - 1).at(y)    .connections[int(Direction::E)])  return true;
    if (x > 0 && y > 0 &&              tileGrid->at(x - 1).at(y - 1).connections[int(Direction::SE)]) return true;
}

path Pathfinder::getNeighbours(Cell tilePos) {
    auto [x, y] = tilePos;
    auto width = tileGrid->size();
    auto height = tileGrid->at(0).size();
    auto tile = tileGrid->at(x).at(y);

    path connections;

    if (y > 0                       && isAccessible({x, y - 1})     && tile.connections[int(Direction::N)])  connections.emplace_back(x, y - 1);
    if (x < width-1 && y > 0        && isAccessible({x + 1, y - 1}) && tile.connections[int(Direction::NE)]) connections.emplace_back(x + 1, y - 1);
    if (x < width-1                 && isAccessible({x + 1, y})     && tile.connections[int(Direction::E)])  connections.emplace_back(x + 1, y);
    if (x < width-1 && y < height-1 && isAccessible({x + 1, y + 1}) && tile.connections[int(Direction::SE)]) connections.emplace_back(x + 1, y + 1);
    if (y < height-1                && isAccessible({x, y + 1})     && tile.connections[int(Direction::S)])  connections.emplace_back(x, y + 1);
    if (x > 0 && y < height-1       && isAccessible({x - 1, y + 1}) && tile.connections[int(Direction::SW)]) connections.emplace_back(x - 1, y + 1);
    if (x > 0                       && isAccessible({x - 1, y})     && tile.connections[int(Direction::W)])  connections.emplace_back(x - 1, y);
    if (x > 0 && y > 0              && isAccessible({x - 1, y - 1}) && tile.connections[int(Direction::NW)]) connections.emplace_back(x - 1, y - 1);

    return connections;
}

void Pathfinder::drawDebugGrid() {
    assert(isSetup);
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            if (!isAccessible({i, j})) continue;

            for (auto neighbour : getNeighbours({ i, j })) {
                auto [nx, ny] = neighbour;
                Debug::drawLine({i + 0.5f, j + 0.5f}, {nx + 0.5f, ny + 0.5f}, BLUE, true);
            }
        }
    }
}

double Pathfinder::calculateHValue(Cell a, Cell b) {
    return Vector2Distance(a, b);
}

bool Pathfinder::isTileInGrid(Cell tile) {
    return tile.x >= 0 && tile.x < cols && tile.y >= 0 && tile.y < rows;
}
