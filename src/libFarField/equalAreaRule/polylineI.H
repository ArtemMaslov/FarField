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

#include "polyline.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

scalar FarField::polyline::interpolate(scalar x) const
{
    return interpolate(x, 0, intervals_.size());
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

const DynamicList<FarField::polyline::orientPoint>&
FarField::polyline::getOrientPoints() const
{
    return orientPoints_;
}


const List<FarField::interval>& 
FarField::polyline::getIntervals() const
{
    return intervals_;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

scalar FarField::polyline::getXMin() const
{
    return xMin_;
}


scalar FarField::polyline::getXMax() const
{
    return xMax_;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

label FarField::polyline::getPrevIntIdx(label pointIdx) const
{
    // if (!(pointIdx > 0 && pointIdx <= augPoints.size() - 1))
    // {
    //     FatalErrorInFunction
    //         << "Assert failed pointIdx > 0 && pointIdx <= augPoints.size() - 1" << nl
    //         << "    pointIdx = " << pointIdx << nl
    //         << "    augPoints.size() = " << augPoints.size() << nl
    //         << exit(FatalError);
    // }

    return pointIdx - 1;
}


label FarField::polyline::getNextIntIdx(label pointIdx) const
{
    // if (!(pointIdx >= 0 && pointIdx < augPoints.size() - 1))
    // {
    //     FatalErrorInFunction
    //         << "Assert failed pointIdx >= 0 && pointIdx < augPoints.size() - 1" << nl
    //         << "    pointIdx = " << pointIdx << nl
    //         << "    augPoints.size() = " << augPoints.size() << nl
    //         << exit(FatalError);
    // }

    return pointIdx;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

label FarField::polyline::getPrevPointIdx(label intIdx) const
{
    // if (!(intIdx > 0 && intIdx <= intervals_.size() - 1))
    // {
    //     FatalErrorInFunction
    //         << "Assert failed intIdx > 0 && intIdx <= intervals_.size() - 1" << nl
    //         << "    intIdx = " << intIdx << nl
    //         << "    intervals_.size() = " << intervals_.size() << nl
    //         << exit(FatalError);
    // }

    return intIdx;
}


label FarField::polyline::getNextPointIdx(label intIdx) const
{
    // if (!(intIdx >= 0 && intIdx < intervals_.size() - 1))
    // {
    //     FatalErrorInFunction
    //         << "Assert failed intIdx >= 0 && intIdx < intervals_.size() - 1" << nl
    //         << "    intIdx = " << intIdx << nl
    //         << "    intervals_.size() = " << intervals_.size() << nl
    //         << exit(FatalError);
    // }

    return intIdx + 1;
}


// ************************************************************************* //
