//
// Created by marcel on 24.05.21.
//

#ifndef FICTION_NETWORK_BLUEPRINTS_HPP
#define FICTION_NETWORK_BLUEPRINTS_HPP

#include <mockturtle/views/names_view.hpp>

namespace blueprints
{

template <typename Ntk>
mockturtle::names_view<Ntk> maj1_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");
    const auto c = ntk.create_pi("c");

    const auto m = ntk.create_maj(a, b, c);

    ntk.create_po(m, "f");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> maj4_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");
    const auto c = ntk.create_pi("c");
    const auto d = ntk.create_pi("d");
    const auto e = ntk.create_pi("e");

    const auto m1 = ntk.create_maj(a, b, c);
    const auto m2 = ntk.create_maj(b, c, d);
    const auto m3 = ntk.create_maj(c, d, e);
    const auto m4 = ntk.create_maj(m1, m2, m3);

    ntk.create_po(m1, "m1");
    ntk.create_po(m2, "m2");
    ntk.create_po(m3, "m3");
    ntk.create_po(m4, "m4");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> unbalanced_and_inv_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");

    const auto n = ntk.create_not(b);
    const auto f = ntk.create_and(a, n);

    ntk.create_po(f, "f");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> and_or_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");

    const auto f1 = ntk.create_and(a, b);
    const auto f2 = ntk.create_or(a, b);

    ntk.create_po(f1, "f1");
    ntk.create_po(f2, "f2");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> multi_output_and_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");

    const auto f = ntk.create_and(a, b);

    ntk.create_po(f, "f1");
    ntk.create_po(f, "f2");
    ntk.create_po(f, "f3");
    ntk.create_po(f, "f4");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> nary_operation_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi();
    const auto x2 = ntk.create_pi();
    const auto x3 = ntk.create_pi();
    const auto x4 = ntk.create_pi();
    const auto x5 = ntk.create_pi();

    const auto f1 = ntk.create_nary_and({x1, x2, x3});
    const auto f2 = ntk.create_nary_or({x2, x3, x4});
    const auto f3 = ntk.create_nary_xor({x3, x4, x5});

    ntk.create_po(f1);
    ntk.create_po(f2);
    ntk.create_po(f3);

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> constant_gate_input_maj_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi();
    const auto x2 = ntk.create_pi();

    const auto a1 = ntk.create_and(x1, x2);
    const auto m1 = ntk.create_maj(x1, x2, ntk.get_constant(true));
    const auto a2 = ntk.create_and(ntk.create_not(a1), m1);

    ntk.create_po(a2);

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> half_adder_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");

    const auto sum = ntk.create_and(x1, x2);
    const auto c   = ntk.create_xor(x1, x2);

    ntk.create_po(sum, "sum");
    ntk.create_po(c, "carry");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> full_adder_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("cin");

    const auto w1 = ntk.create_and(x1, x2);
    const auto w2 = ntk.create_xor(x1, x2);
    const auto w3 = ntk.create_and(x3, w2);

    const auto c   = ntk.create_or(w1, w3);
    const auto sum = ntk.create_xor(x3, w2);

    ntk.create_po(sum, "sum");
    ntk.create_po(c, "carry");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> mux21_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("c");

    const auto n1  = ntk.create_not(x3);
    const auto a1  = ntk.create_and(x1, n1);
    const auto a2  = ntk.create_and(x2, x3);
    const auto mux = ntk.create_or(a1, a2);

    ntk.create_po(mux, "f");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> se_coloring_corner_case_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");
    const auto x2 = ntk.create_pi("x2");

    const auto x3 = ntk.create_buf(x1);
    const auto x4 = ntk.create_buf(x2);

    const auto x5 = ntk.create_and(x3, x4);

    const auto x6 = ntk.create_buf(x5);

    const auto x7 = ntk.create_and(x3, x6);
    const auto x8 = ntk.create_and(x4, x6);

    ntk.create_po(x7, "f1");
    ntk.create_po(x8, "f2");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> fanout_substitution_corner_case_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");

