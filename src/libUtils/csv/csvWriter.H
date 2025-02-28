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
    This file is part of Utils library based on OpenFOAM.

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


#ifndef Utils_csvWriter_H
#define Utils_csvWriter_H

#include "OFstream.H"

#include "csvOptions.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Utils
{

using namespace Foam;

/*---------------------------------------------------------------------------*\
                          Class csvWriter Declaration
\*---------------------------------------------------------------------------*/

class csvWriter
:
    public OFstream,
    public csvOptions
{
private:
    bool rowStarted_;

public:
    csvWriter(const csvOptions& opts);

    void nr();

    template <class T>
    Utils::csvWriter& operator<<(const T& value);
    
}; // End class csvWriter.


//- An csvWriter manipulator.
typedef csvWriter& (*csvWriterManip)(csvWriter&);


//- operator<< handling for manipulators without arguments.
inline csvWriter& operator<<(csvWriter& csv, csvWriterManip funct)
{
    return funct(csv);
}


//- New row.
inline csvWriter& nr(csvWriter& csv)
{
    csv.nr();
    return csv;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Utils.

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "csvWriterI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // End define Utils_csvWriter_H.

// ************************************************************************* //
