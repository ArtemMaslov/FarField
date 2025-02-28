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

#include <cstddef>

#include "files.H"

using namespace FarField;
using namespace Files;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

static constexpr size_t fileTypesCount = static_cast<size_t>(type::max_);
static const char* const fileTypeNames[fileTypesCount] = 
{
    "input",
    "nearField",
    "farField",
    "WhithamFunction",
    "shocks"
};


Istream& FarField::Files::operator>>(Istream& is, type& val)
{
    token tok(is);
    bool good = false;

    if (tok.isWord())
    {
        word units = tok.wordToken();
        for (size_t i = 0; i < fileTypesCount; i++)
        {
            if (units == fileTypeNames[i])
            {
                val = static_cast<type>(i);
                good = true;
            }
        }
    }
    
    if (good)
    {
        is.check(FUNCTION_NAME);
        return is;
    }
    else
    {
        FatalIOErrorInFunction(is);
        if (tok.good())
        {
            FatalIOError
                << "Wrong token type - expected Farfield::distFileType, found "
                << tok.info();
        }
        else
        {
            FatalIOError
                << "Bad token - could not get Farfield::distFileType";
        }
        FatalIOError << exit(FatalIOError);
        is.setBad();
        return is;
    }
}


Ostream& FarField::Files::operator<<(Ostream& os, const type& val)
{
    os.write(fileTypeNames[static_cast<size_t>(val)]);
    os.check(FUNCTION_NAME);
    return os;
}


// ************************************************************************* //
