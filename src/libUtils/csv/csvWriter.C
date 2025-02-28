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

#include "OSspecific.H"

#include "csvWriter.H"
#include <iomanip>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Utils
{

static const fileName& createParentDirIfNotExist(const fileName& path)
{
    const fileName& parentDir = path.path();

    if (!Foam::exists(parentDir))
        Foam::mkDir(parentDir);

    return path;
}

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Utils::csvWriter::csvWriter(const csvOptions& opts)
:
    OFstream(createParentDirIfNotExist(opts.path)),
    csvOptions(opts),
    rowStarted_(true)
{
    stdStream() << std::fixed << std::setprecision(10);
}


void Utils::csvWriter::nr()
{
    *static_cast<OFstream*>(this) << rowSeparator;
    rowStarted_ = true;
}


// ************************************************************************* //