    const auto fo1 = ntk.create_buf(x1);
    const auto fo2 = ntk.create_buf(fo1);
    const auto fo3 = ntk.create_buf(fo1);

    const auto n1 = ntk.create_not(fo2);
    const auto n2 = ntk.create_not(fo2);
    const auto n3 = ntk.create_not(fo3);
    const auto n4 = ntk.create_not(fo3);

    ntk.create_po(n1, "f1");
    ntk.create_po(n2, "f2");
    ntk.create_po(n3, "f3");
    ntk.create_po(n4, "f4");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> inverter_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");

    const auto fo1 = ntk.create_buf(x1);

    const auto n1 = ntk.create_not(fo1);
    const auto n2 = ntk.create_not(fo1);

    ntk.create_po(n1, "f1");
    ntk.create_po(n2, "f2");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> clpl()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");
    const auto x2 = ntk.create_pi("x2");
    const auto x3 = ntk.create_pi("x3");
    const auto x4 = ntk.create_pi("x4");
    const auto x5 = ntk.create_pi("x5");
    //    const auto x6 = ntk.create_pi("x6");
    //    const auto x7 = ntk.create_pi("x7");
    //    const auto x8 = ntk.create_pi("x8");
    //    const auto x9 = ntk.create_pi("x9");
    //    const auto x10 = ntk.create_pi("x10");
    //    const auto x11 = ntk.create_pi("x11");

    const auto a1 = ntk.create_and(x1, x2);
    const auto o1 = ntk.create_or(a1, x3);
    const auto a2 = ntk.create_and(o1, x4);
    const auto o2 = ntk.create_or(a2, x5);
    //    const auto a3 = ntk.create_and(o2, x6);
    //    const auto o3 = ntk.create_or(a3, x7);
    //    const auto a4 = ntk.create_and(o3, x8);
    //    const auto o4 = ntk.create_or(a4, x9);
    //    const auto a5 = ntk.create_and(o4, x10);
    //    const auto o5 = ntk.create_or(a5, x11);

    ntk.create_po(o1, "f1");
    ntk.create_po(o2, "f2");
    //    ntk.create_po(o3, "f3");
    //    ntk.create_po(o4, "f4");
    //    ntk.create_po(o5, "f5");

    return ntk;
}

template <typename Ntk>
Ntk one_to_five_path_difference_network()
{
    Ntk ntk{};

    const auto x1 = ntk.create_pi();
    const auto x2 = ntk.create_pi();

    const auto p1 = ntk.create_buf(x1);
    const auto p2 = ntk.create_buf(ntk.create_buf(ntk.create_buf(ntk.create_buf(ntk.create_buf(x2)))));

    const auto a = ntk.create_and(p1, p2);

    ntk.create_po(a);

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> nand_xnor_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");
    const auto x2 = ntk.create_pi("x2");

    const auto nand1 = ntk.create_nand(x1, x2);
    const auto nor1  = ntk.create_nor(x1, x2);
    const auto xor1  = ntk.create_xor(nand1, nor1);
    const auto xnor1 = ntk.create_xnor(nand1, nor1);

    ntk.create_po(xor1, "f1");
    ntk.create_po(xnor1, "f2");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> topolinano_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");
    const auto x2 = ntk.create_pi("x2");

    const auto fo1 = ntk.create_buf(x1);
    const auto fo2 = ntk.create_buf(fo1);
    const auto a1  = ntk.create_and(x1, x2);
    const auto o1  = ntk.create_or(a1, fo2);

    ntk.create_po(fo1, "f1");
    ntk.create_po(fo2, "f2");
    ntk.create_po(o1, "f3");

    return ntk;
}

template <typename Ntk>
Ntk xnor2()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    /*const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("d");*/

    const auto n1    = ntk.create_not(x1);
    const auto a1  = ntk.create_and(x1, n1);
    const auto n2    = ntk.create_not(x2);
    const auto a2  = ntk.create_and(x2, n2);
    const auto a3 = ntk.create_or(a1, a2);
    const auto a4 = ntk.create_not(a3);

    ntk.create_po(a4, "f");

    return ntk;
}

