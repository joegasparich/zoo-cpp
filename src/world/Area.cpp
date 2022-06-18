#include <util/jmath.h>

#include <utility>
#include "Area.h"

Area::Area(std::string id) :
    m_id{std::move(id)},
    m_colour{jmath::random(), jmath::random(), jmath::random()}
{}
Area::~Area() = default;
