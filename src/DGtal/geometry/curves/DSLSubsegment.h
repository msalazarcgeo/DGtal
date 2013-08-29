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

#pragma once

/**
 * @file DSLSubsegment.h
 * @author Isabelle Sivignon (\c isabelle.sivignon@gipsa-lab.grenoble-inp.fr )
 * gipsa-lab Grenoble Images Parole Signal Automatique (CNRS, UMR 5216), CNRS, France
 *
 * @date 2012/07/17
 *
 *
 */

#if defined(DSLSubsegment_RECURSES)
#error Recursive header files inclusion detected in DSLSubsegment.h
#else // defined(DSLSubsegment_RECURSES)
/** Prevents recursive inclusion of headers. */
#define DSLSubsegment_RECURSES

#if !defined DSLSubsegment_h
/** Prevents repeated inclusion of headers. */
#define DSLSubsegment_h

//////////////////////////////////////////////////////////////////////////////
// Inclusions
#include <iostream>
#include "DGtal/base/Common.h"
//////////////////////////////////////////////////////////////////////////////

#include "DGtal/kernel/CPointPredicate.h"
#include "DGtal/arithmetic/IntegerComputer.h"

#include <iomanip> 

namespace DGtal
{

/////////////////////////////////////////////////////////////////////////////
// class DSLSubsegment
/**
 * Description of class 'DSLSubsegment' <p>
 * \brief Aim: Given a Digital Straight line and two endpoints A and B
 * on this line, compute the minimal characteristics of the digital
 * subsegment [AB] in logarithmic time. Two algorithms are implemented:
 * one is based on the local computation of lower and upper convex
 * hulls, the other is based on a dual transformation and uses the Farey
 * fan.     
 */


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
  
//  TInteger is the type of integer used, TNumber is the type of number
//  used to represent the input DSL parameters
  template <typename TInteger, typename TNumber>
    class DSLSubsegment
  {
      // ----------------------- Standard services ------------------------------
    public:
      
      /**
       * Destructor.
     */
      ~DSLSubsegment(){};
      
    // ----------------------- Interface --------------------------------------
    public:
      
      /**
       * Writes/Displays the object on an output stream.
       * @param out the output stream where the object is written.
       */
      void selfDisplay ( std::ostream & out ) const;
      
      /**
       * Checks the validity/consistency of the object.
       * @return 'true' if the object is valid, 'false' otherwise.
       */
      bool isValid() const;
      
      
      typedef TNumber Number;
      typedef TInteger Integer;
      typedef long double FloatType;
      typedef DGtal::PointVector<2,Integer> Ray;
      typedef DGtal::PointVector<2,Integer> Point;
      typedef DGtal::PointVector<2,Number> PointF;

      typedef DGtal::PointVector<2,Integer> Vector;
      typedef DGtal::PointVector<2,Number> VectorF;
      
  protected:
      /**
       * The minimal characteristics of the subsegment AB of the
       * DSL(a,b,mu) are (myA,myB,myMu).
       *
       */
      Integer myA;
      
      /**
       * The minimal characteristics of the subsegment AB of the
       * DSL(a,b,mu) are (aa,bb,nu).
       *
       */
      Integer myB;
      
      /**
       * The minimal characteristics of the subsegment AB of the
       * DSL(a,b,mu) are (aa,bb,nu).
       *
       */
      Integer myMu;
      
      
      Number myPrecision;
      
  public:
      /**  
       * Constructor
       * Given the parameters of a DSL 0 <= ax -by + mu < b, and two points A and B of this DSL,
       * compute the parameters of the DSS [AB]. The algorithm used
       * depends on the value of the boolean (Farey fan if true, local
       * convex hull otherwise).
       * @param Number a
       * @param Number b
       * @param Number mu
       * @param Point A
       * @param Point B
       * @param a boolean
       */
      DSLSubsegment(Number a, Number b, Number mu, Point A, Point B, bool farey);

      /**
       * Constructor
       * Given a straight line of equation y = alpha x + beta, and two
       * points A and B of the OBQ digitization of this line, compute
       * the parameters of the DSS [AB]. The algorithm implemented uses
       * the Farey fan. Requires a precision parameter for floating-point geometrical predicates
       * @param Number alpha
       * @param Number beta
       * @param Point A
       * @param Point B
       * @param Number precision
       */
      DSLSubsegment(Number alpha, Number beta, Point A, Point B, Number precision = 1e-10);
       
       
  protected: 
      /**
	 Function called by the constructor when the input parameters
	 are integers and the Farey Fan algorithm is used.
      */
      void DSLSubsegmentFareyFan(Number a, Number b, Number mu, Point A, Point B);
      