template <typename Ntk>
Ntk input_sort_view_one()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto n1  = ntk.create_not(x3);
    const auto a1  = ntk.create_and(x1, x3);
    const auto a2  = ntk.create_and(x2, n1);
    const auto mux = ntk.create_or(a1, a2);

    ntk.create_po(mux, "f");

    return ntk;
}

template <typename Ntk>
Ntk input_sort_view_two()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("d");
    const auto n1  = ntk.create_not(x2);
    const auto n2  = ntk.create_not(x3);
    const auto gate1 = ntk.create_and(x2, x3);
    const auto gate2 = ntk.create_and(x1, n2);
    const auto gate3 = ntk.create_and(x4, n1);
    const auto gate4 = ntk.create_and(gate1, gate2);
    const auto gate5 = ntk.create_and(gate2, gate3);
    const auto gate6 = ntk.create_and(gate4, gate5);

    ntk.create_po(gate6, "f");

    return ntk;
}

template <typename Ntk>
Ntk input_sort_view_three()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("d");
    const auto x1_n  = ntk.create_not(x1);
    const auto x2_n  = ntk.create_not(x2);
    const auto gate1 = ntk.create_and(x3, x4);
    const auto gate2 = ntk.create_or(x2, x3);
    const auto gate3 = ntk.create_and(x1_n, x2_n);
    const auto gate4 = ntk.create_and(gate1, gate3);
    const auto gate5 = ntk.create_and(gate4, gate2);

    ntk.create_po(gate5, "f");

    return ntk;
}

template <typename Ntk>
Ntk input_sort_view_four()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("d");
    const auto gate1 = ntk.create_and(x1, x3);
    const auto gate2 = ntk.create_and(x2, x4);
    const auto gate4 = ntk.create_and(gate1, gate2);

    ntk.create_po(gate4, "f");

    return ntk;
}

template <typename Ntk>
Ntk input_sort_view_bca()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto n2  = ntk.create_not(x2);
    const auto gate1 = ntk.create_and(x1, n2);
    const auto gate2 = ntk.create_and(x2, x3);
    const auto gate4 = ntk.create_and(gate1, gate2);

    ntk.create_po(gate4, "f");

    return ntk;
}
template <typename Ntk>
Ntk fanout_inv_blc()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("d");

    const auto n1  = ntk.create_not(x1);

    const auto a1  = ntk.create_and(n1, x2);
    const auto a2  = ntk.create_and(n1, x3);
    const auto a3  = ntk.create_and(n1, x4);
    const auto a4  = ntk.create_and(a1, a2);
    const auto a5  = ntk.create_and(a4, a3);
    const auto a6  = ntk.create_and(a5, n1);

    ntk.create_po(a6, "f");

    return ntk;
}

template <typename Ntk>
Ntk TEST_maj_one_buf()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("d");

    const auto m1 = ntk.create_maj(x1, x2, x3);
    const auto a1 = ntk.create_and(m1, x4);

    ntk.create_po(a1, "f");

    return ntk;
}

template <typename Ntk>
Ntk TEST_maj_two_pluslogic_buf()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");

    const auto m1 = ntk.create_maj(x1, x2, x3);
    const auto a1 = ntk.create_and(x1, x2);
    const auto o1 = ntk.create_or(x2, x3);

    const auto a2 = ntk.create_and(m1, a1);
    const auto a3 = ntk.create_and(a1, o1);

    const auto o2 = ntk.create_or(a2, a3);

    ntk.create_po(o2, "f");

    return ntk;
}

template <typename Ntk>
Ntk TEST_maj_two_plus_buf()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("d");

    const auto m1 = ntk.create_maj(x1, x2, x3);
    const auto a1 = ntk.create_or(x3, x4);

    const auto a2 = ntk.create_and(m1, a1);
    const auto a3 = ntk.create_and(x4, a2);

    ntk.create_po(a3, "f");

    return ntk;
}

template <typename Ntk>
Ntk TEST_maj_two_buf()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("c");
    const auto x5    = ntk.create_pi("c");

    const auto m1 = ntk.create_maj(x1, x2, x3);
    const auto a1 = ntk.create_and(m1, x4);
    const auto a2 = ntk.create_and(a1, x5);

    ntk.create_po(a2, "f");

    return ntk;
}

