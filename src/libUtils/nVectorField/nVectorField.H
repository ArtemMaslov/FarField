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

#ifndef Utils_nVectorField_H
#define Utils_nVectorField_H

#include <tuple>
#include <utility>

#include "DynamicList.H"
#include "ops.H"

#include "tupleIO.H"
#include "tupleFor.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Utils
{

using namespace Foam;

template <typename... T>
class nVectorField : public DynamicList<std::tuple<T...>>
{
    using base = DynamicList<std::tuple<T...>>;

public:
    using rowT = std::tuple<T...>;
    
    using base::base;

    void write(const fileName& path);

    void read(const fileName& path);
};

template <typename T1, typename T2, typename Op>
class bexpr;

template <typename T>
using expr = bexpr<T, T, void>;

template <size_t idx, typename... T>
class cmpt_
{
public:
    nVectorField<T...>& field_;

    cmpt_(nVectorField<T...>& field) :
        field_(field)
    {
    }
};

template <typename T1, typename T2, typename Op>
class bexpr
{
public:
    T1 t1_;
    T2 t2_;

    bexpr(const T1& t1, const T2& t2) :
        t1_(t1),
        t2_(t2)
    {}

    bexpr(T1&& t1, T2&& t2) :
        t1_(std::move(t1)),
        t2_(std::move(t2))
    {}

    auto operator () (size_t ridx)
    {
        auto r1 = t1_(ridx);
        auto r2 = t2_(ridx);
        return Op()(r1, r2);
    }
};


template <typename T>
class bexpr<T, T, void> // expr<T>
{
public:
    T t_;

    bexpr(const T& t) :
        t_(t)
    {}

    bexpr(T&& t) :
        t_(std::move(t))
    {}

    T operator () (size_t ridx)
    {
        return t_;
    }
};


template <typename T1, typename T2, typename Op>
class bexpr<bexpr<T1, T2, Op>, bexpr<T1, T2, Op>, void> // expr<bexpr<T1, T2, Op>>
{
    using T = bexpr<T1, T2, Op>;
public:
    T t_;

    bexpr(const T& t) :
        t_(t)
    {}

    bexpr(T&& t) :
        t_(std::move(t))
    {}

    auto operator () (size_t ridx)
    {
        auto r = t_(ridx);
        return r;
    }
};


template <size_t idx, typename... T>
class bexpr<cmpt_<idx, T...>, cmpt_<idx, T...>, void> // expr<cmpt<idx, T...>>
{
public:
    cmpt_<idx, T...> c_;

    bexpr(const cmpt_<idx, T...>& t) :
        c_(t)
    {
    }

    bexpr(cmpt_<idx, T...>&& t) :
        c_(std::move(t))
    {
    }


    auto operator () (size_t ridx)
    {
        auto r = (*this)[ridx];
        return r;
    }

    auto& operator [] (size_t ridx)
    {
        return std::get<idx>(c_.field_[ridx]);
    }

    template <typename... Y>
    void operator = (bexpr<Y...> e)
    {
        forAll(c_.field_, ri)
        {
            std::get<idx>(c_.field_[ri]) = e(ri);
        }
    }

    #define NVECTORFIELD_OP(opeq, op)                                         \
        template <typename... Y>                                              \
        void operator opeq (bexpr<Y...> e) { *this = *this op e; }            \
                                                                              \
        template <typename Y>                                                 \
        void operator opeq (Y e) { *this = *this op e; }

    NVECTORFIELD_OP(+=, +)
    NVECTORFIELD_OP(-=, -)
    NVECTORFIELD_OP(*=, *)
    NVECTORFIELD_OP(/=, /)

    #undef NVECTORFIELD_OP
};


#define NVECTORFIELD_OP(op, opName)                                           \
    template <typename... Ts1, typename... Ts2>                               \
    auto operator op (bexpr<Ts1...> be1, bexpr<Ts2...> be2)                   \
    {                                                                         \
        using et1 = decltype(be1(0));                                         \
        using et2 = decltype(be2(0));                                         \
        using top = decltype(be1(0) op be2(0));                               \
        using FoamOp = Foam::opName##Op3<top, et1, et2>;                      \
        return bexpr<bexpr<Ts1...>, bexpr<Ts2...>, FoamOp>(be1, be2);         \
    }                                                                         \
                                                                              \
    template <typename... Ts1, typename T2>                                   \
    auto operator op (bexpr<Ts1...> be1, T2 e2)                               \
    {                                                                         \
        expr<T2> be2 {e2};                                                    \
        using et1 = decltype(be1(0));                                         \
        using top = decltype(be1(0) op e2);                                   \
        using FoamOp = Foam::opName##Op3<top, et1, T2>;                       \
        return bexpr<bexpr<Ts1...>, expr<T2>, FoamOp>(be1, e2);               \
    }                                                                         \
                                                                              \
    template <typename T1, typename... Ts2>                                   \
    auto operator op (T1 e1, bexpr<Ts2...> be2)                               \
    {                                                                         \
        expr<T1> be1 {e1};                                                    \
        using et2 = decltype(be2(0));                                         \
        using top = decltype(e1 op be2(0));                                   \
        using FoamOp = Foam::opName##Op3<top, T1, et2>;                       \
        return bexpr<expr<T1>, bexpr<Ts2...>, FoamOp>(e1, be2);               \
    }

NVECTORFIELD_OP(+, plus)
NVECTORFIELD_OP(-, minus)
NVECTORFIELD_OP(*, multiply)
NVECTORFIELD_OP(/, divide)

#undef NVECTORFIELD_OP

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template <size_t idx, typename... T>
using cmpt = expr<cmpt_<idx, T...>>;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Utils

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "nVectorFieldIO.C"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
