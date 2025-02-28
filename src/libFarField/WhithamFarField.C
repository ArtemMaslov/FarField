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

#include "List.H"
#include "fileName.H"
#include "files.H"
#include "stdFoam.H"
#include "zero.H"

#include "WhithamFarField.H"
#include "pxDist.H"
#include "fyDist.H"
#include "options.H"
#include "polyline.H"
#include "equalAreaRuleSimple.H"
#include "fpoint.H"
#include "helperPoints.H"

using namespace Foam;
using namespace FarField;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

FarField::Whitham::Whitham
(
    const scalar heatCapacityRatio,
    const scalar MachNumber,
    const options& opts,
    const label  integrateStepsCount,
    const label iterationsCount
)
:
    heatCapacityRatio_(heatCapacityRatio),
    MachNumber_(MachNumber),
    betaCoefficient_(computeBeta()),
    kappaCoefficient_(computeKappa()),
    integrateStepsCount_(integrateStepsCount),
    iterationsCount_(iterationsCount),
    opts_(opts)
{
    Info<< "\n"
           "Whitham's theory parameters:" << "\n"
           "    Heat capacity ratio    = " << heatCapacityRatio_ << "\n"
           "    Mach number            = " << MachNumber_ << "\n"
           "    betaCoefficient        = " << betaCoefficient_ << "\n"
           "    kappaCoefficient_      = " << kappaCoefficient_ << "\n"
           "    integrateStepsCount    = " << integrateStepsCount_ << "\n"
           "    iterationsCount        = " << iterationsCount_ << "\n"
        << endl;
}


FarField::Whitham::Whitham
(
    const dictionary& propsDict,
    const options& opts
)
:
    Whitham
    (
        propsDict.getScalar("HeatCapacity"),
        propsDict.getScalar("MachNumber"),
        opts,
        propsDict.getScalar("integrateStepsCount"),
        propsDict.getOrDefault("iterationsCount", 1)
    )
{
    p0_ = (propsDict.getScalar("p0"));
    p1_ = (propsDict.getScalar("p1"));
}


// * * * * * * * * * * * * * * * * Formulas  * * * * * * * * * * * * * * * * //

scalar FarField::Whitham::computeBeta() const noexcept
{
    if (MachNumber_ <= 1)
    {
        FatalErrorInFunction
            << "For Whitham theory Mach number cannot be less or equal to 1"
            << exit(FatalError);
    }
    return std::sqrt(std::pow(MachNumber_, 2) - 1);
}


scalar FarField::Whitham::computeKappa() const noexcept
{
    return 
        ( heatCapacityRatio_ + 1 )
       *( std::pow(MachNumber_, 4) )
       /( std::sqrt(2*std::pow(betaCoefficient_, 3.0)) );
}


// * * * * * * * * * * * * * Computation algorithm * * * * * * * * * * * * * //

// void FarField::Whitham::xToMachCone
// (
//     List<point>& px,
//     const scalar r
// )
// {
//     forAll(px, i)
//     {
//         px[i].px.x -= r * betaCoefficent_ / MachNumber_;
//     }
// }


// void FarField::Whitham::xToNwaveZeroPoint
// (
//     List<point>& px
// ) const
// {
//     Info << "Moving shocks" << endl;
//     // Find minimum negative pressure from the end.
//     label minIdx = 0;
//     forAll(px, idx)
//     {
//         if (px[idx].px.p < px[minIdx].px.p)
//         {
//             minIdx = idx;
//         }
//     }
//     Info << "minIdx = " << minIdx << endl;
//     if (minIdx < 0 || px[minIdx].px.p > 0)
//     {
//         // Cannot format pressure distribution. Do not nothing.
//         Info << "return1" << endl;
//         return;
//     }

//     label idx = 0;
//     // Now find first positive pressure.
//     for (idx = minIdx; idx >= 0; idx--)
//     {
//         if (px[idx].px.p >= 0)
//         {
//             break;
//         }
//     }
//     if (idx < 0)
//     {
//         // Cannot format pressure distribution. Do not nothing.
//         Info << "return2" << endl;
//         return;
//     }

//     polyline::interval intersectInt
//         {
//             px[idx].px.x,
//             px[idx].px.p,
//             px[idx + 1].px.x,
//             px[idx + 1].px.p
//         };
    