template <typename Ntk>
Ntk TEST_maj_three_buf()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("c");
    const auto x5    = ntk.create_pi("c");
    const auto x6    = ntk.create_pi("c");

    const auto m1 = ntk.create_maj(x1, x2, x3);
    const auto a1 = ntk.create_and(m1, x4);
    const auto a2 = ntk.create_and(a1, x5);
    const auto a3 = ntk.create_and(a2, x6);

    ntk.create_po(a3, "f");

    return ntk;
}

template <typename Ntk>
Ntk TEST_maj_four_buf()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("c");
    const auto x5    = ntk.create_pi("c");
    const auto x6    = ntk.create_pi("c");
    const auto x7    = ntk.create_pi("c");

    const auto m1 = ntk.create_maj(x1, x2, x3);
    const auto a1 = ntk.create_and(m1, x4);
    const auto a2 = ntk.create_and(a1, x5);
    const auto a3 = ntk.create_and(a2, x6);
    const auto a4 = ntk.create_and(a3, x7);

    ntk.create_po(a4, "f");

    return ntk;
}


template <typename Ntk>
Ntk TEST_maj_reroute()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("d");
    const auto x5    = ntk.create_pi("e");

    const auto a1 = ntk.create_and(x1, x2);
    const auto o1 = ntk.create_or(x1, x2);
    const auto a2 = ntk.create_and(a1, o1);

    const auto m1 = ntk.create_maj(x3, x4, x5);

    const auto a3 = ntk.create_and(m1, o1);

    const auto a4 = ntk.create_and(a2, a3);

    ntk.create_po(a4, "f");

    return ntk;
}

template <typename Ntk>
Ntk TEST_maj_reroute_aoig()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("d");
    const auto x5    = ntk.create_pi("e");

    const auto a1 = ntk.create_and(x1, x2);
    const auto o1 = ntk.create_or(x1, x2);
    const auto a2 = ntk.create_and(a1, o1);

    //const auto m1 = ntk.create_maj(x3, x4, x5);
    const auto a3 = ntk.create_and(x3, x4);
    const auto a4 = ntk.create_and(x3, x5);
    const auto a5 = ntk.create_and(x4, x5);
    const auto o2 = ntk.create_or(a3, a4);
    const auto m1 = ntk.create_or(o2, a5);

    const auto a6 = ntk.create_and(m1, o1);

    const auto a7 = ntk.create_and(a2, a6);

    ntk.create_po(a7, "f");

    return ntk;
}

template <typename Ntk>
Ntk TEST_maj_e_s_buf()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("d");
    const auto x5    = ntk.create_pi("e");
    const auto x6    = ntk.create_pi("f");

    const auto a1 = ntk.create_and(x4, x5);
    const auto m1 = ntk.create_maj(x1, x2, x3);
    const auto a2 = ntk.create_and(x6, m1);
    const auto a3 = ntk.create_and(a1, a2);

    ntk.create_po(a3, "f");

    return ntk;
}

template <typename Ntk>
Ntk TEST_maj_maj_buf()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");

    const auto m1 = ntk.create_maj(x1, x2, x3);

    const auto a1 = ntk.create_and(x1, x2);
    const auto a2 = ntk.create_and(x2, x3);

    const auto m2 = ntk.create_maj(m1, a1, a2);

    ntk.create_po(m2, "f");

    return ntk;
}

template <typename Ntk>
Ntk TESTb()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");
    const auto x4    = ntk.create_pi("d");
    const auto n1    = ntk.create_not(x1);

    const auto a1 = ntk.create_and(n1, x2);
    const auto a2 = ntk.create_and(x3, x4);
    const auto a3 = ntk.create_and(a1, x3);
    const auto a4 = ntk.create_and(a2, x2);
    const auto a5 = ntk.create_and(a3, a4);

    ntk.create_po(a5, "f");

    return ntk;
}

