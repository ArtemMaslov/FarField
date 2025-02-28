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

#include "interval.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

FarField::interval::interval() :
    x1(0),
    x2(0),
    a_(0),
    b_(0),
    area_(0)
{
}

FarField::interval::interval
(
    scalar inX1,
    scalar inY1,
    scalar inX2,
    scalar inY2
) noexcept
:
    x1(Foam::min(inX1, inX2)),
    x2(Foam::max(inX1, inX2)),
    a_(0),
    b_(0),
    area_(0)
{
    if (inX1 == inX2)
    {
        Info<< "[FarField warning] trivial interval x2 == x1." << nl
            << "    x  = " << inX1 << nl
            << "    y1 = " << inY1 << nl
            << "    y2 = " << inY2 << nl << endl;
        a_ = 0;
        b_ = (inY1 + inY2) / 2;
    }
    else
    {
        a_    = (inY2 - inY1)/(inX2 - inX1);
        b_    = inY2 - a_*inX2;
        // Formula for area_ is correct, if:
        //     y1, y2 > 0;
        //     y1, y2 < 0;
        //     y1 > 0, y2 < 0 and vice versa. 
        area_ = (inY1 + inY2)*(this->x2 - this->x1)/2;
    }
}


bool FarField::interval::containsX
(
    scalar x
) const noexcept
{
    return (x1 <= x && x < x2);
}


scalar FarField::interval::interpolate
(
    scalar x
) const noexcept
{
    return a_*x + b_;
}


scalar FarField::interval::getArea() const noexcept
{
    return area_;
}


scalar FarField::interval::calcInnerPointX(scalar y) const
{
    if (a_ == 0)
    {
        FatalErrorInFunction
            << "Division on zero" << exit(FatalError);
    }
    scalar x = (y - b_)/a_;
    if (!containsX(x))
    {
        FatalErrorInFunction
            << "Interval doesn't contain y." << exit(FatalError);
    }
    return x;
}


scalar FarField::interval::calcPointX(scalar y) const
{
    if (a_ == 0)
    {
        return (x1 + x2) / 2;
        FatalErrorInFunction
            << "Division on zero" << exit(FatalError);
    }
    scalar x = (y - b_)/a_;
    return x;
}


scalar FarField::interval::calcY1() const noexcept
{
    return interpolate(x1);
}


scalar FarField::interval::calcY2() const noexcept
{
    return interpolate(x2);
}


// ************************************************************************* //
