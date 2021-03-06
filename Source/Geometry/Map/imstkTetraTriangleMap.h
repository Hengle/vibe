/*=========================================================================

   Library: iMSTK

   Copyright (c) Kitware, Inc. & Center for Modeling, Simulation,
   & Imaging in Medicine, Rensselaer Polytechnic Institute.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

=========================================================================*/

#pragma once

#include <limits>

// imstk
#include "imstkGeometryMap.h"
#include "imstkSurfaceMesh.h"
#include "imstkTetrahedralMesh.h"

namespace imstk
{
///
/// \class TetraTriangleMap
///
/// \brief Computes and applies the triangle-tetrahedra map. The master mesh is the
///  tetrahedral mesh and the slave is the surface triangular mesh.
///
class TetraTriangleMap : public GeometryMap
{
public:
    ///
    /// \brief Constructor
    ///
    TetraTriangleMap() : GeometryMap(GeometryMap::Type::TetraTriangle), m_boundingBoxAvailable(false) {}

    ///
    /// \brief Constructor
    ///
    TetraTriangleMap(std::shared_ptr<Geometry> master, std::shared_ptr<Geometry> slave)
        : GeometryMap(GeometryMap::Type::TetraTriangle), m_boundingBoxAvailable(false)
    {
        this->setMaster(master);
        this->setSlave(slave);
    }

    ///
    /// \brief Destructor
    ///
    virtual ~TetraTriangleMap() override = default;

    ///
    /// \brief Compute the tetra-triangle mesh map
    ///
    void compute() override;

	///
	/// \brief Load the tetra-triangle mesh map
	///
	void load(std::string p_filePath);

    ///
    /// \brief write tetra-triangle mesh map to file
    ///
	bool write(std::string p_filePath);

    ///
    /// \brief Apply (if active) the tetra-triangle mesh map
    ///
    void apply() override;

    ///
    /// \brief Print the map
    ///
    void print() const override;

    ///
    /// \brief Check the validity of the map
    ///
    bool isValid() const override;

    ///
    /// \brief Set the geometry that dictates the map
    ///
    void setMaster(std::shared_ptr<Geometry> master) override;

    ///
    /// \brief Set the geometry that follows the master
    ///
    void setSlave(std::shared_ptr<Geometry> slave) override;

protected:

    ///
    /// \brief Find the tetrahedron that encloses a given point in 3D space
    ///
    size_t findEnclosingTetrahedron(const Vec3d& pos) const;

    ///
    /// \brief Update bounding box of each tetrahedra of the mesh
    ///
    void updateBoundingBox();

    ///
    /// \brief Find the closest tetrahedron based on the distance to their centroids for a given
    /// point in 3D space
    ///
    size_t findClosestTetrahedron(const Vec3d& pos) const;

    std::vector<TetrahedralMesh::WeightsArray> m_verticesWeights; ///> weights

    std::vector<size_t> m_verticesEnclosingTetraId;               ///> Enclosing tetrahedra to interpolate the weights upon

    std::vector<Vec3d> m_bBoxMin;
    std::vector<Vec3d> m_bBoxMax;
    bool m_boundingBoxAvailable;

    bool m_write = true;
private:
    StdVectorOfVec3d m_slaveVerts;
};
}  // namespace imstk
