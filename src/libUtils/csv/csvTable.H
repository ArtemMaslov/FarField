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
    This file is part of Utils library based on OpenFOAM.

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

#ifndef Utils_csvTable_H
#define Utils_csvTable_H

#include "error.H"
#include "List.H"

#include "nVectorField.H"
#include "csvReader.H"
#include "csvWriter.H"
#include "csvOptions.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Utils
{

/*---------------------------------------------------------------------------*\
                           Class csvTable Declaration
\*---------------------------------------------------------------------------*/

class csvTable
{
public:
    template <typename... T>
    static void read
    (
        const csvOptions& opts,
        List<string>& header,
        nVectorField<T...>& content
    );


    template <typename... T>
    static void write
    (
        const csvOptions& opts,
        const List<string>& header,
        nVectorField<T...>& content
    );
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Utils.

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "csvTable.C"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // End define Utils_csvTable_H.

// ************************************************************************* //
