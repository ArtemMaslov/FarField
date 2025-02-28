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

#include "OSspecific.H"
#include "argList.H"
#include "error.H"

#include "files.H"

using namespace FarField;
using namespace FarField::Files;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

static word getDefXColName(Files::type type)
{
    switch (type)
    {
        case Files::type::input:
        case Files::type::nearField:
        case Files::type::farField:
            return "x";
        case Files::type::WhithamFunction:
            return "y";
        default:
            NotImplemented;
            return "";
    }
}


static word getDefYColName(Files::type type)
{
    switch (type)
    {
        case Files::type::input:
        case Files::type::nearField:
            return "p";
        case Files::type::farField:
            return "dp_p";
        case Files::type::WhithamFunction:
            return "F";
        default:
            NotImplemented;
            return "";
    }
}


static unitsType getDefXUnits(Files::type type)
{
    switch (type)
    {
        case Files::type::input:
        case Files::type::nearField:
        case Files::type::farField:
            return unitsType::abs;
        default:
            NotImplemented;
            return unitsType::abs;
    }
}


static unitsType getDefPUnits(Files::type type)
{
    switch (type)
    {
        case Files::type::input:
        case Files::type::nearField:
            return unitsType::abs;
        case Files::type::farField:
            return unitsType::rel;
        default:
            NotImplemented;
            return unitsType::abs;
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

FarField::Files::file::file() :
    type(Files::type::max_),
    path()
{}


FarField::Files::file::file(const Files::type type, const fileName& path) :
    type(type),
    path(path)
{}


FarField::Files::file::file(const dictionary& propsDict) :
    type(propsDict.get<Files::type>("type")),
    path(propsDict.getFileName("path"))
{
    if (type == Files::type::input && !Foam::exists(path))
    {
        FatalErrorInFunction
            << "Input file path \"" << path << "\" does not exist.\n"
            << "Current directory: \"" << argList::envGlobalPath() << "\""
            << endl << exit(FatalError);
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

FarField::Files::distFile::distFile
(
    const Files::type type,
    const fileName& path
) :
    file(type, path),
    xColumnName(getDefXColName(type)),
    yColumnName(getDefYColName(type))
{}


FarField::Files::distFile::distFile(const dictionary& propsDict) :
    file(propsDict),
    xColumnName(propsDict.getOrDefault( "xColumnName", getDefXColName(type) )),
    yColumnName(propsDict.getOrDefault( "yColumnName", getDefXColName(type) ))
{}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

FarField::Files::pxFile::pxFile(const Files::type type, const fileName& path) :
    distFile(type, path),
    xUnits(getDefXUnits(type)),
    pUnits(getDefPUnits(type))
{}


FarField::Files::pxFile::pxFile(const dictionary& propsDict) :
    distFile(propsDict),
    xUnits(propsDict.getOrDefault( "xUnits", getDefXUnits(type) )),
    pUnits(propsDict.getOrDefault( "pUnits", getDefPUnits(type) ))
{}


// ************************************************************************* //
