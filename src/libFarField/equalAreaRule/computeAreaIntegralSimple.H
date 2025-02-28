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

#ifndef FarField_computeAreaIntegralSimple_H
#define FarField_computeAreaIntegralSimple_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "polyline.H"
#include "shock.H"

namespace FarField
{

using namespace Foam;

/*---------------------------------------------------------------------------*\
                     Class computeAreaIntegral Declaration
\*---------------------------------------------------------------------------*/

class computeAreaIntegral
{
private:
    const polyline& polyline_;
    const scalar    xLine_;
    const label     startOrientPoint_;
          shock     outShock_;

private:
    scalar calcFigureArea
    (
        const label startIntIdxIncl,
        const label stopIntIdxIncl,
        const label nextOrientPointIdx,
        const label stopOrientPointIdx
    );

    scalar calcRightFigureArea();
    scalar calcLeftFigureArea();

    scalar integrateHalfRight(const label intersectIntIdx);
    scalar integrateHalfLeft(const label intersectIntIdx);
    
public:
    computeAreaIntegral
    (
        const polyline& polyline,
        const scalar xLine,
        const label startOrientPoint
    );

    void compute
    (
        scalar& outRightArea,
        scalar& outLeftArea,
        shock&  outShock
    );
}; // End class computeAreaIntegral

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace FarField

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // end define FarField_computeAreaIntegralSimple_H

// ************************************************************************* //