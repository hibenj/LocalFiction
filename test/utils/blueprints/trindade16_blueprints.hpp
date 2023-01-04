//
// Created by Hien Benjamin on 04.01.2023.
//

#ifndef FICTION_TRINDADE16_BLUEPRINTS_HPP
#define FICTION_TRINDADE16_BLUEPRINTS_HPP

#include <mockturtle/views/names_view.hpp>

namespace blueprints
{

template <typename Ntk>
mockturtle::names_view<Ntk> t16_mux21()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto in0 = ntk.create_pi("in0");
    const auto in1 = ntk.create_pi("in1");
    const auto in2 = ntk.create_pi("in2");
    const auto w1 = ntk.create_not(in2);
    const auto w2 = ntk.create_and(in0, w1);
    const auto w3 = ntk.create_and(in1, in2);
    const auto out = ntk.create_or(w2, w3);

    ntk.create_po(out, "out");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> t16_xor2()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");
    const auto w1 = ntk.create_and(a, b);
    const auto w2 = ntk.create_or(a, b);
    const auto w3 = ntk.create_not(w1);
    const auto out = ntk.create_and(w2, w3);

    ntk.create_po(out, "out");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> t16_xnor2()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");
    const auto w1 = ntk.create_not(b);
    const auto w2 = ntk.create_and(a, w1);
    const auto w3 = ntk.create_not(a);
    const auto w4 = ntk.create_and(w3, b);
    const auto w5 = ntk.create_or(w2, w4);
    const auto out = ntk.create_not(w5);

    ntk.create_po(out, "out");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> t16_par_gen()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");
    const auto c = ntk.create_pi("c");
    const auto w1 = ntk.create_not(b);
    const auto w2 = ntk.create_and(c, w1);
    const auto w3 = ntk.create_not(c);
    const auto w4 = ntk.create_and(w3, b);
    const auto n1 = ntk.create_or(w2, w4);
    const auto w5 = ntk.create_not(n1);
    const auto w6 = ntk.create_and(a, w5);
    const auto w7 = ntk.create_not(a);
    const auto w8 = ntk.create_and(w7, n1);
    const auto parity = ntk.create_or(w6, w8);

    ntk.create_po(parity, "parity");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> t16_par_check()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");
    const auto c = ntk.create_pi("c");
    const auto p = ntk.create_pi("p");
    const auto w1 = ntk.create_not(b);
    const auto w2 = ntk.create_and(a, w1);
    const auto w3 = ntk.create_not(a);
    const auto w4 = ntk.create_and(w3, b);
    const auto n1 = ntk.create_or(w2, w4);
    const auto w5 = ntk.create_not(p);
    const auto w6 = ntk.create_and(w5, c);
    const auto w7 = ntk.create_not(a);
    const auto w8 = ntk.create_and(w7, p);
    const auto n2 = ntk.create_or(w6, w8);
    const auto w9 = ntk.create_not(n2);
    const auto w10 = ntk.create_and(n1, w9);
    const auto w11 = ntk.create_not(n1);
    const auto w12 = ntk.create_and(w11, n2);
    const auto parity_check = ntk.create_or(w10, w12);

    ntk.create_po(parity_check, "parity_check");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> t16_FA()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto x = ntk.create_pi("x");
    const auto y = ntk.create_pi("y");
    const auto cin = ntk.create_pi("cin");
    const auto w1 = ntk.create_and(x, y);
    const auto nx = ntk.create_not(x);
    const auto ny = ntk.create_not(y);
    const auto x_h = ntk.create_and(x, ny);
    const auto y_h = ntk.create_and(nx, y);
    const auto w2 = ntk.create_or(x_h, y_h);
    const auto w3 = ntk.create_and(cin, w2);
    const auto cout = ntk.create_or(w1, w3);
    const auto ncin = ntk.create_not(cin);
    const auto nw2 = ntk.create_not(w2);
    const auto cin_h = ntk.create_and(cin, nw2);
    const auto w2_h = ntk.create_and(ncin, w2);
    const auto s = ntk.create_or(cin_h, w2_h);

    ntk.create_po(s, "s");
    ntk.create_po(cout, "cout");

    return ntk;
}


}


#endif  // FICTION_TRINDADE16_BLUEPRINTS_HPP
