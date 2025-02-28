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
#include "Ostream.H"
#include "error.H"
#include "floatScalar.H"
#include "stdFoam.H"
#include "csvWriter.H"

#include "polyline.H"

using namespace Foam;
using namespace Utils;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

FarField::polyline::polyline(pxDist& dist) :
    augPoints(),
    xMin_(VGREAT),
    xMax_(-VGREAT),
    intervals_(),
    orientPoints_()
{
    Info<< "polyline constructor." << endl;

    if (dist.size() < 2)
    {
        FatalErrorInFunction
            << "[FarField]  Linear interpolation requires more or equal than "
               "2 points."
            << exit(FatalError);
    }

    // Add helper points.
    initAugPoints(dist);

    // Calculate interpolation intervals.
    initIntervals();

    // Calculate orientation points.
    initOrientPoints();
}


void FarField::polyline::initAugPoints(pxDist& dist)
{
    // Calculate xMin and xMax.
    forAll(dist, i)
    {
        if (dist.x()[i] > xMax_)
            xMax_ = dist.x()[i];
        if (dist.x()[i] < xMin_)
            xMin_ = dist.x()[i];
    }

    const size_t augPointsCapacity = dist.size();

    // May be reserved more space, than needed.
    augPoints.reserve(augPointsCapacity);
    intervals_.reserve(augPointsCapacity - 1);

    // Check if helper points are already added.
    if (xMin_ < dist.x()[0])
    {
        FatalErrorInFunction
            << "Polyline cannot add left helper point.\n"
               "xMin     = " << xMin_ << "\n"
               "x[first] = " << dist.x()[0] << "\n"
            << exit(FatalError);
    }

    if (xMax_ > dist.x()[dist.size() - 1])
    {
        FatalErrorInFunction
            << "Polyline cannot add right helper point.\n"
               "xMax    = " << xMax_ << "\n"
               "x[last] = " << dist.x()[dist.size() - 1] << "\n"
            << exit(FatalError);
    }

    // Copy inner points.
    forAll(dist, i)
        augPoints.append({dist.x()[i], dist.p()[i]});
}


void FarField::polyline::initIntervals()
{
    const size_t intervalsCount = augPoints.size() - 1;
    for (size_t i = 0; i < intervalsCount; ++i)
    {
        intervals_.append
        ({
            augPoints[i    ].x,
            augPoints[i    ].y,
            augPoints[i + 1].x,
            augPoints[i + 1].y
        });
    }
}


