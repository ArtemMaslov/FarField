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

#ifndef FarField_equalAreaRuleSimple_H
#define FarField_equalAreaRuleSimple_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "DynamicList.H"
#include "List.H"

#include "fpoint.H"
#include "polyline.H"
#include "shock.H"

namespace FarField
{

using namespace Foam;

/*---------------------------------------------------------------------------*\
                     Class equalAreaRuleSimple Declaration
\*---------------------------------------------------------------------------*/

class equalAreaRuleSimple
{
private:
    const polyline& polyline_;

    //- Pressure distribution is interpolated. Each interpolation interval
    //  is splitted on intervalStepsCount parts. Shock wave is placed at every
    //  new point and areas left and right from the shock is computed. 
    const label stepsCount_;

public:
    equalAreaRuleSimple
    (
        const polyline& polyline,
        label stepsCount = 50
    );

    void apply(DynamicList<fpoint>& outPDist, DynamicList<shock>& outShocks);

    bool checkShocks(const DynamicList<shock>& shocks);
    bool checkDist(const DynamicList<fpoint>& dist);

private:
    void findShocks(DynamicList<shock>& shocks);
    void removeShocks(DynamicList<shock>& shocks);

    // return true, if shock have been removed.
    bool removeLeftMiniShock
    (
        DynamicList<shock>& shocks,
        const label currIdx,
        const label prevIdx
    );

    // return true, if shock have been removed.
    bool removeRightMiniShock
    (
        DynamicList<shock>& shocks,
        const label currIdx,
        const label prevIdx
    );

    // return true, if shock have been removed.
    bool removeOverlappingMainShocks
    (
        DynamicList<shock>& shocks,
        const label currIdx,
        const label prevIdx
    );

    void addShock(DynamicList<shock>& shocks, label startOrientPoint);

    void placeShocks(DynamicList<fpoint>& pDist, DynamicList<shock>& shocks);

}; // End class equalAreaRuleSimple


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace FarField

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // end define FarField_equalAreaRuleSimple_H

// ************************************************************************* //