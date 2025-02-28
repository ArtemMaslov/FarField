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

#include "error.H"

#include "filesHandler.H"

using namespace FarField;
using namespace FarField::Files;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

FarField::Files::filesHandler::filesHandler(const dictionary& propsDict) :
    inputFiles_(),
    files_(),
    pxFiles_(),
    distFiles_()
{
    const dictionary& inputDict = propsDict.subDict("inputNearField");
    inputFiles_.emplace_back(inputDict);

    const dictionary& outputDict = propsDict.subDict("outputFiles");

    for (const entry& entry : outputDict)
    {
        if (!entry.isDict())
        {
            FatalErrorInFunction
                << "Found non-dictionary entry \"" << entry
                << "\" in FarField outputFiles dictionary:"
                << outputDict << exit(FatalError);
        }

        // const word& dictName = entry.keyword();
        // const dictionary& fileDict = outputDict.subDict(dictName);
        
        Info << "reading output file";
        const dictionary& fileDict = entry.dict();
        type type = fileDict.get<Files::type>("type");
        
        switch (type)
        {
            case type::nearField:
            case type::farField:
                pxFiles_.emplace_back(fileDict);
            case type::WhithamFunction:
                distFiles_.emplace_back(fileDict);
                break;
            case type::shocks:
                files_.emplace_back(fileDict);
                break;
            default:
                NotImplemented;
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::Files::filesHandler::writeNearField
(
    pxDist& pxDist,
    const bool keepUnits
) const
{
    forAll(pxFiles_, i)
    {
        if (pxFiles_[i].type == type::nearField)
            pxDist.write(pxFiles_[i], keepUnits);
    }
}


void FarField::Files::filesHandler::writeNearField(const pxDist& pxDist) const
{
    forAll(pxFiles_, i)
    {
        if (pxFiles_[i].type == type::nearField)
            pxDist.write(pxFiles_[i]);
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::Files::filesHandler::writeFarField
(
    pxDist& pxDist,
    const bool keepUnits
) const
{
    forAll(pxFiles_, i)
    {
        if (pxFiles_[i].type == type::farField)
            pxDist.write(pxFiles_[i], keepUnits);
    }
}


void FarField::Files::filesHandler::writeFarField(const pxDist& pxDist) const
{
    forAll(pxFiles_, i)
    {
        if (pxFiles_[i].type == type::farField)
            pxDist.write(pxFiles_[i]);
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::Files::filesHandler::writeWhithamFunction
(
    const fyDist& fyDist
) const
{
    forAll(distFiles_, i)
    {
        if (distFiles_[i].type == type::WhithamFunction)
            fyDist.write(distFiles_[i]);
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::Files::filesHandler::writeShocks(DynamicList<shock>& shocks) const
{
    forAll(files_, i)
    {
        if (files_[i].type == type::shocks)
            FarField::writeShocks(shocks, files_[i]);
    }
}

// ************************************************************************* //
