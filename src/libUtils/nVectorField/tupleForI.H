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

#include "tupleFor.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Utils
{
namespace Details
{


template <size_t I, size_t N>
struct tupleFor
{
    template
    <
        typename... Ts, 
        typename OnInterationHandler,
        typename... Args
    >
    static inline void Iteration
    (
        std::tuple<Ts...>& t,
        OnInterationHandler&& f,
        Args... args
    )
    {
        // Do iteration.
        f(std::get<I>(t), I, args...);
        // Next iteration.
        tupleFor<I + 1, N>::Iteration(t, f, args...);
    }
};


template <size_t N>
struct tupleFor<N, N>
{
    template
    <
        typename... Ts, 
        typename OnInterationHandler,
        typename... Args
    >
    static inline void Iteration
    (
        std::tuple<Ts...>& t,
        OnInterationHandler&& f,
        Args... args
    )
    {
        // For end. Iteration after the last one.
    };
};


}
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template
<
    size_t start,
    size_t stop,
    typename... Ts,
    typename OnInterationHandler,
    typename... Args
>
void Utils::tupleFor
(
    std::tuple<Ts...>& t,
    OnInterationHandler f,
    Args... args
)
{
    Details::tupleFor<start, stop>::Iteration(t, f, args...);
}


template
<
    typename... Ts,
    typename OnInterationHandler,
    typename... Args
>
void Utils::tupleForAll
(
    std::tuple<Ts...>& t,
    OnInterationHandler f,
    Args... args
)
{
    constexpr size_t size = std::tuple_size<std::tuple<Ts...>>::value;
    Details::tupleFor<0, size>::Iteration(t, f, args...);
}


// ************************************************************************* //
