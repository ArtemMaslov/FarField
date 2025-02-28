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

#ifndef FarField_pxDist_H
#define FarField_pxDist_H

#include "scalar.H"

#include "nVectorField.H"

#include "unitsType.H"
#include "files.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace FarField
{

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

class pxDist : public Utils::nVectorField<scalar, scalar>
{
    typedef nVectorField<scalar, scalar> base;
private:

unitsType xUnits_;
unitsType pUnits_;

public:

    scalar Lref;
    scalar pinf;

    pxDist(scalar Lref, scalar pinf);

    pxDist(const pxDist& dist);
    pxDist& operator = (const pxDist& dist);

    auto x() { return Utils::cmpt<0, scalar, scalar>{*this}; }
    auto p() { return Utils::cmpt<1, scalar, scalar>{*this}; }

    void xToRel();
    void xToUnits(unitsType newXUnits);
    void xToAbs();

    void pToRel();
    void pToAbs();
    void pToUnits(unitsType newPUnits);

    #warning Убрать эти функции. Пусть x(unitsType newUnits). Или WhithamFarField переделать.
    void setXUnits(unitsType xUnits);
    void setPUnits(unitsType pUnits);

    void read(const Files::pxFile& fileOpts);

    void write
    (
        const Files::pxFile& fileOpts,
        const bool keepUnits = true
    );
    void write(const Files::pxFile& fileOpts) const;

}; // End class pxDist.

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace FarField.

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // end define FarField_pxDist_H.

// ************************************************************************* //
