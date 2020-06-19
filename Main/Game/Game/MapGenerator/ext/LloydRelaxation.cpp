#include <vector>
#include "LloydRelaxation.h"

using namespace Delaunay;

// https://github.com/PouletFrit/csDelaunay/blob/master/Delaunay/Voronoi.cs
std::unique_ptr<Delaunay::Voronoi> DelaunayExt::VoronoiWithLloydRelaxation(
	std::vector<Delaunay::Point*>& points,
	Delaunay::Rectangle& plotBounds,
	int iterations
	)
{
	std::unique_ptr<Voronoi> voronoi =
		std::unique_ptr<Voronoi>(new Voronoi(points, NULL, plotBounds));

	// Reapeat the whole process for the number of iterations asked
	for (int i = 0; i < iterations; i++) {
		std::vector<Point*> newPoints;
		// Go thourgh all sites
		auto regions = voronoi->regions();

		for(auto region : regions)
		{
			// Loop all corners of the site to calculate the centroid
			//std::vector<Point*> region = voronoi.region(site);

			if (region.size() < 1) {
				continue;
			}

			Point* centroid = Point::create(0, 0);
			float signedArea = 0;
			float x0 = 0;
			float y0 = 0;
			float x1 = 0;
			float y1 = 0;
			float a = 0;
			// For all vertices except last
			for (int j = 0; j < region.size() - 1; j++) {
				x0 = region[j]->x;
				y0 = region[j]->y;
				x1 = region[j + 1]->x;
				y1 = region[j + 1]->y;
				a = x0 * y1 - x1 * y0;
				signedArea += a;
				centroid->x += (x0 + x1) * a;
				centroid->y += (y0 + y1) * a;
			}
			// Do last vertex
			x0 = region[region.size() - 1]->x;
			y0 = region[region.size() - 1]->y;
			x1 = region[0]->x;
			y1 = region[0]->y;
			a = x0 * y1 - x1 * y0;
			signedArea += a;
			centroid->x += (x0 + x1) * a;
			centroid->y += (y0 + y1) * a;

			signedArea *= 0.5f;
			centroid->x /= (6.0f * signedArea);
			centroid->y /= (6.0f * signedArea);
			// Move site to the centroid of its Voronoi cell
			newPoints.push_back(centroid);
		}

		// Between each replacement of the cendroid of the cell,
		// we need to recompute Voronoi diagram:
		voronoi =
			std::unique_ptr<Voronoi>(new Voronoi(newPoints, NULL, plotBounds));
	}
	return voronoi;
}