template <typename Ntk>
Ntk mini()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");

    const auto a1 = ntk.create_and(x1, x2);
    const auto o1 = ntk.create_or(x2, x3);
    const auto o2 = ntk.create_or(a1, o1);

    ntk.create_po(o2, "f");

    return ntk;
}

template <typename Ntk>
Ntk TESTc()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");

    const auto a1 = ntk.create_and(x1, x2);
    const auto a2 = ntk.create_and(x1, x3);

    const auto a4 = ntk.create_and(a1, a2);
    const auto a5 = ntk.create_and(x1, a4);
    const auto a6 = ntk.create_and(x1, a5);

    ntk.create_po(a6, "f");

    return ntk;
}

template <typename Ntk>
Ntk onebitAdderAOIG()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");

    const auto o1 = ntk.create_or(x2, x3);
    const auto a1 = ntk.create_and(x2, x3);

    const auto n1    = ntk.create_not(a1);

    const auto a2 = ntk.create_and(o1, n1);
    const auto o2 = ntk.create_or(x1, a2);
    const auto a3 = ntk.create_and(x1, a2);

    const auto n2    = ntk.create_not(a3);

    const auto a4 = ntk.create_and(o2, a3);
    const auto o3 = ntk.create_or(x2, a4);

    ntk.create_po(a4, "f");
    ntk.create_po(o3, "g");

    return ntk;
}

template <typename Ntk>
Ntk onebitAdderMajority()
{
    Ntk ntk{};
    const auto a    = ntk.create_pi("a");
    const auto b    = ntk.create_pi("b");
    const auto cin    = ntk.create_pi("c");
    const auto n_cin    = ntk.create_not(cin);

    const auto m11 = ntk.create_and(a, b);
    const auto m12 = ntk.create_and(a, cin);
    const auto m13 = ntk.create_and(b, cin);
    const auto m14 = ntk.create_or(m11, m12);
    const auto m1 = ntk.create_or(m13, m14);
    const auto n_m1 = ntk.create_not(m1);
    const auto m21 = ntk.create_and(n_m1, a);
    const auto m22 = ntk.create_and(n_m1, b);
    const auto m23 = ntk.create_or(m21, m22);
    const auto m2 = ntk.create_or(m23, m11);
    const auto m31 = ntk.create_and(n_cin, m2);
    const auto m32 = ntk.create_and(n_cin, m1);
    const auto m33 = ntk.create_and(m2, m1);
    const auto m34 = ntk.create_or(m31, m32);
    const auto m3 = ntk.create_or(m33, m34);
    ntk.create_po(m3, "f");

    /*assign M11 = A & B;
    assign M12 = A & Cin;
    assign M13 = B & Cin;
    assign M14 = M11 | M12;
    assign M1 = M13 | M14;
    assign M21 = ~M1 & A;
    assign M22 = ~M1 & B;
    assign M23 = M21 | M22;
    assign M2 = M23 | M11;
    assign M31 = ~Cin & M2;
    assign M32 = ~Cin & M1;
    assign M33 = M2 & M1;
    assign M34 = M31 | M32;
    assign M3 = M33 | M34;*/

    return ntk;
}


