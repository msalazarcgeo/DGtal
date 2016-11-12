/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

/**
 * @file testMeshVoxelization.cpp
 * @ingroup Tests
 *
 * @date 2016/01/25
 *
 * Functions for testing class MeshVoxelizer
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "DGtalCatch.h"
#include "DGtal/shapes/MeshVoxelizer.h"
#include "DGtal/kernel/sets/CDigitalSet.h"
#include "DGtal/kernel/domains/HyperRectDomain.h"
#include "DGtal/io/readers/MeshReader.h"
#include "DGtal/io/Display3D.h"
///////////////////////////////////////////////////////////////////////////////

using namespace DGtal;
using namespace Z3i;

#define SEP 26

TEST_CASE("Basic voxelization test", "[voxelization]")
{
  using Space3Dint = SpaceND<3>;
  using Domain   = HyperRectDomain<Space3Dint>;
  using PointR3  = PointVector<3, double>;
  using VectorR3 = PointVector<3, double>;
  using PointR2  = PointVector<2, double>;
  using PointZ3  = PointVector<3, int>;
  using DigitalSet = DigitalSetBySTLSet<Domain>;
  using MeshVoxelizer = MeshVoxelizer< DigitalSet, SEP>;
  
  // ---------------------------------------------------------
  SECTION("Test distance point/plan 3D")
  {
    // Triangle ABC in R3
    const PointR3 A(38.6908 , 14.5441 , -0.71205);
    const PointR3 B(34.6171 , 13.5999 , 2.44455);
    const PointR3 C(37.4205 , 2.44239 , 6.31301);
    
    // Point v
    const PointZ3 v(35, 2, 5);
    
    const VectorR3 e1 = A - B;
    const VectorR3 e2 = A - C;
    
    double distance = MeshVoxelizer::distance(A, e1.crossProduct(e2), v);
    
    REQUIRE( 2.40 < distance );
    REQUIRE( distance < 2.41 );
  }
  
  // ---------------------------------------------------------
  SECTION("Test if 2D point is inside triangle 2D")
  {
    // Triangle ABC in R2
    PointR2 A(1.0, 1.0);
    PointR2 B(2.0, 3.0);
    PointR2 C(3.0, 1.0);
    
    typedef InHalfPlaneBySimple3x3Matrix<PointR2, double> OrientationFunctor;
    OrientationFunctor orientationFunctor;
    
    //geometric predicate
    PredicateFromOrientationFunctor2<OrientationFunctor> pointPredicate( orientationFunctor );
    
    if(! pointPredicate(A, B, C))
    {
      PointR2 tmp = A;
      A = C;
      C = tmp;
    }
    
    // Test if point v is inside triangle ABC
    // 0 : outside
    // 1 : inside
    // 2 : on edge
    // 3 : on vertex
    PointR2 v;
    
    v[0] = 3.0;
    v[1] = 3.0;
    REQUIRE(MeshVoxelizer::pointIsInside2DTriangle(A, B, C, v) == 0); // outside
    
    v[0] = 2.0;
    v[1] = 2.0;
    REQUIRE(MeshVoxelizer::pointIsInside2DTriangle(A, B, C, v) == 1); // inside
    
    v[0] = 2;
    v[1] = 1;
    REQUIRE(MeshVoxelizer::pointIsInside2DTriangle(A, B, C, v) == 2); // on edge
    
    v[0] = 3;
    v[1] = 1;
    REQUIRE(MeshVoxelizer::pointIsInside2DTriangle(A, B, C, v) == 3); // on vertex
    
    // another triangle
    A[0] = -0.891282; A[1] = 9.91201;
    B[0] = -1.40823; B[1] = 9.91261;
    C[0] = -1.36963; C[1] = 9.37414;
    
    v[0] = -1.16961;
    v[1] = 9.83039;
    REQUIRE(MeshVoxelizer::pointIsInside2DTriangle(A, B, C, v) == 1); // inside
  }
  
  // ---------------------------------------------------------
  SECTION("Test if 3D point is inside voxel")
  {
    // Triangle ABC in R2
    PointR3 P(-0.89, 9.91, 0.86);
    PointZ3 v(-1, 10, 1);
    
    REQUIRE(MeshVoxelizer::pointIsInsideVoxel(P, v) == true); // inside
    
    P[0] = -1.41;
    P[1] = 9.91;
    REQUIRE(MeshVoxelizer::pointIsInsideVoxel(P, v) == true); // inside
    
    P[0] = -1.37;
    P[1] = 9.37;
    REQUIRE(MeshVoxelizer::pointIsInsideVoxel(P, v) == false); // outside
    
    P[0] = -1.17;
    P[1] = 9.83;
    P[2] = 0;
    REQUIRE(MeshVoxelizer::pointIsInsideVoxel(P, v) == false); // outside
  }
  
  // ---------------------------------------------------------
  SECTION("Voxelization of a single triange")
  {
    Domain domain(Point(0,0,0), Point(10,10,10));
    DigitalSet outputSet(domain);
    MeshVoxelizer voxelizer;
    voxelizer.voxelize(outputSet, Point(5,0,0), Point(0,5,0), Point(0,0,5));
    REQUIRE( outputSet.size() == 12);
  }
}
