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

#ifndef FarField_interval_H
#define FarField_interval_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "scalar.H"

namespace FarField
{

using namespace Foam;

/*---------------------------------------------------------------------------*\
                              interval Declaration
\*---------------------------------------------------------------------------*/

class interval
{
public:


    // Interval boundaries.
    scalar x1;
    scalar x2;
private:
    // Line equation: y = a*x + b.
    scalar a_;
    scalar b_;
    scalar area_;

public:

    interval();

    interval
    (
        scalar inX1,
        scalar inY1,
        scalar inX2,
        scalar inY2
    ) noexcept;

    bool containsX(scalar x) const noexcept;

    scalar interpolate(scalar x) const noexcept;

    scalar calcInnerPointX(scalar y) const;

    scalar calcPointX(scalar y) const;

    scalar getArea() const noexcept;

    scalar calcY1() const noexcept;

    scalar calcY2() const noexcept;

    friend Ostream& operator<<
    (
        Ostream& os,
        const interval& value
    );

}; // End class interval


Ostream& operator<<
(
    Ostream& os,
    const interval& value
);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace FarField

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // end define FarField_interval_H

// ************************************************************************* //