/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

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

#include "argList.H"
#include "IFstream.H"
#include "dictionary.H"
#include "fileName.H"

#include "options.H"
#include "WhithamFarField.H"

using namespace FarField;
using namespace FarField::Files;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

static void ComputeWhithamFarField
(
    dictionary& propsDict,
    options& opts,
    pxDist& p0x,
    pxDist& out_p1x
);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char* argv[])
{
    // Initialize OpenFoam environment.
    argList::addNote
    (
        "Library used to compute far-field pressure distribution using "
        "known near-field one utilizing Whitham's theory.\n"
    );

    argList::noParallel();
    argList::noFunctionObjects();

    #include "setRootCase.H"

    Info<< "Reading FarField configuration..." << endl;

    IFstream ifs {"constant/farFieldProperties"};
    dictionary propsDict {ifs};
    
    // Read far-field options.

    options opts{propsDict};

    // Read near-field pressure distribution.

    const filesHandler::InputFilesList& inputFiles =
        opts.files.getInputFiles();
    
    forAll(inputFiles, inputFilesIdx)
    {
        const pxFile& inputFile = inputFiles[inputFilesIdx];

        Info<< "Input file = \"" << inputFile.path << "\"" << endl;

        Info<< "Reading near-field data..." << endl;

        pxDist p0x {opts.Lref, opts.pinf};
        p0x.read(inputFile);
        opts.files.writeNearField(p0x);

        Info<< "Near-field data is read." << endl;

        Info<< "Computing far-field pressure..." << endl;
        // Compute far-field pressure distribution.
        pxDist p1x {opts.Lref, opts.pinf};
        ComputeWhithamFarField(propsDict, opts, p0x, p1x);

        // Write far-field pressure distribution.
        opts.files.writeFarField(p1x);

        Info<< "Computing done for input file = \""  << inputFile.path << "\""
            << endl;
    }

    Info<< "Done." << endl;

    return 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

static void ComputeWhithamFarField
(
    dictionary& propsDict,
    options& opts,
    pxDist& p0x,
    pxDist& out_p1x
)
{
    Info<< "Using Whitham theory." << endl;
    
    Whitham FarField
    (
        propsDict.subDict("WhithamParams"),
        opts
    );

    FarField.computeFarFieldPressure(opts.r0, p0x, opts.r1, out_p1x);

    Info<< "Formatting result pressure distribution..." << endl;
}


// ************************************************************************* //