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

#include "csvTableReader.H"

#include "csvTable.H"

#include "pxDist.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

FarField::pxDist::pxDist(scalar Lref, scalar pinf) :
    nVectorField<scalar, scalar>{},
    xUnits_(unitsType::abs),
    pUnits_(unitsType::abs),
    Lref(Lref),
    pinf(pinf)
{
}


FarField::pxDist::pxDist(const pxDist& dist) :
    pxDist(dist.Lref, dist.pinf)
{
    xUnits_ = dist.xUnits_;
    pUnits_ = dist.pUnits_;
    resize(dist.size());
    deepCopy(dist);
}


FarField::pxDist& FarField::pxDist::operator = (const pxDist& dist)
{
    Lref = dist.Lref;
    pinf = dist.pinf;
    xUnits_ = dist.xUnits_;
    pUnits_ = dist.pUnits_;
    resize(dist.size());
    deepCopy(dist);
    return (*this);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::pxDist::xToRel()
{
    if (xUnits_ == unitsType::rel)
        return;
    x() /= Lref;
    xUnits_ = unitsType::rel;
}


void FarField::pxDist::xToAbs()
{
    if (xUnits_ == unitsType::abs)
        return;
    x() *= Lref;
    xUnits_ = unitsType::abs;
}


void FarField::pxDist::xToUnits(unitsType newXUnits)
{
    if (xUnits_ == newXUnits)
        return;
    switch (newXUnits)
    {
        case unitsType::rel:
            xToRel();
            break;
        case unitsType::abs:
            xToAbs();
            break;
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::pxDist::pToRel()
{
    if (pUnits_ == unitsType::rel)
        return;
    p() = (p() - pinf) / pinf;
    pUnits_ = unitsType::rel;
}


void FarField::pxDist::pToAbs()
{
    if (pUnits_ == unitsType::abs)
        return;
    p() = pinf * p() + pinf;
    pUnits_ = unitsType::abs;
}


void FarField::pxDist::pToUnits(unitsType newPUnits)
{
    if (pUnits_ == newPUnits)
        return;
    switch (newPUnits)
    {
        case unitsType::rel:
            pToRel();
            break;
        case unitsType::abs:
            pToAbs();
            break;
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::pxDist::setXUnits(unitsType xUnits)
{
    xUnits_ = xUnits;
}


void FarField::pxDist::setPUnits(unitsType pUnits)
{
    pUnits_ = pUnits;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void FarField::pxDist::read(const Files::pxFile& fileOpts)
{
    #warning Use csvTable.

    Info << "Reading file csvreader" << endl;

    dictionary csvInfo;
    csvInfo.add("hasHeaderLine", true);
    csvInfo.add("separator", string(','));
    // First column (coordinate along sampling axis).
    csvInfo.add("refColumn", 0);
    // Second column (pressure).
    csvInfo.add("componentColumns", "(1)");
    
    csvTableReader<scalar> pressure_reader(csvInfo);

    List<Tuple2<scalar, scalar>> pDistReader;
    pressure_reader(fileOpts.path, pDistReader);

    (*this).reserve(pDistReader.size());
    forAll(pDistReader, i)
    {
        (*this).append(std::make_tuple(
            pDistReader[i].first(),
            pDistReader[i].second()
        ));
    }

    xUnits_ = fileOpts.xUnits;
    pUnits_ = fileOpts.pUnits;

    Info<< "input file size: " << (*this).size() << endl;
}


void FarField::pxDist::write
(
    const Files::pxFile& fileOpts,
    const bool keepUnits
)
{
    if (fileOpts.type != Files::type::nearField
     && fileOpts.type != Files::type::farField)
    {
        FatalErrorInFunction
            << "[FarField] For pxDist file type must be \"nearField\" "
                "or \"farField\"."
            << exit(FatalError);
    }

    const unitsType savedXUnits = xUnits_;
    const unitsType savedPUnits = pUnits_;

    xToUnits(fileOpts.xUnits);
    pToUnits(fileOpts.pUnits);

    List<string> header {fileOpts.xColumnName, fileOpts.yColumnName};
    Utils::csvTable::write({fileOpts.path}, header, *this);

    if (keepUnits)
    {
        xToUnits(savedXUnits);
        pToUnits(savedPUnits);
    }
}


void FarField::pxDist::write(const Files::pxFile& fileOpts) const
{
    const_cast<pxDist*>(this)->write(fileOpts, true);
}


// ************************************************************************* //