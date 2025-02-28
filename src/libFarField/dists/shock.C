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

#include "csvWriter.H"

#include "shock.H"

using namespace Utils;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// FarField::shock::shock()
// {
// }

// FarField::shock::shock
// (
//     scalar xLine,
//     fpoint shockPoint1,
//     fpoint shockPoint2,
//     label  prevPointIdx,
//     label  nextPointIdx,
//     bool   removed
// )
// :
//     xLine(xLine),
//     shockPoint1(shockPoint1),
//     shockPoint2(shockPoint2),
//     prevPointIdx(prevPointIdx),
//     nextPointIdx(nextPointIdx),
//     removed(removed)
// {
// }


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::writeShocks
(
    const DynamicList<shock>& shocks,
    const Files::shocksFile& opts
)
{
    csvWriter csv{{opts.path}};

    csv << "xLine"
        << "y1"
        << "y2"
        << "leftP1Idx"
        << "leftP2Idx"
        << "rightP1Idx"
        << "rightP2Idx"
        << "removed"
        << nr;

    forAll(shocks, i)
    {
        csv << shocks[i].xLine
            << shocks[i].y1
            << shocks[i].y2
            << shocks[i].leftP1Idx
            << shocks[i].leftP2Idx
            << shocks[i].rightP1Idx
            << shocks[i].rightP2Idx
            << shocks[i].removed
            << nr;
    }
}

// ************************************************************************* //