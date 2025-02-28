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

#ifndef FarField_filesHandler_H
#define FarField_filesHandler_H

#include "dictionary.H"
#include "DynamicList.H"

#include "files.H"
#include "pxDist.H"
#include "fyDist.H"
#include "shock.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace FarField
{

using namespace Foam;

namespace Files
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

class filesHandler
{
public:
    using InputFilesList = DynamicList<pxFile, 10>;

    filesHandler(const dictionary& propsDict);

    const InputFilesList& getInputFiles() const { return inputFiles_; };


    void writeNearField
    (
        pxDist& pxDist,
        const bool keepUnits = true
    ) const;

    void writeNearField(const pxDist& pxDist) const;


    void writeFarField
    (
        pxDist& pxDist,
        const bool keepUnits = true
    ) const;

    void writeFarField(const pxDist& pxDist) const;


    void writeWhithamFunction(const fyDist& fyDist) const;


    void writeShocks(DynamicList<shock>& shocks) const;


private:
    InputFilesList           inputFiles_;
    DynamicList<file,     1> files_;
    DynamicList<pxFile,   2> pxFiles_;
    DynamicList<distFile, 1> distFiles_;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Files
} // End namespace FarField

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // end define FarField_filesHandler_H

// ************************************************************************* //
