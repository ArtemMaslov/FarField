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

#include "WhithamFarField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
//              Compute Whitham's function by relative pressure.
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template <typename T1, typename T2>
auto FarField::Whitham::computeY
(
    const T1& x,
    const T2& F,
    const scalar r
) const noexcept
{
    return x - betaCoefficient_*r + kappaCoefficient_*Foam::sqrt(r)*F;
}


template <typename T>
auto FarField::Whitham::computeF
(
    const T& dp_p,
    const scalar r
) const noexcept
{
    return 
        dp_p*Foam::sqrt(2*betaCoefficient_*r)
      / (heatCapacityRatio_*Foam::sqr(MachNumber_));
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template <typename T1, typename T2>
auto FarField::Whitham::computeX
(
    const T1& y,
    const T2& F,
    const scalar r
) const noexcept
{
    return y + betaCoefficient_*r - kappaCoefficient_*Foam::sqrt(r)*F;
}


template <typename T1, typename T2>
auto FarField::Whitham::computeP
(
    const T1& y,
    const T2& F,
    const scalar r,
    scalar p0,
    scalar p1
) const noexcept
{
    return
        F*heatCapacityRatio_*Foam::sqr(MachNumber_)
      / Foam::sqrt(2*betaCoefficient_*r);
}


// ************************************************************************* //
