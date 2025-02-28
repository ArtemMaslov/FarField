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

#ifndef FarField_shock_H
#define FarField_shock_H

#include "fpoint.H"
#include "DynamicList.H"

#include "files.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace FarField
{

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
//                       Shock named-points definition
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

//   26     27     28      29       30
//    *-----*------*-------*--------*
//     \*         P2  |
//    25 \-* 24       |
//          \-   left |
//     22     \-figure|
//       *-----* 23   |
//         \-*        |
//          21\--  20 |
//                \*--|     19
//                P1  |\---*     18
//                    |  P2 \---*  17
//                    |          \*---* 16
//                    |  11 *-\* 12    \-
//                    |      \- \--\    \-
//                    |       \-    *--*-\*
//                    |        * 10 13 14  15
//                    |         \-  9
//                    |  right    \*
//                    |  figure     \8  7
//                    |              *-*
//  0          1    2 | 3   4     5     \- 6
//  *----------*----*---*---*-----*-------*
//                      P1

// * - points of polyline, they are indexed 0-30.
// Orientation points: 0, 6, 11, 15, 22, 23, 26, 30.

// 15 - shock start orient point.
// 22 - shock stop  orient point.

// Right figure:
//     rightP1Idx  = 3
//     rightP2Idx  = 19

//     rightOP1Idx = 6
//     rightOP2Idx = 15

// Left figure:
//     leftP1Idx   = 20
//     leftP2Idx   = 28

//     leftOP1Idx  = 22
//     leftOP2Idx  = 26 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

struct shock
{
    scalar xLine      = 0;
    scalar y1         = 0;
    scalar y2         = 0;
    label  leftP1Idx  = 0;
    label  leftP2Idx  = 0;
    label  rightP1Idx = 0;
    label  rightP2Idx = 0;
    bool   removed    = false;
};

void writeShocks
(
    const DynamicList<shock>& shocks,
    const Files::shocksFile& opts
);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace FarField

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // end define FarField_shock_H

// ************************************************************************* //