      /**
	 Function called by the constructor when the input parameters
	 are integers and the local convex hull algorithm is used.
      */
      void DSLSubsegmentLocalCH(Number a, Number b, Number mu, Point A, Point B);
      
      
      /**
       * Constructor.
       * Forbidden by default (protected to avoid g++ warnings).
       */
      DSLSubsegment();
      
      
 
      class RayC
      {
      public :
	Integer x;
	Integer y;
	
	/**
	 * Default constructor.
	 * not valid
	 */
	RayC();
	
	/**
	 * Constructor of the ray R(alpha,beta): beta = -alpha x0 + y0
	 * @param Integer x0
	 * @param Integer y0
	 */
	RayC(const Integer x0, const Integer y0);
	
	/**
	 * Constructor of the ray passing through the point (p/q,r/q)
	 * and of given slope
	 * @param Integer p
	 * @param Integer q
	 * @param Integer r
	 * @param Integer slope
	 */
	RayC(const Integer p, const Integer q, const Integer r, const Integer slope);
	
	~RayC();
	
      };
      
      
      typedef enum Position
      {
	ABOVE,
	BELOW,
	ONTO
      } Position;
      
      
      Integer min (Integer a, Integer b);
      
      
      /**
       * Compute the intersection between the line of direction v
       * passing through P and the vertical line x = n.
       * The intersection point is of the form P + \alpha*v and the
       * function returns the value floor(alpha).
       * @param Point P
       * @param Vector v
       * @param Integer n
       * @return an integer
       */
      Integer intersectionVertical(Point P, Vector v, Integer n);
      
      
      /**
       * Compute the intersection between the line of direction v
       * passing through P and the line y = (aL[1]/aL[0])*x +r 
       * The intersection point is of the form P + \alpha*v and the
       * function returns the value floor(alpha).
       * @param Point P 
       * @param Vector V
       * @param Vector aL 
       * @param Integer r
       * @return an integer
       */
      Integer intersection(Point P, Vector v, Vector aL, Integer r);
      
      /**
       * Compute the intersection between the line of direction v
       * passing through P and the line y = s*x 
       * The intersection point is of the form P + \alpha*v and the
       * function returns the value floor(alpha).
       * @param Point P
       * @param Vector v
       * @param Number s
       * @return an integer
       */
      Integer intersection(Point P, Vector v, Number s);
      
      /**
       * Update the Bezout vector v according to the new point A in the
       * case of integer parameters
       * 
       */
      void update(Vector u, Point A, Vector l, Integer r, Vector *v);
      
      /**
       * Update the Bezout vector v according to the new point A in the
       * case of floating-point parameters
       *
       */
      void update(Vector u, Point A, Number s, Vector *v);
      
      /**
       * Compute the lower integer convex hull of the line of directional vector
       * l and intercept mu between the points A and B. The algorithm
       * works in two steps (from left to right and right to left). Each
       * step returns the two closest points, and these four points are
       * returned by the function.
       * 
       */
      void lowerConvexHull(Vector l, Integer mu, Point A, Point B, Point *prevInfL, Point *infL, Point *infR, Point *prevInfR);
      
      /**
       * Compute the left part of the upper and lower convex hulls of
       * the line of directional vector l and intercept r, between x=0
       * and x=n. Returns the upper and lower closest
       * points. Implementation of Charrier and Buzer algorithm
       * (Discrete Applied Mathematics, 2009) 
       *
       */
      void convexHullApprox(Vector l, Integer r, Integer n, Point *inf, Point *sup);
      
      /**
       * Compute the left part of the upper and lower convex hulls of
       * the line of slope s, between x=0
       * and x=n. Returns the upper and lower closest points. 
       *
       */
      void convexHullApprox(Number s, Integer n, Point *inf, Point *sup);
      
      /**
       * Compute the left part of the upper and lower convex hulls of
       * the line of slope s, between x=0
       * and x=n. Returns the last two points computed.
       * Implementation of Charrier and Buzer algorithm
       * (Discrete Applied Mathematics, 2009)  
       *
       */
      void convexHullApproxTwoPoints(Vector l, Integer r, Integer n, Point *inf, Point *sup, Point *prevInf, Point *prevSup, bool inv);
      
      
      /**
       * Compute the left part of the upper and lower convex hulls of
       * the line of directional vector l, between x=0 and x=n. Returns
       * the last two points computed. Implementation of Har-Peled
       * algorithm (Computational Geometry: Theory and Applications, 1998)
       *
       */
      void convexHullHarPeled(Vector l, Integer n, Point *inf, Point *sup);
      
      /**
       * Compute the term following fp/fq in the Farey series of order n. 
       *
       */
      
