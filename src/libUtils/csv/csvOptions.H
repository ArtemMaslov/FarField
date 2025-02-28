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

#ifndef Utils_csvOptions_H
#define Utils_csvOptions_H

#include "fileName.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Utils
{

using namespace Foam;

/*---------------------------------------------------------------------------*\
                          Class csvOptions Declaration
\*---------------------------------------------------------------------------*/

class csvOptions
{
public:
    // Options both for csvReader and csvWriter:

        //- Table file path.
        //  File extension does not matter. For example, it can be ".txt", 
        //  but the file contents are in CSV format.
        const fileName path;
        //- What char sequence is considered to be columns separator.
        //  By RFC 4180 standard it should be comma ','.
        //  But some implementations use semicolon ';'. 
        const char* const columnSeparator;
        //- What char sequence is considered to be row separator.
        const char* const rowSeparator;

    // Options for csvReader only:

        //- Should csvReader skip leading and trailing spaces in field
        //- or spaces are part of the field and should not be skipped.
        const bool skipLeadingAndTrailingSpaces : 1;
        //- 
        const bool hasHeader                   : 1;

    csvOptions
    (
        const fileName& path,
        const bool hasHeader = true,
        const char* const columnSeparator = ",",
        const char* const rowSeparator    = "\n",
        const bool skipLeadingAndTrailingSpaces = true
    );
};


} // End namespace Utils.

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // End define Utils_csvOptions_H.

// ************************************************************************* //
