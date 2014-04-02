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
 * @file testVoronoiCovarianceMeasure.cpp
 * @ingroup Tests
 * @author Jacques-Olivier Lachaud (\c jacques-olivier.lachaud@univ-savoie.fr )
 * Laboratory of Mathematics (CNRS, UMR 5127), University of Savoie, France
 *
 * @date 2014/02/09
 *
 * Functions for testing class VoronoiCovarianceMeasure.
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>
#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/math/Statistic.h"
#include "DGtal/math/MPolynomial.h"
#include "DGtal/topology/LightImplicitDigitalSurface.h"
#include "DGtal/geometry/surfaces/estimation/CSurfaceLocalGeometricEstimator.h"
#include "DGtal/geometry/surfaces/estimation/VoronoiCovarianceMeasureOnDigitalSurface.h"
#include "DGtal/geometry/surfaces/estimation/VCMDigitalSurfaceNormalEstimator.h"
#include "DGtal/geometry/surfaces/estimation/TrueDigitalSurfaceLocalEstimator.h"
#include "DGtal/shapes/GaussDigitizer.h"
#include "DGtal/shapes/ShapeGeometricFunctors.h"
#include "DGtal/shapes/implicit/ImplicitPolynomial3Shape.h"
#include "DGtal/io/readers/MPolynomialReader.h"

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Functions for testing class VoronoiCovarianceMeasureOnSurface
///////////////////////////////////////////////////////////////////////////////

template <typename TPoint3>
struct ImplicitDigitalEllipse3 {
  typedef TPoint3 Point;
  inline
  ImplicitDigitalEllipse3( double a, double b, double c )
  : myA( a ), myB( b ), myC( c )
  {}
  inline
  bool operator()( const TPoint3 & p ) const
  {
    double x = ( (double) p[ 0 ] / myA );
    double y = ( (double) p[ 1 ] / myB );
    double z = ( (double) p[ 2 ] / myC );
    return ( x*x + y*y + z*z ) <= 1.0;
  }
  double myA, myB, myC;
};

/**
 * Example of a test. To be completed.
 *
 */
