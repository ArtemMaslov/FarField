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

#include <cstddef>

#include "token.H"

#include "tupleFor.H"
#include "tupleIO.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Utils
{
namespace Details
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template <size_t I, size_t N>
struct tupleIO
{
    template <typename... T>
    static inline void Read(Foam::Istream& is, std::tuple<T...>& t)
    {
        is >> std::get<I>(t);
        tupleIO<I + 1, N>::Read(is, t);
    }

    template <typename... T>
    static inline void Write(Foam::Ostream& os, const std::tuple<T...>& t)
    {
        os << Foam::token::SPACE << std::get<I>(t);
        tupleIO<I + 1, N>::Write(os, t);
    }
};


template <size_t N>
struct tupleIO<N, N>
{
    template <typename... T>
    static inline void Read(Foam::Istream& is, std::tuple<T...>& t) {};

    template <typename... T>
    static inline void Write(Foam::Ostream& os, const std::tuple<T...>& t) {};
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace details.
} // End namespace Utils.

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template <typename... T>
Foam::Istream& Foam::operator >>
(
    Istream& is,
    std::tuple<T...>& t
)
{
    // tuple format: (T1 T2 T3)
    is.readBegin("std::tuple");

    constexpr size_t size = std::tuple_size<std::tuple<T...>>::value;
    Utils::Details::tupleIO<0, size>::Read(is, t);

    is.readEnd("std::tuple");

    is.check(FUNCTION_NAME);

    return is;
}


template <typename... T>
Foam::Ostream& Foam::operator <<
(
    Ostream& os,
    const std::tuple<T...>& t
)
{
    os << token::BEGIN_LIST << std::get<0>(t);

    constexpr size_t size = std::tuple_size<std::tuple<T...>>::value;
    Utils::Details::tupleIO<1, size>::Write(os, t);

    os << token::END_LIST;

    os.check(FUNCTION_NAME);

    return os;
}


// ************************************************************************* //
