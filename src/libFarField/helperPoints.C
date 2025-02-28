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

#include "point2D.H"

#include "interval.H"
#include "helperPoints.H"

using namespace Foam;
using namespace FarField;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

struct details
{
    scalar xMin;
    scalar xMax;

    point2D pMin;
    point2D pMax;

    helperPoints& hp;

    scalar deltaMinX;
    scalar deltaMaxX;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

static void findMinMax(details& d);
static void evaluateXMinMax(details& d);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::helperPoints::addHelperPoints()
{
    details d
    {
        .xMin = +VGREAT,
        .xMax = -VGREAT,
        .pMin =
        {
            0,
            +VGREAT
        },
        .pMax =
        {
            0,
            -VGREAT
        },
        .hp = *this,
        .deltaMinX = 0,
        .deltaMaxX = 0
    };

    findMinMax(d);
    evaluateXMinMax(d);

    // +1 for the left helper point.
    // +2 for the right helper points.
    outPx.reserve(p0x.size() + 3);

    // We will not interpolate pressure to zero from the left side.
    // But we will interpolate it from the right side.
    // Physical meaning of this interpolation: pressure disturbance goes to
    // zero after the aircraft has flown.

    // Add left helper point.
    outPx.append({d.xMin - d.deltaMinX *100, 0});

    // To interpolate pressure to zero from the right side, we need to find
    // the last inner point.
    size_t lastInnerPoint = p0x.size() - 1;
    scalar xZero = 0;
    while (lastInnerPoint >= 1)
    {
        const interval interval
        {
            p0x.x()[lastInnerPoint - 1],
            p0x.p()[lastInnerPoint - 1],
            p0x.x()[lastInnerPoint],
            p0x.p()[lastInnerPoint],
        };

        // IF the line containing the interval intersects the zero line to
        // the right of the interval,
        // THEN the left point of the interval is the last inner point.
        xZero = interval.calcPointX(0);
        if (xZero > interval.x1)
        {
            lastInnerPoint--;
            break;
        }

        lastInnerPoint--;
    }

    // Copy inner points.
    for (size_t i = 0; i <= lastInnerPoint; i++)
        outPx.append({p0x.x()[i], p0x.p()[i]});

    // Add back helper points.
    outPx.append({xZero, 0});
    outPx.append({max(d.xMax, xZero) + d.deltaMaxX*100, 0});
    
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

static void findMinMax(details& d)
{
    forAll(d.hp.p0x, i)
    {
        scalar x = d.hp.p0x.x()[i];
        scalar p = d.hp.p0x.p()[i];

        // Find xMax.
        if (x > d.xMax)
            d.xMax = x;
        // Find xMin.
        if (x < d.xMin)
            d.xMin = x;

        // Find pMin.
        if (p < d.pMin[1])
            d.pMin = {x, p};
        // Find pMax.
        if (p > d.pMax[1])
            d.pMax = {x, p};
    }

    Info<< "xMax = " << d.xMax << "\n"
           "xMin = " << d.xMin << "\n"
           "pMax = " << d.pMax[1] << "\n"
           "pMin = " << d.pMin[1]
        << endl;
}


static void evaluateXMinMax(details& d)
{
    const scalar deltaCoef =
        d.hp.wh.kappaCoefficient_*(Foam::sqrt(d.hp.r1) - Foam::sqrt(d.hp.r0))
      * Foam::sqrt(2 * d.hp.wh.betaCoefficient_ * d.hp.r0)
      / (d.hp.wh.heatCapacityRatio_ * Foam::sqr(d.hp.wh.MachNumber_));

    d.deltaMaxX = -deltaCoef * d.pMin[1];
    d.deltaMinX = deltaCoef * d.pMax[1];
    if (d.deltaMaxX < 0 || d.deltaMinX < 0)
    {
        FatalErrorInFunction
            << "Logic error" << exit(FatalError);
    }
}


// ************************************************************************* //
