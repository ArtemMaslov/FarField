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

#include "csvReader.H"
#include "filesIOErrors.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template <class T>
Utils::csvReader& Utils::csvReader::operator>>(T& value)
{
    if (rowStarted_)
        rowStarted_ = false;
    else
    {
        string sep {};
        *(static_cast<IFstream*>(this)) >> sep;
        if (sep == columnSeparator)
        {
            FatalIOErrorInFile(path, this->lineNumber())
                << "csvReader: unknown column separator '" << sep << "', "
                "expected '" << columnSeparator << "'."
                << exit(FatalError);
        }
    }

    *(static_cast<IFstream*>(this)) >> value;
    return *this;
}


// ************************************************************************* //