template <typename Ntk>
Ntk FA()
{
    Ntk ntk{};
    const auto x    = ntk.create_pi("a");
    const auto y    = ntk.create_pi("b");
    const auto cin    = ntk.create_pi("c");

    const auto a1 = ntk.create_and(x, y);
    const auto xor1 = ntk.create_xor(x, y);
    const auto a2 = ntk.create_and(xor1, cin);
    const auto o1 = ntk.create_or(a1, a2);
    const auto xor2 = ntk.create_xor(cin, xor1);



    ntk.create_po(o1, "cout");
    ntk.create_po(xor2, "s");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> seq_one()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");

    const auto r1_o = ntk.create_ro();

    const auto xo1 = ntk.create_xor(x1, r1_o);

    ntk.create_ri(xo1);

    ntk.create_po(xo1, "cout");


    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> seq_two()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");

    const auto r1_o = ntk.create_ro();

    const auto r2_o = ntk.create_ro();

    const auto xo1 = ntk.create_xor(x1, r1_o);

    const auto a1 = ntk.create_and(r1_o, r2_o);

    ntk.create_po(a1, "cout");
    ntk.create_ri(xo1);
    ntk.create_ri(a1);


    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> seq_two_z()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");

    const auto r1_o = ntk.create_ro();

    const auto r2_o = ntk.create_ro();

    const auto xo1 = ntk.create_xor(x1, r1_o);

    const auto a1 = ntk.create_and(x1, r2_o);

    ntk.create_po(a1, "cout");
    ntk.create_ri(xo1);
    ntk.create_ri(a1);


    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> seq_three()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");

    const auto r1_o = ntk.create_ro();

    const auto r2_o = ntk.create_ro();

    const auto r3_o = ntk.create_ro();

    const auto xo1 = ntk.create_xor(x1, r1_o);

    const auto a1 = ntk.create_and(x1, r2_o);

    const auto a2 = ntk.create_and(a1, r3_o);

    ntk.create_po(a2, "cout");
    ntk.create_ri(xo1);
    ntk.create_ri(a1);
    ntk.create_ri(a2);



    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> seq_four()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");

    const auto r1_o = ntk.create_ro();

    const auto r2_o = ntk.create_ro();

    const auto r3_o = ntk.create_ro();

    const auto r4_o = ntk.create_ro();

    const auto xo1 = ntk.create_xor(x1, r1_o);

    const auto a1 = ntk.create_and(x1, r2_o);

    const auto a2 = ntk.create_and(a1, r3_o);

    const auto a3 = ntk.create_and(r4_o, a2);

    ntk.create_po(a2, "cout");
    ntk.create_ri(xo1);
    ntk.create_ri(a1);
    ntk.create_ri(a2);
    ntk.create_ri(a3);



    return ntk;
}
template <typename Ntk>
mockturtle::names_view<Ntk> multi_out()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");

    const auto a = ntk.create_and(x1, x2);
    const auto o = ntk.create_or(x1, x2);

    ntk.create_po(a, "cout");
    ntk.create_po(o, "cout");



    return ntk;
}
template <typename Ntk>
mockturtle::names_view<Ntk> majority()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x0 = ntk.create_pi("z");
    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("c");
    const auto x4 = ntk.create_pi("d");
    const auto n0 = ntk.create_not(x0);
    const auto n2 = ntk.create_not(x2);
    const auto n4 = ntk.create_not(x4);

    const auto a1 = ntk.create_and(x2, x4);
    const auto na1 = ntk.create_not(a1);
    const auto a2= ntk.create_and(x1, a1);
    const auto na2 = ntk.create_not(a2);
    const auto a3 = ntk.create_and(n0, na2);
    const auto na3 = ntk.create_not(a3);
    const auto a4 = ntk.create_and(n2, n4);
    const auto na4 = ntk.create_not(a4);
    const auto a5 = ntk.create_and(x1, na4);
    const auto na5 = ntk.create_not(a5);
    const auto a6 = ntk.create_and(na1, na5);
    const auto na6 = ntk.create_not(a6);
    const auto a7 = ntk.create_and(na3, na6);
    const auto o = ntk.create_or(x3, a7);

    /*assign new_n7_ = pi2 & pi4;
    assign new_n8_ = pi1 & new_n7_;
    assign new_n9_ = ~pi0 & ~new_n8_;
    assign new_n10_ = ~pi2 & ~pi4;
    assign new_n11_ = pi1 & ~new_n10_;
    assign new_n12_ = ~new_n7_ & ~new_n11_;
    assign new_n13_ = ~new_n9_ & ~new_n12_;
    assign po0 = pi3 | new_n13_;*/

    ntk.create_po(o, "cout");



    return ntk;
}


