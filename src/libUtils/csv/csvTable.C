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

#include "error.H"

#include "csvTable.H"
#include "tupleFor.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template <typename... T>
void Utils::csvTable::read
(
    const csvOptions& opts,
    List<string>& header,
    nVectorField<T...>& content
)
{
    NotImplemented

    //csvReader csv {opts};

    // Trying to read header.
    
    // if (header.size() != 0)
    // {
    //     forAll(header, i)
    //         csv << header[i];
    //     csv << nr;
    // }

    // forAll(content, i)
    // {
    //     tupleForAll(content[i], details::write);
    //     csv << nr;
    // }
}


template <typename... T>
void Utils::csvTable::write
(
    const csvOptions& opts,
    const List<string>& header,
    nVectorField<T...>& content
)
{
    csvWriter csv {opts};

    // Write csv header, only if it exist.
    if (header.size() != 0)
    {
        forAll(header, i)
            csv << header[i];
        csv << "idx";
        csv << nr;
    }

    size_t idx = 0;
    forAll(content, i)
    {
        tupleForAll(content[i], [&](auto&& t, label I){ csv << t; });
        csv << idx++;
        csv << nr;
    }
}


// ************************************************************************* //