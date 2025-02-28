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

#include <cassert>

#include "computeAreaIntegralSimple.H"
#include "scalarFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

FarField::computeAreaIntegral::computeAreaIntegral
(
    const polyline& polyline,
    const scalar xLine,
    const label startOrientPoint
)
:
    polyline_(polyline),
    xLine_(xLine),
    startOrientPoint_(startOrientPoint),
    outShock_()
{
    assert(startOrientPoint_ >= 1);
    outShock_.xLine = xLine_;
}


void FarField::computeAreaIntegral::compute
(
    scalar& outRightArea,
    scalar& outLeftArea,
    shock&  outShock
)
{
    // Right figure.
    outRightArea = calcRightFigureArea();
    
    // Left figure.
    outLeftArea = calcLeftFigureArea();

    outShock = outShock_;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

scalar FarField::computeAreaIntegral::calcRightFigureArea()
{
    auto& ints = polyline_.getIntervals();

    label p1Idx  = 0;
    label p2Idx  = 0;
    label op1Idx = 0;
    label op2Idx = startOrientPoint_;
    polyline_.findShockRightPoints
    (
        startOrientPoint_,
        xLine_,
        p1Idx,
        p2Idx,
        op1Idx
    );
    outShock_.rightP1Idx = p1Idx;
    outShock_.rightP2Idx = p2Idx;

    // Half interval near P1.
    const label intersectInt1Idx = polyline_.getPrevIntIdx(p1Idx);
    scalar minusHalfArea = integrateHalfRight(intersectInt1Idx);
    outShock_.y1 = ints[intersectInt1Idx].interpolate(xLine_);

    // Half interval near P2.
    const label intersectInt2Idx = polyline_.getNextIntIdx(p2Idx);
    scalar plusHalfArea = integrateHalfRight(intersectInt2Idx);

    // Calculate figure area.
    scalar area = 
        calcFigureArea
        (
            intersectInt1Idx + 1,
            intersectInt2Idx - 1,
            op1Idx,
            op2Idx
        );
    
    return area + plusHalfArea - minusHalfArea;
}


scalar FarField::computeAreaIntegral::calcLeftFigureArea()
{
    auto& ints = polyline_.getIntervals();

    label stopOrientPoint = startOrientPoint_ + 1;
    label p1Idx  = 0;
    label p2Idx  = 0;
    label op1Idx = stopOrientPoint;
    label op2Idx = 0;
    polyline_.findShockLeftPoints
    (
        stopOrientPoint,
        xLine_,
        p1Idx,
        p2Idx,
        op2Idx
    );
    outShock_.leftP1Idx = p1Idx;
    outShock_.leftP2Idx = p2Idx;

    // Half interval near P1.
    const label intersectInt1Idx = polyline_.getPrevIntIdx(p1Idx);
    scalar minusHalfArea = integrateHalfLeft(intersectInt1Idx);

    // Half interval near P2.
    const label intersectInt2Idx = polyline_.getNextIntIdx(p2Idx);
    scalar plusHalfArea = integrateHalfLeft(intersectInt2Idx);
    outShock_.y2 = ints[intersectInt2Idx].interpolate(xLine_);

    // Calculate figure area.
    scalar area = 
        calcFigureArea
        (
            intersectInt1Idx + 1,
            intersectInt2Idx - 1,
            op1Idx,
            op2Idx
        );
    
    return area + plusHalfArea - minusHalfArea;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Foam::scalar FarField::computeAreaIntegral::calcFigureArea
(
    label const startIntIdxIncl,
    label const stopIntIdxIncl,
    label const nextOrientPointIdx,
    label const stopOrientPointIdx
)
{
    auto& ops = polyline_.getOrientPoints();

    scalar area = 0;
    scalar sign = -1;

    label startIntIdx = startIntIdxIncl;

    for
    (
        label orientPointIdx = nextOrientPointIdx;
        orientPointIdx <= stopOrientPointIdx;
        orientPointIdx++
    )
    {
        label orientPointIdxInPoints = ops[orientPointIdx].index;
        label stopIntIdx = polyline_.getPrevIntIdx(orientPointIdxInPoints);

        area += sign * polyline_.calcArea(startIntIdx, stopIntIdx);
        sign = -sign;

        startIntIdx = polyline_.getNextIntIdx(orientPointIdxInPoints);
    }

    area += sign * polyline_.calcArea(startIntIdx, stopIntIdxIncl);

    return area;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Foam::scalar FarField::computeAreaIntegral::integrateHalfRight
(
    const label intersectIntIdx
)
{
    const interval& intersectInt = polyline_.getIntervals()[intersectIntIdx];
    interval halfInt
    {
        xLine_,
        intersectInt.interpolate(xLine_),
        intersectInt.x2,
        intersectInt.calcY2()
    };
    return halfInt.getArea();
}


Foam::scalar FarField::computeAreaIntegral::integrateHalfLeft
(
    const label intersectIntIdx
)
{
    const interval& intersectInt = polyline_.getIntervals()[intersectIntIdx];
    interval halfInt
    {
        intersectInt.x1,
        intersectInt.calcY1(),
        xLine_,
        intersectInt.interpolate(xLine_)
    };
    return halfInt.getArea();
}


// ************************************************************************* //