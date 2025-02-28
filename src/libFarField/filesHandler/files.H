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

#ifndef FarField_files_H
#define FarField_files_H

#include "dictionary.H"
#include "fileName.H"
#include "word.H"

#include "unitsType.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace FarField
{

using namespace Foam;

namespace Files
{

class file;
class pxFile;
class distFile;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

enum class type
{
    input           = 0, // pxFile
    nearField       = 1, // pxFile
    farField        = 2, // pxFile
    WhithamFunction = 3, // distFile
    shocks          = 4, // file
    max_
};


Istream& operator>>(Istream& is, type& val);
Ostream& operator<<(Ostream& os, const type& val);


using inputFile           = pxFile;
using nearFieldFile       = pxFile;
using farFieldFile        = pxFile;
using WhithamFunctionFile = distFile;
using shocksFile          = file;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

class file
{
public:
    Files::type type;
    fileName path;

    file();

    file(const Files::type type, const fileName& path);
    
    file(const dictionary& propsDict);
};


class distFile : public file
{
public:
    word xColumnName;
    word yColumnName;

    distFile() = default;

    distFile(const Files::type type, const fileName& path);

    distFile(const dictionary& propsDict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

class pxFile : public distFile
{
public:
    unitsType xUnits;
    unitsType pUnits;

    pxFile() = default;

    pxFile(const Files::type type, const fileName& path);

    pxFile(const dictionary& propsDict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Files
} // End namespace FarField

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // end define FarField_files_H

// ************************************************************************* //