bool testVoronoiCovarianceMeasureOnSurface()
{
  unsigned int nbok = 0;
  unsigned int nb = 0;
  
  using namespace DGtal;
  using namespace DGtal::Z3i; // gets Space, Point, Domain

  typedef MPolynomial< 3, double > Polynomial3;
  typedef MPolynomialReader<3, double> Polynomial3Reader;
  typedef ImplicitPolynomial3Shape<Z3i::Space> ImplicitShape;
  typedef GaussDigitizer< Z3i::Space, ImplicitShape > ImplicitDigitalShape;
  // typedef ImplicitDigitalEllipse3<Point> ImplicitDigitalEllipse;
  typedef LightImplicitDigitalSurface<KSpace,ImplicitDigitalShape> SurfaceContainer;
  typedef DigitalSurface< SurfaceContainer > Surface;
  typedef Surface::ConstIterator ConstIterator;
  typedef SurfaceContainer::Surfel Surfel;
  typedef ExactPredicateLpSeparableMetric<Space,2> Metric;
  typedef HatPointFunction<Point,double> KernelFunction;
  // typedef BallConstantPointFunction<Point,double> KernelFunction; // worse than above
  typedef VoronoiCovarianceMeasureOnDigitalSurface<SurfaceContainer,Metric,KernelFunction> VCMOnSurface;
  trace.beginBlock("Creating Surface");
  std::string poly_str = "1.0-0.16*x^2+0.22*y^2+0.3*z^2";
  Polynomial3 poly;
  Polynomial3Reader reader;
  std::string::const_iterator iter = reader.read( poly, poly_str.begin(), poly_str.end() );
  if ( iter != poly_str.end() )
    {
      std::cerr << "ERROR: I read only <"
                << poly_str.substr( 0, iter - poly_str.begin() )
                << ">, and I built P=" << poly << std::endl;
      return 1;
    }
  CountedPtr<ImplicitShape> shape( new ImplicitShape( poly ) );

  Point p1( -10, -10, -10 );
  Point p2( 10, 10, 10 );
  KSpace K;
  nbok += K.init( p1, p2, true ) ? 1 : 0;
  nb++;
  trace.info() << "(" << nbok << "/" << nb << ") "
               << "K.init() is ok" << std::endl;

  // Digitizer
  CountedPtr<ImplicitDigitalShape> dshape( new ImplicitDigitalShape() );
  dshape->attach( *shape );
  dshape->init( p1, p2, 1.0 );

  // ImplicitDigitalEllipse ellipse( 6.0, 4.5, 3.4 );
  Surfel bel = Surfaces<KSpace>::findABel( K, *dshape, 10000 );
  SurfaceContainer* surfaceContainer = new SurfaceContainer
    ( K, *dshape, SurfelAdjacency<KSpace::dimension>( true ), bel );
  CountedConstPtrOrConstPtr<Surface> ptrSurface( new Surface( surfaceContainer ) ); // acquired
  trace.endBlock();

  trace.beginBlock("Computing VCM on surface." );
  KernelFunction chi( 1.0, 7.0 );
  CountedPtr<VCMOnSurface> vcm_surface( new VCMOnSurface( ptrSurface, Pointels, 
                                                          5.0, 7.0, chi, 7.0, Metric(), true ) );
  trace.endBlock();

  trace.beginBlock("Wrapping normal estimator." );
  typedef VCMDigitalSurfaceNormalEstimator<SurfaceContainer,Metric,KernelFunction> VCMNormalEstimator;
  // Is CNormalVectorEstimator deprecated ?
  // BOOST_CONCEPT_ASSERT(( CNormalVectorEstimator< VCMNormalEstimator > ));
  VCMNormalEstimator estimator( vcm_surface );
  estimator.init( 1.0, ptrSurface->begin(), ptrSurface->end() );
  trace.endBlock();

  trace.beginBlock("Evaluating normals wrt true normal." );
  typedef ShapeGeometricFunctors::ShapeNormalVectorFunctor<ImplicitShape> NormalFunctor;
  typedef TrueDigitalSurfaceLocalEstimator<KSpace, ImplicitShape, NormalFunctor> TrueNormalEstimator;
  
  BOOST_CONCEPT_ASSERT(( CSurfaceLocalGeometricEstimator< VCMNormalEstimator > ));
  BOOST_CONCEPT_ASSERT(( CSurfaceLocalGeometricEstimator< TrueNormalEstimator > ));
  TrueNormalEstimator true_estimator( K, NormalFunctor() );
  true_estimator.attach( shape );
  true_estimator.init( 1.0, ptrSurface->begin(), ptrSurface->end() );
  Statistic<double> error_true;
  Statistic<double> error_triv_true;
  for ( ConstIterator it = ptrSurface->begin(), itE = ptrSurface->end(); it != itE; ++it )
    {
      RealVector n_est  = estimator.eval( it );
      RealVector n_true = true_estimator.eval( it );
      RealVector n_triv = - vcm_surface->mapSurfel2Normals().find( *it )->second.trivialNormal;
      error_true.addValue( n_est.dot( n_true ) );
      error_triv_true.addValue( n_triv.dot( n_true ) );
    }
  error_true.terminate();
  error_triv_true.terminate();
  trace.info() << "VCM/true  cos angle avg = " << error_true.mean() << std::endl;
  trace.info() << "VCM/true  cos angle dev = " << sqrt( error_true.unbiasedVariance() ) << std::endl;
  trace.info() << "triv/true cos angle avg = " << error_triv_true.mean() << std::endl;
  trace.info() << "triv/true cos angle dev = " << sqrt( error_triv_true.unbiasedVariance() ) << std::endl;
  nbok += error_true.mean() > 0.95 ? 1 : 0;
  nb++;
  trace.info() << "(" << nbok << "/" << nb << ") "
               << "cos angle avg > 0.95" << std::endl;
  nbok += sqrt( error_true.unbiasedVariance() ) < 0.05 ? 1 : 0;
  nb++;
  trace.info() << "(" << nbok << "/" << nb << ") "
               << "cos angle dev < 0.05" << std::endl;
  nbok += error_true.mean() > error_triv_true.mean() ? 1 : 0;
  nb++;
  trace.info() << "(" << nbok << "/" << nb << ") "
               << "VCM/true is closer to 1.0 than triv/true." << std::endl;
  trace.endBlock();

  return nbok == nb;
}

///////////////////////////////////////////////////////////////////////////////
// Standard services - public :

int main( int argc, char** argv )
{
  using namespace std;
  using namespace DGtal;
  trace.beginBlock ( "Testing VoronoiCovarianceMeasureOnSurface ..." );
  bool res = testVoronoiCovarianceMeasureOnSurface();
  trace.emphase() << ( res ? "Passed." : "Error." ) << endl;
  trace.endBlock();
  return res ? 0 : 1;
}
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
