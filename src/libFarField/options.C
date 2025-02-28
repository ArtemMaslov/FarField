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

#include "options.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

FarField::options::options(const dictionary& propsDict) :
    files(propsDict),
    r0(propsDict.getScalar("r0")),
    r1(propsDict.getScalar("r1")),
    Lref(propsDict.getScalar("referenceLength")),
    pinf(propsDict.getScalar("referencePressure"))
{
    Info<< nl
        << "Common FarField properties:" << nl
        << "    Near-field distance r0 = " << r0 << " m" << nl
        << "    Far-field  distance r1 = " << r1 << " m" << nl
        << "    Reference length       = " << Lref << " m" << nl
        << "    Reference pressure     = " << pinf << " Pa" << nl
        << endl;
}


// ************************************************************************* //