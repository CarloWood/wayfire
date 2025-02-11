#pragma once

#include "wayfire/toplevel-view.hpp"
#include <string>

inline std::string tiled_edges_to_string(uint32_t tiled_edges)
{
    if (tiled_edges == 0)
        return {"0"};

    if ((tiled_edges & ~wf::TILED_EDGES_ALL))
        return {"Illegal tiled_edges!"};

    if (tiled_edges == wf::TILED_EDGES_ALL)
        return {"TILED_EDGES_ALL"};

    if (tiled_edges == wf::TILED_EDGES_VERTICAL)
        return {"TILED_EDGES_VERTICAL"};

    if (tiled_edges == wf::TILED_EDGES_HORIZONTAL)
        return {"TILED_EDGES_HORIZONTAL"};

    wlr_edges edges[4] = { WLR_EDGE_TOP, WLR_EDGE_BOTTOM, WLR_EDGE_LEFT, WLR_EDGE_RIGHT };
    char const* edges_str[4] = { "WLR_EDGE_TOP", "WLR_EDGE_BOTTOM", "WLR_EDGE_LEFT", "WLR_EDGE_RIGHT" };

    std::string result;
    for (int i = 0; i < 4; ++i)
    {
        if ((tiled_edges & edges[i]))
        {
            if (!result.empty())
                result += "|";
            result += edges_str[i];
        }
    }

    return result;
}