      Point nextTermInFareySeriesEuclid(Integer fp, Integer fq, Integer n);
      

      
      /**
       * Compute the ray of highest slope in O(1) knowing the ray of smallest
       * slope and the order of the Farey fan
       * 
       */
       RayC rayOfHighestSlope(Integer p, Integer q, Integer r, Integer smallestSlope, Integer n);
      
       /**
	* Compute the ceil of the slope of the line through (f=p/q,r/q)
	* and point (a/b,mu/b) -  O(1)
	*
	*/
       Number slope(Integer p, Integer q, Integer r, Number a, Number b, Number mu);
       
       /**
	* Compute the ceil of the slope of the line through (f=p/q,r/q)
	* and floating-point point (alpha,beta) -  O(1)
	*
	*/
       Number slope(Integer p, Integer q, Integer r, Number alpha, Number beta);
       
      
       /**
	* Compute the position of the point (a/b,mu/b) with respect to a
	* ray r
	* Return BELOW, ABOVE or ONTO
	*/
       Position positionWrtRay(RayC r, Number a, Number b, Number mu);
       
       	       
       /**
	* Compute the position of the floating-point point(alpha,beta) with respect to a
	* ray r
	* Return BELOW, ABOVE or ONTO
	*/
       Position positionWrtRay(RayC r, Number alpha, Number beta);
       
       /**
	* Computes the ray of smallest slope emanating from the point (f=p/q,
	* r/q) using the knowledge of the next fraction g in the Farey Series.
	* Complexity O(1)
	*
	*/ 
      RayC smartRayOfSmallestSlope(Integer fp, Integer fq, Integer gp, Integer gq, Integer r); 
      
      
      /** 
       * 
       */
      Integer smartFirstDichotomy(Integer fp, Integer fq, Integer gp, Integer gq, Number a, Number b, Number mu, Integer n, bool *flagRayFound);
      
      Integer smartFirstDichotomy(Integer fp, Integer fq, Integer gp, Integer gq, Number alpha, Number beta, Integer n, bool *flagRayFound);
      
      
      
      /**
       * Compute the closest ray below the point (a/b,mu/b) passing
       * through the point (fp/fq,r/fq) in the Farey fan of order n
       *
       */

      RayC localizeRay(Integer fp, Integer fq, Integer gp, Integer gq, Integer r, Number a, Number b, Number mu,  Integer n);

      
      /**
       * Compute the closest ray below the point (alpha,beta) passing
       * through the point (fp/fq,r/fq) in the Farey fan of order n
       *
       */
      RayC localizeRay(Integer fp, Integer fq, Integer gp, Integer gq, Integer r, Number alpha, Number beta, Integer n);
      
      
      /**
       * Compute the ray passing through from (f=p/q,h/q) just above r
       * Complexity O(1)
       */
      
      RayC raySup(Integer fp, Integer fq, RayC r);
      
  
      /**
       * The two fractions f and g together with the ray r define a segment
       *  PQ. PQ is part of the lower boundary of exactly one cell of the
       *  FareyFan. This cell represents a DSS. This function computes the
       * vertex of the cell that represents the minimal characteristics
       *  of the DSS.
       * Optimized version of the algorithm presented in the paper
       *  "Walking in the Farey Fan to compute the characteristics of
       *  discrete straight line subsegment" (Isabelle Sivignon, DGCI
       *  2013, Springer LNCS 7749)
       * Complexity of nextTermInFareySeriesEuclid
       */
      void findSolutionWithoutFractions(Integer fp, Integer fq, Integer gp, Integer gq, RayC r, Integer n, Integer *resAlphaP, Integer *resAlphaQ, Integer *resBetaP, bool found);  // resBetaQ = resAlphaQ  
      
      /**
       * Corresponds to the algorithm of paper "Walking in the Farey Fan
       * to compute the characteristics of  discrete straight line
       * subsegment" (Isabelle Sivignon, DGCI 2013, Springer LNCS 7749)
       */
      void shortFindSolution(Integer fp, Integer fq, Integer gp, Integer gq, RayC r, Integer n, Integer *resAlphaP, Integer *resAlphaQ, Integer *resBetaP, bool found);  // resBetaQ = resAlphaQ  
      
      
      

      // ------------------------- Accessors ------------------------------
      
  public:
      
      /**
       * @return an Integer of value @a myA.
       */
      Integer getA() const;
      /**
       * @return an Integer of value @a myB.
       */
      Integer getB() const;
      /**
       * @return an Integer of value @a myMu.
       */
      Integer getMu() const;


  };
  
   
}// namespace DGtal



  //Includes inline functions.
#include "DSLSubsegment.ih"


//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#endif // !defined DSLSubsegment_h

#undef DSLSubsegment_RECURSES
#endif // else defined(DSLSubsegment_RECURSES)