template <typename Ntk>
mockturtle::names_view<Ntk> twobitAdderMaj()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto A = ntk.create_pi("z");
    const auto B = ntk.create_pi("a");
    const auto C = ntk.create_pi("b");
    const auto D = ntk.create_pi("c");
    const auto E = ntk.create_pi("d");

    const auto a1 = ntk.create_and(A, B);
    const auto na1 = ntk.create_not(a1);
    const auto a2 = ntk.create_and(A, C);
    const auto na2 = ntk.create_not(a2);
    const auto a3 = ntk.create_and(na1, na2);
    const auto na3 = ntk.create_not(a3);
    const auto a4 = ntk.create_and(B, C);
    const auto na4 = ntk.create_not(a4);
    const auto a5 = ntk.create_and(a3, na4);
    const auto na5 = ntk.create_not(a5);
    const auto a6 = ntk.create_and(na2, na4);
    const auto na6 = ntk.create_not(a6);
    const auto a7 = ntk.create_and(na1, a6);
    const auto na7 = ntk.create_not(a7);
    const auto a8 = ntk.create_and(na5, na7);
    const auto na8 = ntk.create_not(a8);
    const auto a9 = ntk.create_and(C, na5);
    const auto na9 = ntk.create_not(a9);
    const auto a10 = ntk.create_and(na8, na9);
    const auto na10 = ntk.create_not(a10);
    const auto a11 = ntk.create_and(C, na5);
    const auto na11 = ntk.create_not(a11);
    const auto a12 = ntk.create_or(na10, a11);
    const auto na12 = ntk.create_not(a12);
    const auto a13 = ntk.create_and(D, E);
    const auto na13 = ntk.create_not(a13);
    const auto a14 = ntk.create_and(D, na5);
    const auto na14 = ntk.create_not(a14);
    const auto a15 = ntk.create_and(na13, na14);
    const auto na15 = ntk.create_not(a15);
    const auto a16 = ntk.create_and(E, na5);
    const auto na16 = ntk.create_not(a16);
    const auto a17 = ntk.create_and(a15, na16);
    const auto na17 = ntk.create_not(a17);
    const auto a18 = ntk.create_and(na13, na16);
    const auto na18 = ntk.create_not(a18);
    const auto a19 = ntk.create_and(na14, a18);
    const auto na19 = ntk.create_not(a19);
    const auto a20 = ntk.create_and(na17, na19);
    const auto na20 = ntk.create_not(a20);
    const auto a21 = ntk.create_and(na5, na17);
    const auto na21 = ntk.create_not(a21);
    const auto a22 = ntk.create_and(na5, na19);
    const auto na22 = ntk.create_not(a22);
    const auto a23 = ntk.create_and(na21, na22);
    const auto na23 = ntk.create_not(a23);
    const auto a24 = ntk.create_or(a20, na23);
    const auto na24 = ntk.create_not(a24);
    /*
assign new_n8_ = A & B;1
assign new_n9_ = A & C;2
assign new_n10_ = ~new_n8_ & ~new_n9_;3
assign new_n11_ = B & C;4
assign new_n12_ = new_n10_ & ~new_n11_;5
assign new_n13_ = ~new_n9_ & ~new_n11_;6
assign new_n14_ = ~new_n8_ & new_n13_;7
assign new_n15_ = ~new_n12_ & ~new_n14_;8
assign new_n16_ = C & ~new_n12_;9
assign new_n17_ = ~new_n15_ & ~new_n16_;10
assign new_n18_ = C & ~new_n14_;11
assign M3 = ~new_n17_ | new_n18_;12
assign new_n20_ = D & E;13
assign new_n21_ = D & ~new_n12_;14
assign new_n22_ = ~new_n20_ & ~new_n21_;15
assign new_n23_ = E & ~new_n12_;16
assign new_n24_ = new_n22_ & ~new_n23_;17
assign new_n25_ = ~new_n20_ & ~new_n23_;18
assign new_n26_ = ~new_n21_ & new_n25_;19
assign new_n27_ = ~new_n24_ & ~new_n26_;20
assign new_n28_ = ~new_n12_ & ~new_n24_;21
assign new_n29_ = ~new_n12_ & ~new_n26_;22
assign new_n30_ = ~new_n28_ & ~new_n29_;
assign M6 = new_n27_ | ~new_n30_;*/

    ntk.create_po(na24, "cout");



    return ntk;
}

}  // namespace blueprints

#endif  // FICTION_NETWORK_BLUEPRINTS_HPP
