/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2025 ISP RAS (www.ispras.ru) UniCFD Group (www.unicfd.ru)
-------------------------------------------------------------------------------
License
    This file is part of FarField library based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#ifndef FarField_polyline_H
#define FarField_polyline_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "List.H"
#include "DynamicList.H"

#include "pxDist.H"
#include "fpoint.H"
#include "interval.H"

namespace FarField
{

using namespace Foam;

/*---------------------------------------------------------------------------*\
                         Class polyline Declaration
\*---------------------------------------------------------------------------*/

class polyline
{
public:

    class orientPoint
    {
    public:
        fpoint p;
        // Index of orientation point in polyline points list.
        size_t  index;

        friend Ostream& operator<<
        (
            Ostream& os, 
            const polyline::orientPoint& value
        );
    };


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

public:
    //- Points of pressure distribution augmented with helper points.
    DynamicList<fpoint> augPoints;

private:

    scalar xMin_;
    scalar xMax_;

    //- List of linearly interpolated intervals.
    //  Interval contains linear coefficients and it's x-boundaries.
    DynamicList<interval> intervals_;

    //- List of orientation points.
    //  Polyline has right direction, when x2 >= x1 for current interval.
    //  Polyline has left direction, when x2 < x1 for current interval.
    //  When direction changes, when point called orientation.
    //  
    //  Used to speed up equal area rule, which is used for placing shock waves
    //  boundaries.
    DynamicList<orientPoint> orientPoints_;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

public:

    polyline(pxDist& dist);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    // return y of interpolated point.
    inline scalar interpolate(scalar xLine) const;

    // return y of interpolated point.
    scalar interpolate
    (
        const scalar xLine,
        const label startIntIndexIncl,
        const label stopIntIndexIncl
    ) const;

    // Find index of interval, which is intersected by polyline
    // starting from `startIntIdxIncl` interval in ascending order.
    label findIntersectIntIdxAsc
    (
        const label startIntIdxIncl,
        const scalar xLine
    ) const;

    // Find index of interval, which is intersected by polyline
    // starting from `startIntIdxIncl` interval in descending order.
    label findIntersectIntIdxDes
    (
        const label startIntIdxIncl,
        const scalar xLine
    ) const;

    // Calculate area under polyline from `startIntIdxIncl` to `stopIntIdxIncl`
    // intervals inclusively. Function just sum up area under intervals, so
    // function cannot be applied to multi-valued part of polyline.
    scalar calcArea
    (
        const label startIntIdxIncl,
        const label stopIntIdxIncl
    ) const;

    // Find index of orientation point for which is true:
    // orientPoint.x <= xLine <= orientPoint.x
    // starting from `startOpIdx` orientation point.
    label findOrientPointIdx
    (
        const label startOpIdx,
        const scalar xLine
    ) const;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    //         D
    //      |  v
    // A-**-|--*--*
    //  \ F |
    //    \-|-
    //      | \*
    //      | G \-
    // ---*-|-*-*-B
    //    ^ |
    //    C
    
    // *    - points of polyline.
    // B    - shock start orient point.
    // A    - shock stop orient point.
    // C    - shock prev point. The last point of polyline before shock.
    // D    - shock next point. The first point of polyline after shock.
    // F, G - regions, which are cut off by shock from polyline.

    // Regions F and G can also contain orientation points and shocks, which
    // are called 'mini shocks'.
    // In this case A is the first orientation point of polyline, which is to 
    // the left of the shock.
    // And B is the last orientation point of polyline, which is to the right
    // of shock.

    // Find shock previous point considering mini shocks.
    void findShockPrevPointIdx
    (
        const label shockStartOrientPoint,
        const scalar xLine,
        label& prevShockPointIdx,
        label& intersectShockIntIdx
    ) const;

    // Find shock next point considering mini shocks.
    void findShockNextPointIdx
    (
        const label shockStopOrientPoint,
        const scalar xLine,
        label& nextShockPointIdx,
        label& intersectShockIntIdx
    ) const;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    // Find rightP1 and rightP2 points.
    void findShockRightPoints
    (
        const label shockStartOrientPoint,
        const scalar xLine,
        label& p1Idx,
        label& p2Idx,
        label& op1Idx
    ) const;

    void findShockRightP1
    (
        const label shockStartOrientPoint,
        const scalar xLine,
        label& p1Idx,
        label& op1Idx
    ) const;

    label findShockRightP2
    (
        const label shockStartOrientPoint,
        const scalar xLine
    ) const;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    // Find leftP1 and leftP2 points.
    void findShockLeftPoints
    (
        const label shockStopOrientPoint,
        const scalar xLine,
        label& p1Idx,
        label& p2Idx,
        label& op2Idx
    ) const;

    label findShockLeftP1
    (
        const label shockStopOrientPoint,
        const scalar xLine
    ) const;

    void findShockLeftP2
    (
        const label shockStopOrientPoint,
        const scalar xLine,
        label& p2Idx,
        label& op2Idx
    ) const;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    inline const DynamicList<orientPoint>& getOrientPoints() const;
    inline const List<interval>& getIntervals() const;

    inline scalar getXMin() const;
    inline scalar getXMax() const;

    inline label getPrevIntIdx(label pointIdx) const;
    inline label getNextIntIdx(label pointIdx) const;

    inline label getPrevPointIdx(label intIdx) const;
    inline label getNextPointIdx(label intIdx) const;

private:
    void initAugPoints(pxDist& dist);
    void initIntervals();
    void initOrientPoints();

}; // End class polyline


Ostream& operator<<
(
    Ostream& os, 
    const polyline::orientPoint& value
);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace FarField

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "polylineI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // end define FarField_polyline_H

// ************************************************************************* //