//     scalar x0 = intersectInt.calcPointX(0);
//     Info << "x0 = " << x0 << endl;

//     forAll(px, i)
//     {
//         px[i].px.x -= x0;
//     }
// }

void FarField::Whitham::computeFarFieldPressure
(
    const scalar r0,
    pxDist& p0x,
    const scalar r,
    pxDist& outPx
) const
{
    p0x.pToRel();
    p0x.xToAbs();

    pxDist currPx {p0x.Lref, p0x.pinf};
    currPx.setXUnits(unitsType::abs);
    currPx.setPUnits(unitsType::rel);

    helperPoints hp {r0, p0x, r, currPx, *this};
    hp.addHelperPoints();

    currPx.write
    (
        Files::pxFile{Files::type::farField,
        "output/helperPoints.csv"}
    );
    
    const scalar dr = (r - r0) / iterationsCount_;
    const scalar endR = r;
    scalar currR = r0;
    size_t iterIdx = 1;
    while (currR < endR)
    {
        #warning add debug on each iterations.

        const scalar nextR = currR + dr;

        Info<< "Iteration " << iterIdx << ":\n"
               "current R = " << currR << "\n"
               "next    R = " << nextR << endl;

        Info<< "Computing Whitham function..." << endl;

        fyDist Fy {};
        Fy.resize(currPx.size());
        Fy.F() = computeF(currPx.p(), currR);
        Fy.y() = computeY(currPx.x(), Fy.F(), currR);

        opts_.files.writeWhithamFunction(Fy);

        Info<< "Computing far-field pressure..." << endl;
        
        pxDist nextPx {currPx.Lref, currPx.pinf};
        nextPx.setXUnits(unitsType::abs);
        nextPx.setPUnits(unitsType::rel);
        nextPx.resize(Fy.size());

        nextPx.p() = computeP(Fy.y(), Fy.F(), nextR, p0_, p1_);
        nextPx.x() = computeX(Fy.y(), Fy.F(), nextR);

        string debugFarFieldNoShocksPath = "output/" + std::to_string(iterIdx) + "farField_noShocks.csv";

        nextPx.write(Files::pxFile{Files::type::farField, debugFarFieldNoShocksPath});
        nextPx.write(Files::pxFile{Files::type::farField, "shocks/no_shocks.csv"});

        #warning log no shocks
        // if (log_dp_p1x_no_shocks_)
        // {
        //     Files::outputFarFieldFile out {};
        //     out.filePath = "dp_p1x_no_shocks.csv";
        //     out.pUnits = unitsType::rel;
        //     out.xUnits = unitsType::abs;
        //     out.xColumnName = "x";
        //     out.yColumnName = "p";
        //     out.write(polyline.augPoints, unitsType::abs, unitsType::rel, 1, 1);
        // }

        Info<< "Applying equal area rule..." << endl;

        polyline polyline{nextPx};
        equalAreaRuleSimple equalAreaRule
        {
            polyline,
            integrateStepsCount_
        };
        DynamicList<FarField::fpoint> outNextP {nextPx.size()};
        DynamicList<shock> shocks {};
        equalAreaRule.apply(outNextP, shocks);
        opts_.files.writeShocks(shocks);

        nextPx.resize(outNextP.size());
        forAll(outNextP, i)
        {
            nextPx.x()[i] = outNextP[i].x;
            nextPx.p()[i] = outNextP[i].y;
        }
        
        #warning log shocks, log far-field

        string debugFarFieldShocksPath = "output/" + std::to_string(iterIdx) + "farField_shocks.csv";

        nextPx.write(Files::pxFile{Files::type::farField, debugFarFieldShocksPath});
        nextPx.write(Files::pxFile{Files::type::farField, "shocks/with_shocks.csv"});

        // if (opts_.log_dp_p1x)
        // {
        //     Info<< "Writing far-field relative pressure distribution with shocks "
        //            "into \"dp_p1x.csv\"..."
        //         << endl;
        //     stdCsvWriters::write_px("dp_p1x.csv", out_dp_p);
        // }

        currR = nextR;
        currPx = nextPx;

        iterIdx++;
    }

    // Correction to the non-homogeneous atmosphere, in which pressure
    // and temperature vary with altitude.
    currPx.p() *= Foam::sqrt(p1_/p0_);

    outPx = currPx;
}


// ************************************************************************* //