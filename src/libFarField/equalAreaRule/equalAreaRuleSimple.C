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

#include "DynamicList.H"
#include "dists/shock.H"
#include "interval.H"
#include "label.H"
#include "labelFwd.H"
#include "scalar.H"

#include "equalAreaRuleSimple.H"
#include "computeAreaIntegralSimple.H"
#include "polyline.H"
#include "shock.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

static void CopyRange
(
    const List<FarField::fpoint>& src,
    DynamicList<FarField::fpoint>& dst,
    const label startIdx,
    const label stopIdx
);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

FarField::equalAreaRuleSimple::equalAreaRuleSimple
(
    const polyline& polyline,
    label stepsCount
)
:
    polyline_(polyline),
    stepsCount_(stepsCount)
{}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::equalAreaRuleSimple::apply
(
    DynamicList<fpoint>& outPDist,
    DynamicList<shock>& outShocks
)
{
    Info<< "Using SimpleEqualArea rule and AreaIntegralSimple" << endl;
    
    outPDist.clear();
    outShocks.clear();

    findShocks(outShocks);

    // Check if there is any shock.
    if (outShocks.size() == 0)
    {
        // There are no shocks. Copy pressure distribution.
        CopyRange
        (
            polyline_.augPoints,
            outPDist, 
            0,
            polyline_.augPoints.size() - 1
        );
        return;
    }

    removeShocks(outShocks);

    DynamicList<shock> remainingShocks {outShocks.size()};
    forAll(outShocks, i)
    {
        if (!outShocks[i].removed)
            remainingShocks.append(outShocks[i]);
    }
    checkShocks(remainingShocks);

    placeShocks(outPDist, outShocks);
    checkDist(outPDist);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::equalAreaRuleSimple::findShocks(DynamicList<shock>& shocks)
{
    // Point with index = 0 is the first orientation point and the start of
    // polyline.
    // Point with index = 1 is the first meaningful orientation point.
    // Point with index = size() - 2 is the last meaningful orientation point.
    // Point with index = size() - 1 is the last orientation point and the end
    // of polyline.

    auto& ops = polyline_.getOrientPoints();

    label pointIndex = 1;
    //label shockIdx = 1;
    
    while (pointIndex < ops.size() - 2)
    {
        //Info<< "shock #" << shockIdx << ':' << endl;

        addShock(shocks, pointIndex);
        pointIndex += 2;
        //shockIdx++;
    }
}


void FarField::equalAreaRuleSimple::addShock
(
    DynamicList<shock>& shocks,
    label startOrientPoint
)
{
    // Possible pressure profile with orientation points:
    //
    //      shock wave line/boundary
    //               |
    //          /----|----\-             ...
    //      nxt1  Sl |      \----          \-
    //         \---- |           \---        \-
    //              \|---\           \------nxt2
    //               | Sr \-
    // ... prev -----|----curr
    //               |
    //             xLine
    //
    // Sl - area left from the line.
    // Sr - area right from the line.
    // Equal area rule: Sl must be equal to Sr.
    // prev, curr, nxt1, nxt2 - orientation points.

    auto& ops = polyline_.getOrientPoints();

    const polyline::orientPoint& currPoint = ops[startOrientPoint    ];
    const polyline::orientPoint& nxt1Point = ops[startOrientPoint + 1];

    const scalar x1 = nxt1Point.p.x;
    const scalar x2 = currPoint.p.x;
    const scalar xStep = (x2 - x1) / stepsCount_;
    scalar xLine = x2 - xStep;

    shock  bestShock {};
    scalar bestAreaDelta = VGREAT;

    //Info<< "    x1 = " << x1 << nl
    //    << "    x2 = " << x2 << nl << endl;

    // label iterIdx = 1;

    while (xLine > x1)
    {
        //Info<< "    iter #" << iterIdx << endl;

        computeAreaIntegral area {polyline_, xLine, startOrientPoint};

        scalar rightArea = 0;
        scalar leftArea  = 0;
        shock  currShock {};
        area.compute(rightArea, leftArea, currShock);

        scalar areaDelta = mag(rightArea - leftArea);

        // Info<< "    xLine      = " << xLine << nl
        //     << "    left area  = " << leftArea << nl
        //     << "    right area = " << rightArea << nl
        //     << "    delta      = " << areaDelta << nl << endl;

        if (areaDelta < bestAreaDelta)
        {
            bestShock     = currShock;
            bestAreaDelta = areaDelta;
        }

        xLine -= xStep;
        // iterIdx++;
    }

    shocks.append(bestShock);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::equalAreaRuleSimple::removeShocks(DynamicList<shock>& shocks)
{
    // Remove shocks, which are overlapped by another shock.
    for (label currIdx = 1; currIdx < shocks.size(); currIdx++)
    {
        // Remove only one shock for each iteration.
        // So after the shock have been removed, need to check if remaining 
        // shocks are good.
        scalar currShockX = shocks[currIdx].xLine;
        for (label prevIdx = currIdx - 1; prevIdx >= 0; prevIdx--)
        {            
            scalar prevShockX = shocks[prevIdx].xLine;

            // Skip already removed shocks.
            if (shocks[prevIdx].removed)
                continue;

            // Check if all previous shocks are good.
            if (currShockX > prevShockX
            && !equal(currShockX, prevShockX))
                break; // All previous shocks are good.
            
            // Shocks overlap.

            // Are shocks have same x coordinate.
            if (equal(currShockX, prevShockX))
            {
                // Merge shocks.
                // Extend the second shock to cover the first one.
                shocks[currIdx].y1 = shocks[prevIdx].y1;
                shocks[currIdx].rightP1Idx = shocks[prevIdx].rightP1Idx;
                // Remove the first shock.
                shocks[prevIdx].removed = true;
                continue;
            }
            
            // Pressure distribution with main and mini shocks.
            //    -------------------------------- ...
            //     \-  |          |
            //       \-| <------------------------------------------ left miniShock
            //         |\-        |
            //         |  \-      |
            //       \-----       |
            //         \-         |
            //            \--     | <----------------------------------- main shock
            //                \---|
            //                    |\----
            //                    |     \----
            //                    |          \-----
            //                    |     \--\      \-
            //                    |      \   |-    \-
            //                    |       \  |  \---\-
            //                    |        \ |
            //                    |         \| <------------------- right miniShock
            //                    |          |
            //                    |          |\-
            //                    |          |  \--
            //                    |          |     \- 
            //  ... ----------------------------------
            //
            // mini shocks have to be removed due to existence of main shock.

            if (removeLeftMiniShock(shocks, currIdx, prevIdx))
                break; // Current shock is removed.

            if (removeRightMiniShock(shocks, currIdx, prevIdx))
                continue; // Previous shock is removed.

            // Then remove overlapping main shocks.
            if (removeOverlappingMainShocks(shocks, currIdx, prevIdx))
                continue; // Previous shock is removed.
        }
    }
}


bool FarField::equalAreaRuleSimple::removeLeftMiniShock
(
    DynamicList<shock>& shocks,
    const label currIdx,
    const label prevIdx
)
{
    // In case left mini shock:
    // current shock could be mini shock
    // previous is main shock

    // Current shock is mini shock <=> Current shock is included in 
    //                                 previous one.
    if
    (
      !(shocks[prevIdx].leftP1Idx <= shocks[currIdx].leftP1Idx
     && shocks[prevIdx].leftP2Idx >= shocks[currIdx].leftP2Idx)
    )
        return false;

    // Just remove mini shock. Do nothing with main shock.
    shocks[currIdx].removed = true;

    return true;
}

bool FarField::equalAreaRuleSimple::removeRightMiniShock
(
    DynamicList<shock>& shocks,
    const label currIdx,
    const label prevIdx
)
{
    // In case right mini shock:
    // current shock is main shock
    // previous could be mini shock

    // Previous shock is mini shock <=> Previous shock is included in 
    //                                  current one.
    if
    (
      !(shocks[currIdx].leftP1Idx <= shocks[prevIdx].leftP1Idx
     && shocks[currIdx].leftP2Idx >= shocks[prevIdx].leftP2Idx)
    )
        return false;

    // Just remove mini shock. Do nothing with main shock.
    shocks[prevIdx].removed = true;

    return true;
}

bool FarField::equalAreaRuleSimple::removeOverlappingMainShocks
(
    DynamicList<shock>& shocks,
    const label currIdx,
    const label prevIdx
)
{
    auto& ints = polyline_.getIntervals();

    scalar currXLine       = shocks[currIdx].xLine;
    label  startIntIdx     = 
        polyline_.getPrevIntIdx(shocks[prevIdx].rightP1Idx);
    label  intersectIntIdx =
        polyline_.findIntersectIntIdxDes
        (
            startIntIdx,
            currXLine
        );

    // Extend the second shock to cover the first one.
    shocks[currIdx].y1 = ints[intersectIntIdx].interpolate(currXLine);
    shocks[currIdx].rightP1Idx = polyline_.getNextPointIdx(intersectIntIdx);
    // Remove the first shock.
    shocks[prevIdx].removed = true;

    return true;

    // const scalar currShockX = shocks[shockIdx].xLine;

    // label prevShockIdx = shockIdx - 1;
    // while (prevShockIdx >= 0)
    // {
    //     const scalar prevShockX = shocks[prevShockIdx].xLine;
    //     // If current shock overlap previous one.
    //     if (prevShockX > currShockX)
    //     {
    //         // Extend current shock.

    //         // Remove previous shock.
    //         shocks[prevShockIdx].removed = true;
    //     }
    //     // Or if they have the same x coordinate.
    //     else if (equal(currShockX, prevShockX))
    //     {
    //         // Merge shocks.
    //         // Extend current shock to cover previous shock.
    //         shocks[shockIdx].y1 = shocks[prevShockIdx].y1;

    //         // Remove previous shock.
    //         shocks[prevShockIdx].removed = true;
    //     }
    //     // Else all remaining previous shocks are ok.
    //     else
    //         break;

    //     prevShockIdx--;
    // }

    // return true;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::equalAreaRuleSimple::placeShocks
(
    DynamicList<fpoint>& pDist,
    DynamicList<shock>& shocks
)
{
    // Place shocks in pressure distribution.
    
    label startPointIdx = 0;
    forAll(shocks, shockIdx)
    {
        const shock& sh = shocks[shockIdx];

        if (sh.removed)
            continue;

        label stopPointIdx = sh.rightP1Idx - 1;
        CopyRange
        (
            polyline_.augPoints,
            pDist, 
            startPointIdx,
            stopPointIdx
        );

        // Add shock points:
        pDist.append({sh.xLine, sh.y1});
        pDist.append({sh.xLine, sh.y2});
        startPointIdx = sh.leftP2Idx + 1;
    }
    // Copy points after the last shock.
    CopyRange
    (
        polyline_.augPoints,
        pDist, 
        startPointIdx,
        polyline_.augPoints.size() - 1
    );
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

static void CopyRange
(
    const List<FarField::fpoint>& src,
    DynamicList<FarField::fpoint>& dst,
    const label startIdx,
    const label stopIdx
)
{
    for (label idx = startIdx; idx <= stopIdx; idx++)
        dst.append(src[idx]);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

bool FarField::equalAreaRuleSimple::checkShocks
(
    const DynamicList<shock>& shocks
)
{
    bool good = true;

    for(label i = 1; i < shocks.size(); i++)
    {
        scalar currX = shocks[i  ].xLine;
        scalar prevX = shocks[i-1].xLine;
        if (currX < prevX ||
            equal(currX, prevX))
        {
            Info<< "Shocks are bad: shocks[i].x <= shocks[i-1].x\n"
                   "    i             = " << i << "\n"
                   "    shocks[i  ].x = " << shocks[i  ].xLine << "\n"
                   "    shocks[i-1].x = " << shocks[i-1].xLine << "\n"
                << endl;
            good = false;
        }
    }
    return good;
}


bool FarField::equalAreaRuleSimple::checkDist(const DynamicList<fpoint>& dist)
{
    bool good = true;

    for (label i = 1; i < dist.size(); i++)
    {
        if (dist[i].x < dist[i-1].x)
        {
            Info<< "Bad dist: dist[i].x < dist[i-1].x\n"
                   "    i           = " << i << "\n"
                   "    dist[i  ].x = " << dist[i].x << "\n"
                   "    dist[i-1].x = " << dist[i].x << "\n"
                << endl;
            good = false;
        }
    }
    return good;
}


// ************************************************************************* //