void FarField::polyline::initOrientPoints()
{
    // The first point is always orientation point.
    orientPoints_.push_back({augPoints[0], 0});

    // Find inner orientation points.
    bool isDirectionToTheRight = true;
    const size_t lastPointIdx = augPoints.size() - 1;
    for (size_t i = 1; i < lastPointIdx; ++i)
    {
        // IF   direction is to the right and the next point is located
        //      to the left of the current one.
        // OR
        // IF   direction is to the left and the next point is located
        //      to the right of the current one.
        // THEN the current point is orientation.
        if ((isDirectionToTheRight && augPoints[i + 1].x < augPoints[i].x)
         || (!isDirectionToTheRight && augPoints[i + 1].x > augPoints[i].x))
        {
            orientPoints_.push_back({augPoints[i], i});
            isDirectionToTheRight = !isDirectionToTheRight;
        }
    }

    // The last point is always orientation point.
    orientPoints_.push_back
    (
        {augPoints[lastPointIdx], lastPointIdx}
    );

    #warning make function write orient points.

    csvWriter csv{{"shocks/orientPoints.csv"}};
    csv << "idx" << "x" << "y" << nr;

    forAll(orientPoints_, i)
    {
        csv << orientPoints_[i].index
            << orientPoints_[i].p.x
            << orientPoints_[i].p.y
            << nr;
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

label FarField::polyline::findIntersectIntIdxAsc
(
    const label startIntIdxIncl,
    const scalar x
) const
{
    for (label idx = startIntIdxIncl; idx < intervals_.size(); idx++)
    {
        if (intervals_[idx].containsX(x))
            return idx;
    }
    FatalErrorInFunction
        << "check logic." << exit(FatalError);
    return intervals_.size();
}


label FarField::polyline::findIntersectIntIdxDes
(
    const label startIntIdxIncl,
    const scalar x
) const
{
    for (label idx = startIntIdxIncl; idx >= 0; idx--)
    {
        if (intervals_[idx].containsX(x))
            return idx;
    }
    FatalErrorInFunction
        << "check logic." << exit(FatalError);
    return intervals_.size();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

scalar FarField::polyline::calcArea
(
    const label startIntIdxIncl,
    const label stopIntIdxIncl
) const
{
    scalar area = 0;
    for (label idx = startIntIdxIncl; idx <= stopIntIdxIncl; idx++)
        area += intervals_[idx].getArea();
    return area;
}


label FarField::polyline::findOrientPointIdx
(
    const label startOpIdx,
    const scalar xLine
) const
{
    for (label idx = startOpIdx; idx < orientPoints_.size() - 1; idx++)
    {
        if (orientPoints_[idx].p.x <= xLine
         && xLine <= orientPoints_[idx + 1].p.x)
        {
            return idx;
        }
    }

    FatalErrorInFunction
        << "check logic." << exit(FatalError);
    return orientPoints_.size();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::polyline::findShockPrevPointIdx
(
    const label shockStartOrientPoint,
    const scalar xLine,
    label& prevShockPointIdx,
    label& intersectShockIntIdx
) const
{
    label helperOrientPoint = shockStartOrientPoint;
    for (label i = helperOrientPoint; i >= 0; i--)
    {
        if (orientPoints_[i].p.x > xLine)
            helperOrientPoint = i;
        else
            break;
    }
    
    label startSearchPointIdx = orientPoints_[helperOrientPoint - 1].index;
    label startSearchIntIdx   = getNextIntIdx(startSearchPointIdx);

    // Summarize the result.
    intersectShockIntIdx = findIntersectIntIdxAsc(startSearchIntIdx, xLine);
    prevShockPointIdx    = getPrevPointIdx(intersectShockIntIdx);
}


void FarField::polyline::findShockNextPointIdx
(
    const label shockStopOrientPoint,
    const scalar xLine,
    label& nextShockPointIdx,
    label& intersectShockIntIdx
) const
{
    label helperOrientPoint = shockStopOrientPoint;
    for (label i = helperOrientPoint; i < orientPoints_.size(); i++)
    {
        if (orientPoints_[i].p.x < xLine)
            helperOrientPoint = i;
        else
            break;
    }
    
    label startSearchPointIdx = orientPoints_[helperOrientPoint].index;
    label startSearchIntIdx   = getNextIntIdx(startSearchPointIdx);

    // Summarize the result.
    intersectShockIntIdx = findIntersectIntIdxAsc(startSearchIntIdx, xLine);
    nextShockPointIdx    = getNextPointIdx(intersectShockIntIdx);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Find rightP1 and rightP2 points.
void FarField::polyline::findShockRightPoints
(
    const label shockStartOrientPoint,
    const scalar xLine,
    label& p1Idx,
    label& p2Idx,
    label& op1Idx
) const
{
    findShockRightP1(shockStartOrientPoint, xLine, p1Idx, op1Idx);
    p2Idx = findShockRightP2(shockStartOrientPoint, xLine);
}


void FarField::polyline::findShockRightP1
(
    const label shockStartOrientPoint,
    const scalar xLine,
    label& p1Idx,
    label& op1Idx
) const
{
    // To speed up the process find orientation point first.
    label opIdx = shockStartOrientPoint;
    while (orientPoints_[opIdx - 1].p.x >= xLine)
        opIdx--;
    op1Idx = opIdx;
    
    label pIdx = orientPoints_[opIdx].index;
    while (augPoints[pIdx - 1].x >= xLine)
        pIdx--;
    p1Idx = pIdx;
}


label FarField::polyline::findShockRightP2
(
    const label shockStartOrientPoint,
    const scalar xLine
) const
{
    label p2Idx = orientPoints_[shockStartOrientPoint].index;
    while (augPoints[p2Idx + 1].x >= xLine)
        p2Idx++;
    return p2Idx;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Find leftP1 and leftP2 points.
void FarField::polyline::findShockLeftPoints
(
    const label shockStopOrientPoint,
    const scalar xLine,
    label& p1Idx,
    label& p2Idx,
    label& op2Idx
) const
{
    p1Idx = findShockLeftP1(shockStopOrientPoint, xLine);
    findShockLeftP2(shockStopOrientPoint, xLine, p2Idx, op2Idx);
}


label FarField::polyline::findShockLeftP1
(
    const label shockStopOrientPoint,
    const scalar xLine
) const
{
    label p1Idx = orientPoints_[shockStopOrientPoint].index;
    while (augPoints[p1Idx - 1].x <= xLine)
        p1Idx--;
    return p1Idx;
}


void FarField::polyline::findShockLeftP2
(
    const label shockStopOrientPoint,
    const scalar xLine,
    label& p2Idx,
    label& op2Idx
) const
{
    // To speed up the process find orientation point first.
    label opIdx = shockStopOrientPoint;
    while (orientPoints_[opIdx + 1].p.x <= xLine)
        opIdx++;
    op2Idx = opIdx;

    label pIdx = orientPoints_[opIdx].index;
    while (augPoints[pIdx + 1].x <= xLine)
        pIdx++;
    p2Idx = pIdx;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

scalar FarField::polyline::interpolate
(
    const scalar x, 
    const label startIntIndexIncl,
    const label stopIntIndexIncl
) const
{
    assert(intervals_.front().x1 <= x && x <= intervals_.back().x2);

    const label startIdx = max(startIntIndexIncl, 0);
    const label stopIdx  = min(stopIntIndexIncl, intervals_.size());

    for (label st = startIdx; st < stopIdx; st++)
    {
        if (intervals_[st].containsX(x))
            return intervals_[st].interpolate(x);
    }

    FatalErrorInFunction
        << "[FarField] Linear interpolation doesn't contain point x = " 
        << x << "." << exit(FatalError);
    return 0;
}


// ************************************************************************* //