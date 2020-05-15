#pragma once

#include <memory>
#include "cppDelaunay/delaunay/Voronoi.h"

namespace DelaunayExt {
	std::unique_ptr<Delaunay::Voronoi> VoronoiWithLloydRelaxation(
		std::vector<Delaunay::Point*>& points,
		Delaunay::Rectangle& plotBounds,
		int iterations
	);
}