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

#ifndef FarField_WhithamFarField_H
#define FarField_WhithamFarField_H

#include "scalar.H"

#include "options.H"
#include "pxDist.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace FarField
{

using namespace Foam;

/*---------------------------------------------------------------------------*\
                           Class Whitham Declaration
\*---------------------------------------------------------------------------*/


class Whitham
{
public:

    //- Heat capacity ratio. \gamma = \frac{C_p}{C_V}.
    const scalar heatCapacityRatio_;

    //- Mach number. Equal to \frac{v}{c}, where v is speed of aircraft, c 
    //- is speed of sound.
    const scalar MachNumber_;

    //- Beta coefficient. \beta = \sqrt(M^2 - 1).
    const scalar betaCoefficient_;

    //- Kappa coefficient. 
    //- \kappa = (\gamma + 1) * M^4 * \frac{1}{\sqrt{2} * \beta^{3/2}}.
    const scalar kappaCoefficient_;
    
    const label integrateStepsCount_;

    const label iterationsCount_;

    #warning remove me:
    scalar p0_;
    scalar p1_;

private:
    const options& opts_;

public:

    Whitham
    (
        const scalar heatCapacityRatio,
        const scalar MachNumber,
        const options& opts,
        const label  integrateStepsCount,
        const label iterationsCount
    );

    Whitham
    (
        const dictionary& propsDict,
        const options& opts
    );

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    scalar computeBeta() const noexcept;

    scalar computeKappa() const noexcept;

    // T[i] is scalar or nVectorField component.
    template <typename T1, typename T2>
    auto computeY
    (
        const T1& x,
        const T2& F,
        const scalar r
    ) const noexcept;

    template <typename T1, typename T2>
    auto computeX
    (
        const T1& y,
        const T2& F,
        const scalar r
    ) const noexcept;

    template <typename T>
    auto computeF
    (
        const T& dp_p,
        const scalar r
    ) const noexcept;

    template <typename T1, typename T2>
    auto computeP
    (
        const T1& y,
        const T2& F,
        const scalar r,
        scalar p0,
        scalar p1
    ) const noexcept;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    // void xToMachCone
    // (
    //     List<point>& px,
    //     const scalar r
    // );

    // void xToNwaveZeroPoint
    // (
    //     List<point>& px
    // ) const;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    void computeFarFieldPressure
    (
        const scalar r0,
        pxDist& p0,
        const scalar r,
        pxDist& out_p
    ) const;

}; // End class Whitham.


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace FarField.

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "WhithamFarFieldI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // end define FarField_WhithamFarField_H.

// ************************************************************************* //
