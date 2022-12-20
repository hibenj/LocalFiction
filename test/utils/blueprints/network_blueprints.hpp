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
Ntk fanout_inv_blc_one()
{
    Ntk ntk{};
    const auto x1    = ntk.create_pi("a");
    const auto x2    = ntk.create_pi("b");
    const auto x3    = ntk.create_pi("c");

    const auto n1  = ntk.create_not(x1);

    const auto a1  = ntk.create_and(n1, x2);
    const auto a2  = ntk.create_and(n1, x3);
    const auto a3  = ntk.create_and(a1, a2);


    ntk.create_po(a3, "f");

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
    const auto x1    = ntk.create_pi("m1");
    const auto x2    = ntk.create_pi("m2");
    const auto x3    = ntk.create_pi("m3");
    const auto x4    = ntk.create_pi("d");
    const auto x5    = ntk.create_pi("e");
    const auto x6    = ntk.create_pi("f");
    const auto x7    = ntk.create_pi("g");

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
mockturtle::names_view<Ntk> seq_five()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");

    const auto r1_o = ntk.create_ro();

    const auto r2_o = ntk.create_ro();

    const auto r3_o = ntk.create_ro();

    const auto r4_o = ntk.create_ro();

    const auto r5_o = ntk.create_ro();

    const auto xo1 = ntk.create_xor(x1, r1_o);

    const auto a1 = ntk.create_and(x1, r2_o);

    const auto a2 = ntk.create_and(a1, r3_o);

    const auto a3 = ntk.create_and(r4_o, a2);

    const auto a4 = ntk.create_and(r5_o, a3);

    const auto bridge_out_neg_1 = ntk.create_not(r5_o);
    const auto bridge_out_1 = ntk.create_not(bridge_out_neg_1);

    ntk.create_po(bridge_out_1, "r5_o");
    ntk.create_ri(xo1);
    ntk.create_ri(a1);
    ntk.create_ri(a2);
    ntk.create_ri(a3);
    ntk.create_ri(a4);



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

template <typename Ntk>
mockturtle::names_view<Ntk> b1_r2()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x0 = ntk.create_pi("a");
    const auto x1 = ntk.create_pi("b");
    const auto x2 = ntk.create_pi("c");
    const auto n0 = ntk.create_not(x0);
    const auto n1 = ntk.create_not(x1);
    const auto n2 = ntk.create_not(x2);

    const auto a1 = ntk.create_and(x0, x1);
    const auto na1 = ntk.create_not(a1);
    const auto a2 = ntk.create_and(n0, n1);
    const auto na2 = ntk.create_not(a2);
    const auto a3 = ntk.create_and(na1, na2);
    //const auto na3 = ntk.create_not(a3);
    const auto a4 = ntk.create_and(n2, a1);
    //const auto na4 = ntk.create_not(a4);
    const auto a5 = ntk.create_and(x2, a2);
    //const auto na5 = ntk.create_not(a5);
    const auto a6 = ntk.create_or(a4, a5);
    //const auto na6 = ntk.create_not(a6);


    //const auto o = ntk.create_or(x1, x2);

    ntk.create_po(a3, "cout1");
    ntk.create_po(a6, "cout2");
    ntk.create_po(n2, "cout3");
    ntk.create_po(x2, "cout4");

    /*
assign new_n8_ = pi0 & pi1;1
assign new_n9_ = ~pi0 & ~pi1;2
assign po1 = ~new_n8_ & ~new_n9_;3
assign new_n11_ = ~pi2 & new_n8_;4
assign new_n12_ = pi2 & new_n9_;5
assign po2 = new_n11_ | new_n12_;6
assign po3 = ~pi2;7
assign po0 = pi2;8
endmodule*/


    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> b01()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto LINE1 = ntk.create_pi("LINE1");
    const auto LINE2 = ntk.create_pi("LINE2");
    const auto OVERFLW_REG = ntk.create_ro();
    const auto STATO_REG_2_ = ntk.create_ro();
    const auto STATO_REG_1_ = ntk.create_ro();
    const auto STATO_REG_0_ = ntk.create_ro();
    const auto OUTP_REG = ntk.create_ro();


    const auto U37 = ntk.create_or(LINE2, LINE1);
    const auto U38 = ntk.create_not(STATO_REG_2_);
    const auto U39 = ntk.create_not(STATO_REG_1_);
    const auto U40 = ntk.create_not(LINE2);
    const auto U41 = ntk.create_not(LINE1);
    const auto U42 = ntk.create_not(STATO_REG_0_);
    const auto U43_h = ntk.create_and(STATO_REG_1_, U42);
    const auto U43 = ntk.create_not(U43_h);
    const auto U47_h = ntk.create_and(LINE1, LINE2);
    const auto U47 = ntk.create_not(U47_h);
    const auto U48_h = ntk.create_and(STATO_REG_2_, U43);
    const auto U48 = ntk.create_not(U48_h);
    const auto U49 = ntk.create_not(U37);
    const auto U50_h = ntk.create_and(U49, U42);
    const auto U50 = ntk.create_not(U50_h);
    const auto U51 = ntk.create_not(U47);
    const auto U52 = ntk.create_not(U43);
    const auto U53_h = ntk.create_and(U47, U43);
    const auto U53 = ntk.create_not(U53_h);
    const auto U54_h = ntk.create_and(STATO_REG_2_, U47);
    const auto U54 = ntk.create_not(U54_h);
    const auto U55_h1 = ntk.create_and(STATO_REG_0_, U39);
    const auto U55_h2 = ntk.create_and(U55_h1, U47);
    const auto U55 = ntk.create_not(U55_h2);
    const auto U56_h = ntk.create_and(U52, U54);
    const auto U56 = ntk.create_not(U56_h);
    const auto U58 = ntk.create_not(U48);
    const auto U59_h = ntk.create_and(U53, U38);
    const auto U59 = ntk.create_not(U59_h);
    const auto U60_h1 = ntk.create_and(U50, U39);
    const auto U60_h2 = ntk.create_and(U60_h1, STATO_REG_2_);
    const auto U60 = ntk.create_not(U60_h2);
    const auto U61_h = ntk.create_and(STATO_REG_1_, U49);
    const auto U61 = ntk.create_not(U61_h);
    const auto U62_h = ntk.create_and(U37, U42);
    const auto U62 = ntk.create_not(U62_h);
    const auto U63_h = ntk.create_and(STATO_REG_0_, U47);
    const auto U63 = ntk.create_not(U63_h);
    const auto U64_h = ntk.create_and(U51, U42);
    const auto U64 = ntk.create_not(U64_h);
    const auto U65_h1 = ntk.create_and(U64, U63);
    const auto U65_h2 = ntk.create_and(U39, U38);
    const auto U65_h3 = ntk.create_and(U65_h1, U65_h2);
    const auto U65 = ntk.create_not(U65_h3);
    const auto U66_h1 = ntk.create_and(U43, U37);
    const auto U66_h2 = ntk.create_and(U66_h1, STATO_REG_2_);
    const auto U66 = ntk.create_not(U66_h2);
    const auto U68_h = ntk.create_and(U51, U52);
    const auto U68 = ntk.create_not(U68_h);
    const auto U69_h = ntk.create_and(LINE1, U40);
    const auto U69 = ntk.create_not(U69_h);
    const auto U70_h = ntk.create_and(LINE2, U41);
    const auto U70 = ntk.create_not(U70_h);
    const auto U34_h1 = ntk.create_and(STATO_REG_1_, U38);
    const auto U34 = ntk.create_and(U34_h1, STATO_REG_0_);
    const auto U45_h = ntk.create_and(U60, U59);
    const auto U45 = ntk.create_not(U45_h);
    const auto U46_h = ntk.create_and(U70, U69);
    const auto U46 = ntk.create_not(U46_h);
    const auto U57_h1 = ntk.create_and(U62, U61);
    const auto U57_h2 = ntk.create_and(U57_h1, STATO_REG_2_);
    const auto U57 = ntk.create_not(U57_h2);
    const auto U67_h = ntk.create_and(U34, U47);
    const auto U67 = ntk.create_not(U67_h);
    const auto U71 = ntk.create_not(U46);
    const auto U72_h = ntk.create_and(U58, U71);
    const auto U72 = ntk.create_not(U72_h);
    const auto U73_h = ntk.create_and(U46, U48);
    const auto U73 = ntk.create_not(U73_h);
    const auto U35_h1 = ntk.create_and(U68, U67);
    const auto U35_h2 = ntk.create_and(U66, U65);
    const auto U35_h3 = ntk.create_and(U35_h1, U35_h2);
    const auto U35 = ntk.create_not(U35_h3);
    const auto U36_h1 = ntk.create_and(U57, U55);
    const auto U36_h2 = ntk.create_and(U36_h1, U56);
    const auto U36 = ntk.create_not(U36_h2);
    const auto U44_h = ntk.create_and(U73, U72);
    const auto U44 = ntk.create_not(U44_h);

    const auto bridge_out_neg_1 = ntk.create_not(OUTP_REG);
    const auto bridge_out_1 = ntk.create_not(bridge_out_neg_1);

    const auto bridge_out_neg_2 = ntk.create_not(OVERFLW_REG);
    const auto bridge_out_2 = ntk.create_not(bridge_out_neg_2);

    ntk.create_po(bridge_out_1, "OUTP_REG");
    ntk.create_po(bridge_out_2, "OVERFLW_REG");
    ntk.create_ri(U34);
    ntk.create_ri(U45);
    ntk.create_ri(U36);
    ntk.create_ri(U35);
    ntk.create_ri(U44);

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> b01_C()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto LINE1 = ntk.create_pi("LINE1");
    const auto LINE2 = ntk.create_pi("LINE2");
    const auto OVERFLW_REG_SCAN_IN = ntk.create_pi("OVERFLW_REG_SCAN_IN");
    const auto STATO_REG_2__SCAN_IN = ntk.create_pi("STATO_REG_2__SCAN_IN");
    const auto STATO_REG_1__SCAN_IN = ntk.create_pi("STATO_REG_1__SCAN_IN");
    const auto STATO_REG_0__SCAN_IN = ntk.create_pi("STATO_REG_0__SCAN_IN");
    const auto OUTP_REG_SCAN_IN = ntk.create_pi("OUTP_REG_SCAN_IN");
    const auto U37 = ntk.create_or(LINE2, LINE1);
    const auto U38 = ntk.create_not(STATO_REG_2__SCAN_IN);
    const auto U39 = ntk.create_not(STATO_REG_1__SCAN_IN);
    const auto U40 = ntk.create_not(LINE2);
    const auto U41 = ntk.create_not(LINE1);
    const auto U42 = ntk.create_not(STATO_REG_0__SCAN_IN);
    const auto U43_h = ntk.create_and(U42, STATO_REG_1__SCAN_IN);
    const auto U43 = ntk.create_not(U43_h);
    const auto U47_h = ntk.create_and(LINE1, LINE2);
    const auto U47 = ntk.create_not(U47_h);
    const auto U48_h = ntk.create_and(U43, STATO_REG_2__SCAN_IN);
    const auto U48 = ntk.create_not(U48_h);
    const auto U49 = ntk.create_not(U37);
    const auto U50_h = ntk.create_and(U49, U42);
    const auto U50 = ntk.create_not(U50_h);
    const auto U51 = ntk.create_not(U47);
    const auto U52 = ntk.create_not(U43);
    const auto U53_h = ntk.create_and(U47, U43);
    const auto U53 = ntk.create_not(U53_h);
    const auto U54_h = ntk.create_and(U47, STATO_REG_2__SCAN_IN);
    const auto U54 = ntk.create_not(U54_h);
    const auto U55_h1 = ntk.create_and(U39, U47);
    const auto U55_h2 = ntk.create_and(U55_h1, STATO_REG_0__SCAN_IN);
    const auto U55 = ntk.create_not(U55_h2);
    const auto U56_h = ntk.create_and(U52, U54);
    const auto U56 = ntk.create_not(U56_h);
    const auto U58 = ntk.create_not(U48);
    const auto U59_h = ntk.create_and(U53, U38);
    const auto U59 = ntk.create_not(U59_h);
    const auto U60_h1 = ntk.create_and(U50, U39);
    const auto U60_h2 = ntk.create_and(U60_h1, STATO_REG_2__SCAN_IN);
    const auto U60 = ntk.create_not(U60_h2);
    const auto U61_h = ntk.create_and(U49, STATO_REG_1__SCAN_IN);
    const auto U61 = ntk.create_not(U61_h);
    const auto U62_h = ntk.create_and(U37, U42);
    const auto U62 = ntk.create_not(U62_h);
    const auto U63_h = ntk.create_and(U47, STATO_REG_0__SCAN_IN);
    const auto U63 = ntk.create_not(U63_h);
    const auto U64_h = ntk.create_and(U51, U42);
    const auto U64 = ntk.create_not(U64_h);
    const auto U65_h1 = ntk.create_and(U64, U63);
    const auto U65_h2 = ntk.create_and(U39, U38);
    const auto U65_h3 = ntk.create_and(U65_h1, U65_h2);
    const auto U65 = ntk.create_not(U65_h3);
    const auto U66_h1 = ntk.create_and(U43, U37);
    const auto U66_h2 = ntk.create_and(U66_h1, STATO_REG_2__SCAN_IN);
    const auto U66 = ntk.create_not(U66_h2);
    const auto U68_h = ntk.create_and(U51, U52);
    const auto U68 = ntk.create_not(U68_h);
    const auto U69_h = ntk.create_and(LINE1, U40);
    const auto U69 = ntk.create_not(U69_h);
    const auto U70_h = ntk.create_and(LINE2, U41);
    const auto U70 = ntk.create_not(U70_h);
    const auto U34_h1 = ntk.create_and(U38, STATO_REG_1__SCAN_IN);
    const auto U34 = ntk.create_and(U34_h1, STATO_REG_0__SCAN_IN);
    const auto U45_h = ntk.create_and(U60, U59);
    const auto U45 = ntk.create_not(U45_h);
    const auto U46_h = ntk.create_and(U70, U69);
    const auto U46 = ntk.create_not(U46_h);
    const auto U57_h1 = ntk.create_and(U62, U61);
    const auto U57_h2 = ntk.create_and(U57_h1, STATO_REG_2__SCAN_IN);
    const auto U57 = ntk.create_not(U57_h2);
    const auto U67_h = ntk.create_and(U34, U47);
    const auto U67 = ntk.create_not(U67_h);
    const auto U71 = ntk.create_not(U46);
    const auto U72_h = ntk.create_and(U58, U71);
    const auto U72 = ntk.create_not(U72_h);
    const auto U73_h = ntk.create_and(U46, U48);
    const auto U73 = ntk.create_not(U73_h);
    const auto U35_h1 = ntk.create_and(U68, U67);
    const auto U35_h2 = ntk.create_and(U66, U65);
    const auto U35_h3 = ntk.create_and(U35_h1, U35_h2);
    const auto U35 = ntk.create_not(U35_h3);
    const auto U36_h1 = ntk.create_and(U57, U55);
    const auto U36_h2 = ntk.create_and(U36_h1, U56);
    const auto U36 = ntk.create_not(U36_h2);
    const auto U44_h = ntk.create_and(U73, U72);
    const auto U44 = ntk.create_not(U44_h);

    const auto bridge_out_neg_1 = ntk.create_not(OUTP_REG_SCAN_IN);
    const auto bridge_out_1 = ntk.create_not(bridge_out_neg_1);
    const auto bridge_out_neg_2 = ntk.create_not(OVERFLW_REG_SCAN_IN);
    const auto bridge_out_2 = ntk.create_not(bridge_out_neg_2);

    ntk.create_po(bridge_out_1, "OUTP_REG_SCAN_IN");
    ntk.create_po(bridge_out_2, "OVERFLW_REG_SCAN_IN");
    ntk.create_po(U45, "U45");
    ntk.create_po(U36, "U36");
    ntk.create_po(U35, "U35");
    ntk.create_po(U44, "U44");
    ntk.create_po(U34, "U34");


    return ntk;
}

template <typename Ntk>
    mockturtle::names_view<Ntk> b02()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto LINEA = ntk.create_pi("LINEA");

    const auto U_REG = ntk.create_ro();
    const auto STATO_REG_2_ = ntk.create_ro();
    const auto STATO_REG_1_ = ntk.create_ro();
    const auto STATO_REG_0_ = ntk.create_ro();

    const auto U34 = ntk.create_not(STATO_REG_2_);
    const auto U35 = ntk.create_not(STATO_REG_0_);
    const auto U36 = ntk.create_not(LINEA);
    const auto U37 = ntk.create_not(STATO_REG_1_);
    const auto U40 = ntk.create_or(STATO_REG_2_, LINEA);
    const auto U41_h = ntk.create_and(STATO_REG_0_, U40);
    const auto U41 = ntk.create_not(U41_h);
    const auto U43 = ntk.create_or(LINEA, STATO_REG_1_);
    const auto U44_h = ntk.create_and(U34, U43);
    const auto U44 = ntk.create_not(U44_h);
    const auto U45_h = ntk.create_and(LINEA, U34);
    const auto U45 = ntk.create_not(U45_h);
    const auto U46_h = ntk.create_and(STATO_REG_0_, U45);
    const auto U46 = ntk.create_not(U46_h);
    const auto U47_h1 = ntk.create_or(LINEA, STATO_REG_0_);
    const auto U47 = ntk.create_or(U47_h1, STATO_REG_2_);
    const auto U48_h = ntk.create_and(U46, U37);
    const auto U48 = ntk.create_not(U48_h);
    const auto U49_h = ntk.create_and(LINEA, U34);
    const auto U49 = ntk.create_not(U49_h);
    const auto U50_h = ntk.create_and(STATO_REG_2_, U36);
    const auto U50 = ntk.create_not(U50_h);
    const auto U51_h = ntk.create_and(STATO_REG_0_, U44);
    const auto U51 = ntk.create_not(U51_h);
    const auto U52_h1 = ntk.create_and(STATO_REG_1_, U34);
    const auto U52_h2 = ntk.create_and(U52_h1, U35);
    const auto U52 = ntk.create_not(U52_h2);
    const auto U31_h1 = ntk.create_and(U35, U37);
    const auto U31 = ntk.create_and(U31_h1, STATO_REG_2_);
    const auto U32_h = ntk.create_and(U48, U47);
    const auto U32 = ntk.create_not(U32_h);
    const auto U38_h = ntk.create_and(U52, U51);
    const auto U38 = ntk.create_not(U38_h);
    const auto U39_h1 = ntk.create_and(U50, U49);
    const auto U39_h2 = ntk.create_and(U39_h1, U35);
    const auto U39 = ntk.create_not(U39_h2);
    const auto U42_h = ntk.create_and(STATO_REG_1_, U39);
    const auto U42 = ntk.create_not(U42_h);
    const auto U33_h = ntk.create_and(U42, U41);
    const auto U33 = ntk.create_not(U33_h);

    const auto bridge_out_neg = ntk.create_not(U_REG);
    const auto bridge_out = ntk.create_not(bridge_out_neg);


    ntk.create_po(bridge_out, "U_REG");
    ntk.create_ri(U31);
    ntk.create_ri(U33);
    ntk.create_ri(U38);
    ntk.create_ri(U32);

    return ntk;
}

template <typename Ntk>
    mockturtle::names_view<Ntk> b02_C()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto U_REG_SCAN_IN = ntk.create_pi("U_REG_SCAN_IN");
    const auto LINEA = ntk.create_pi("LINEA");

    const auto STATO_REG_2__SCAN_IN = ntk.create_pi("STATO_REG_2__SCAN_IN");
    const auto STATO_REG_1__SCAN_IN = ntk.create_pi("STATO_REG_1__SCAN_IN");
    const auto STATO_REG_0__SCAN_IN = ntk.create_pi("STATO_REG_0__SCAN_IN");

    const auto U34 = ntk.create_not(STATO_REG_2__SCAN_IN);
    const auto U35 = ntk.create_not(STATO_REG_0__SCAN_IN);
    const auto U36 = ntk.create_not(LINEA);
    const auto U37 = ntk.create_not(STATO_REG_1__SCAN_IN);
    const auto U40 = ntk.create_or(LINEA, STATO_REG_2__SCAN_IN);
    const auto U41_h = ntk.create_and(U40, STATO_REG_0__SCAN_IN);
    const auto U41 = ntk.create_not(U41_h);
    const auto U43 = ntk.create_or(LINEA, STATO_REG_1__SCAN_IN);
    const auto U44_h = ntk.create_and(U34, U43);
    const auto U44 = ntk.create_not(U44_h);
    const auto U45_h = ntk.create_and(LINEA, U34);
    const auto U45 = ntk.create_not(U45_h);
    const auto U46_h = ntk.create_and(U45, STATO_REG_0__SCAN_IN);
    const auto U46 = ntk.create_not(U46_h);
    const auto U47_h1 = ntk.create_or(LINEA, STATO_REG_2__SCAN_IN);
    const auto U47 = ntk.create_or(U47_h1, STATO_REG_0__SCAN_IN);
    const auto U48_h = ntk.create_and(U46, U37);
    const auto U48 = ntk.create_not(U48_h);
    const auto U49_h = ntk.create_and(LINEA, U34);
    const auto U49 = ntk.create_not(U49_h);
    const auto U50_h = ntk.create_and(U36, STATO_REG_2__SCAN_IN);
    const auto U50 = ntk.create_not(U50_h);
    const auto U51_h = ntk.create_and(U44, STATO_REG_0__SCAN_IN);
    const auto U51 = ntk.create_not(U51_h);
    const auto U52_h1 = ntk.create_and(U34, U35);
    const auto U52_h2 = ntk.create_and(U52_h1, STATO_REG_1__SCAN_IN);
    const auto U52 = ntk.create_not(U52_h2);
    const auto U31_h1 = ntk.create_and(U35, U37);
    const auto U31 = ntk.create_and(U31_h1, STATO_REG_2__SCAN_IN);
    const auto U32_h = ntk.create_and(U48, U47);
    const auto U32 = ntk.create_not(U32_h);
    const auto U38_h = ntk.create_and(U52, U51);
    const auto U38 = ntk.create_not(U38_h);
    const auto U39_h1 = ntk.create_and(U50, U49);
    const auto U39_h2 = ntk.create_and(U39_h1, U35);
    const auto U39 = ntk.create_not(U39_h2);
    const auto U42_h = ntk.create_and(U39, STATO_REG_1__SCAN_IN);
    const auto U42 = ntk.create_not(U42_h);
    const auto U33_h = ntk.create_and(U42, U41);
    const auto U33 = ntk.create_not(U33_h);

    const auto bridge_out_neg = ntk.create_not(U_REG_SCAN_IN);
    const auto bridge_out = ntk.create_not(bridge_out_neg);

    ntk.create_po(bridge_out, "U_REG_SCAN_IN");
    ntk.create_po(U33, "U33");
    ntk.create_po(U38, "U38");
    ntk.create_po(U32, "U32");
    ntk.create_po(U31, "U31");

    return ntk;
}
template <typename Ntk>
mockturtle::names_view<Ntk> b02_opt()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto LINEA = ntk.create_pi("LINEA");
    const auto STATO_REG_2_ = ntk.create_ro();
    const auto STATO_REG_1_ = ntk.create_ro();
    const auto STATO_REG_0_ = ntk.create_ro();
    const auto U_REG = ntk.create_ro();
    const auto U72_h = ntk.create_and(LINEA, STATO_REG_1_);
    const auto U72 = ntk.create_not(U72_h);
    const auto U55 = ntk.create_not(STATO_REG_2_);
    const auto U56 = ntk.create_not(STATO_REG_0_);
    const auto U58 = ntk.create_not(LINEA);
    const auto U59_h1 = ntk.create_or(LINEA, STATO_REG_2_);
    const auto U59 = ntk.create_or(U59_h1, STATO_REG_1_);
    const auto U61_h = ntk.create_and(STATO_REG_0_, U59);
    const auto U61 = ntk.create_not(U61_h);
    const auto U62 = ntk.create_or(LINEA, STATO_REG_1_);
    const auto U63_h = ntk.create_and(U55, U62);
    const auto U63 = ntk.create_not(U63_h);
    const auto U65 = ntk.create_or(STATO_REG_0_, STATO_REG_1_);
    const auto U66_h = ntk.create_and(LINEA, U55);
    const auto U66 = ntk.create_not(U66_h);
    const auto U67_h = ntk.create_and(STATO_REG_2_, U58);
    const auto U67 = ntk.create_not(U67_h);
    const auto U68_h = ntk.create_and(U67, U66);
    const auto U68 = ntk.create_not(U68_h);
    const auto U69_h = ntk.create_and(STATO_REG_0_, U63);
    const auto U69 = ntk.create_not(U69_h);
    const auto U70_h1 = ntk.create_and(STATO_REG_1_, U55);
    const auto U70_h2 = ntk.create_and(U70_h1, U56);
    const auto U70 = ntk.create_not(U70_h2);
    const auto U71_h = ntk.create_and(STATO_REG_0_, U58);
    const auto U71 = ntk.create_not(U71_h);
    const auto U52_h1 = ntk.create_or(STATO_REG_1_, STATO_REG_0_);
    const auto U52_h2 = ntk.create_or(U52_h1, U55);
    const auto U52 = ntk.create_not(U52_h2);
    const auto U57_h = ntk.create_and(U70, U69);
    const auto U57 = ntk.create_not(U57_h);
    const auto U60_h = ntk.create_and(STATO_REG_1_, U68);
    const auto U60 = ntk.create_not(U60_h);
    const auto U64_h1 = ntk.create_and(U72, U71);
    const auto U64_h2 = ntk.create_and(U64_h1, U55);
    const auto U64 = ntk.create_not(U64_h2);
    const auto U53_h = ntk.create_and(U65, U64);
    const auto U53 = ntk.create_not(U53_h);
    const auto U54_h = ntk.create_and(U61, U60);
    const auto U54 = ntk.create_not(U54_h);

    const auto bridge_out_neg_1 = ntk.create_not(U_REG);
    const auto bridge_out_1 = ntk.create_not(bridge_out_neg_1);

    ntk.create_po(bridge_out_1, "U_REG");

    ntk.create_ri(U54);
    ntk.create_ri(U57);
    ntk.create_ri(U53);
    ntk.create_ri(U52);

    return ntk;
}
template <typename Ntk>
mockturtle::names_view<Ntk> b02_opt_C()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto U_REG_SCAN_IN = ntk.create_pi("U_REG_SCAN_IN");
    const auto LINEA = ntk.create_pi("LINEA");
    const auto STATO_REG_2__SCAN_IN = ntk.create_pi("STATO_REG_2__SCAN_IN");
    const auto STATO_REG_1__SCAN_IN = ntk.create_pi("STATO_REG_1__SCAN_IN");
    const auto STATO_REG_0__SCAN_IN = ntk.create_pi("STATO_REG_0__SCAN_IN");
    const auto U55 = ntk.create_not(STATO_REG_2__SCAN_IN);
    const auto U56 = ntk.create_not(STATO_REG_0__SCAN_IN);
    const auto U58 = ntk.create_not(LINEA);
    const auto U59_h1 = ntk.create_or(LINEA, STATO_REG_2__SCAN_IN);
    const auto U59 = ntk.create_or(U59_h1, STATO_REG_1__SCAN_IN);
    const auto U61_h = ntk.create_and(U59, STATO_REG_0__SCAN_IN);
    const auto U61 = ntk.create_not(U61_h);
    const auto U62 = ntk.create_or(LINEA, STATO_REG_1__SCAN_IN);
    const auto U63_h = ntk.create_and(U55, U62);
    const auto U63 = ntk.create_not(U63_h);
    const auto U65 = ntk.create_or(STATO_REG_1__SCAN_IN, STATO_REG_0__SCAN_IN);
    const auto U66_h = ntk.create_and(LINEA, U55);
    const auto U66 = ntk.create_not(U66_h);
    const auto U67_h = ntk.create_and(U58, STATO_REG_2__SCAN_IN);
    const auto U67 = ntk.create_not(U67_h);
    const auto U68_h = ntk.create_and(U67, U66);
    const auto U68 = ntk.create_not(U68_h);
    const auto U69_h = ntk.create_and(U63, STATO_REG_0__SCAN_IN);
    const auto U69 = ntk.create_not(U69_h);
    const auto U70_h1 = ntk.create_and(U55, U56);
    const auto U70_h2 = ntk.create_and(U70_h1, STATO_REG_1__SCAN_IN);
    const auto U70 = ntk.create_not(U70_h2);
    const auto U71_h = ntk.create_and(U58, STATO_REG_0__SCAN_IN);
    const auto U71 = ntk.create_not(U71_h);
    const auto U72_h = ntk.create_and(LINEA, STATO_REG_1__SCAN_IN);
    const auto U72 = ntk.create_not(U72_h);
    const auto U52_h1 = ntk.create_or(U55, STATO_REG_1__SCAN_IN);
    const auto U52_h2 = ntk.create_or(U52_h1, STATO_REG_0__SCAN_IN);
    const auto U52 = ntk.create_not(U52_h2);
    const auto U57_h = ntk.create_and(U70, U69);
    const auto U57 = ntk.create_not(U57_h);
    const auto U60_h = ntk.create_and(U68, STATO_REG_1__SCAN_IN);
    const auto U60 = ntk.create_not(U60_h);
    const auto U64_h1 = ntk.create_and(U72, U71);
    const auto U64_h2 = ntk.create_and(U64_h1, U55);
    const auto U64 = ntk.create_not(U64_h2);
    const auto U53_h = ntk.create_and(U65, U64);
    const auto U53 = ntk.create_not(U53_h);
    const auto U54_h = ntk.create_and(U61, U60);
    const auto U54 = ntk.create_not(U54_h);

    const auto bridge_out_neg_1 = ntk.create_not(U_REG_SCAN_IN);
    const auto bridge_out_1 = ntk.create_not(bridge_out_neg_1);

    ntk.create_po(bridge_out_1, "U_REG_SCAN_IN");
    ntk.create_po(U54, "U54");
    ntk.create_po(U57, "U57");
    ntk.create_po(U53, "U53");
    ntk.create_po(U52, "U52");


    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> b02_opt_r()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto RESET_G = ntk.create_pi("RESET_G");
    const auto nRESET_G = ntk.create_pi("nRESET_G");
    const auto LINEA = ntk.create_pi("LINEA");
    const auto STATO_REG_2_ = ntk.create_ro();
    const auto STATO_REG_1_ = ntk.create_ro();
    const auto STATO_REG_0_ = ntk.create_ro();
    const auto U_REG = ntk.create_ro();
    const auto U72_h = ntk.create_and(LINEA, STATO_REG_1_);
    const auto U72 = ntk.create_not(U72_h);
    const auto U55 = ntk.create_not(STATO_REG_2_);
    const auto U56 = ntk.create_not(STATO_REG_0_);
    const auto U58 = ntk.create_not(LINEA);
    const auto U59_h1 = ntk.create_or(LINEA, STATO_REG_2_);
    const auto U59 = ntk.create_or(U59_h1, STATO_REG_1_);
    const auto U61_h = ntk.create_and(STATO_REG_0_, U59);
    const auto U61 = ntk.create_not(U61_h);
    const auto U62 = ntk.create_or(LINEA, STATO_REG_1_);
    const auto U63_h = ntk.create_and(U55, U62);
    const auto U63 = ntk.create_not(U63_h);
    const auto U65 = ntk.create_or(STATO_REG_0_, STATO_REG_1_);
    const auto U66_h = ntk.create_and(LINEA, U55);
    const auto U66 = ntk.create_not(U66_h);
    const auto U67_h = ntk.create_and(STATO_REG_2_, U58);
    const auto U67 = ntk.create_not(U67_h);
    const auto U68_h = ntk.create_and(U67, U66);
    const auto U68 = ntk.create_not(U68_h);
    const auto U69_h = ntk.create_and(STATO_REG_0_, U63);
    const auto U69 = ntk.create_not(U69_h);
    const auto U70_h1 = ntk.create_and(STATO_REG_1_, U55);
    const auto U70_h2 = ntk.create_and(U70_h1, U56);
    const auto U70 = ntk.create_not(U70_h2);
    const auto U71_h = ntk.create_and(STATO_REG_0_, U58);
    const auto U71 = ntk.create_not(U71_h);
    const auto U52_h1 = ntk.create_or(RESET_G, STATO_REG_1_);
    const auto U52_h2 = ntk.create_or(STATO_REG_0_, U55);
    const auto U52_h3 = ntk.create_or(U52_h1, U52_h2);
    const auto U52 = ntk.create_not(U52_h3);
    const auto U57_h1 = ntk.create_and(nRESET_G, U70);
    const auto U57_h2 = ntk.create_and(U57_h1, U69);
    const auto U57 = ntk.create_not(U57_h2);
    const auto U60_h = ntk.create_and(STATO_REG_1_, U68);
    const auto U60 = ntk.create_not(U60_h);
    const auto U64_h1 = ntk.create_and(U72, U71);
    const auto U64_h2 = ntk.create_and(U64_h1, U55);
    const auto U64 = ntk.create_not(U64_h2);
    const auto U53_h1 = ntk.create_and(nRESET_G, U65);
    const auto U53_h2 = ntk.create_and(U53_h1, U64);
    const auto U53 = ntk.create_not(U53_h2);
    const auto U54_h1 = ntk.create_and(nRESET_G, U61);
    const auto U54_h2 = ntk.create_and(U54_h1, U60);
    const auto U54 = ntk.create_not(U54_h2);

    const auto bridge_out_neg_1 = ntk.create_not(U_REG);
    const auto bridge_out_1 = ntk.create_not(bridge_out_neg_1);

    ntk.create_po(bridge_out_1, "U_REG");

    ntk.create_ri(U54);
    ntk.create_ri(U57);
    ntk.create_ri(U53);
    ntk.create_ri(U52);

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> b03()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto REQUEST1 = ntk.create_pi("REQUEST1");
    const auto REQUEST2 = ntk.create_pi("REQUEST2");
    const auto REQUEST3 = ntk.create_pi("REQUEST3");
    const auto REQUEST4 = ntk.create_pi("REQUEST4");
    const auto STATO_REG_0_ = ntk.create_ro();
    const auto CODA0_REG_2_ = ntk.create_ro();
    const auto CODA0_REG_1_ = ntk.create_ro();
    const auto CODA0_REG_0_ = ntk.create_ro();
    const auto CODA1_REG_2_ = ntk.create_ro();
    const auto CODA1_REG_1_ = ntk.create_ro();
    const auto CODA1_REG_0_ = ntk.create_ro();
    const auto CODA2_REG_2_ = ntk.create_ro();
    const auto CODA2_REG_1_ = ntk.create_ro();
    const auto CODA2_REG_0_ = ntk.create_ro();
    const auto CODA3_REG_2_ = ntk.create_ro();
    const auto CODA3_REG_1_ = ntk.create_ro();
    const auto CODA3_REG_0_ = ntk.create_ro();
    const auto GRANT_REG_3_ = ntk.create_ro();
    const auto GRANT_REG_2_ = ntk.create_ro();
    const auto GRANT_REG_1_ = ntk.create_ro();
    const auto GRANT_REG_0_ = ntk.create_ro();
    const auto GRANT_O_REG_3_ = ntk.create_ro();
    const auto GRANT_O_REG_2_ = ntk.create_ro();
    const auto GRANT_O_REG_1_ = ntk.create_ro();
    const auto GRANT_O_REG_0_ = ntk.create_ro();
    const auto RU3_REG = ntk.create_ro();
    const auto FU1_REG = ntk.create_ro();
    const auto FU3_REG = ntk.create_ro();
    const auto RU1_REG = ntk.create_ro();
    const auto RU4_REG = ntk.create_ro();
    const auto FU2_REG = ntk.create_ro();
    const auto FU4_REG = ntk.create_ro();
    const auto RU2_REG = ntk.create_ro();
    const auto STATO_REG_1_ = ntk.create_ro();
    const auto U203 = ntk.create_not(STATO_REG_0_);
    const auto U218 = ntk.create_not(STATO_REG_1_);
    const auto U219 = ntk.create_not(RU3_REG);
    const auto U221 = ntk.create_not(RU1_REG);
    const auto U222_h = ntk.create_and(RU1_REG, STATO_REG_0_);
    const auto U222 = ntk.create_not(U222_h);
    const auto U224 = ntk.create_not(CODA0_REG_2_);
    const auto U225 = ntk.create_not(CODA0_REG_1_);
    const auto U226 = ntk.create_not(CODA0_REG_0_);
    const auto U227 = ntk.create_not(FU1_REG);
    const auto U228 = ntk.create_not(FU2_REG);
    const auto U243 = ntk.create_not(RU2_REG);
    const auto U245 = ntk.create_not(FU3_REG);
    const auto U246 = ntk.create_not(FU4_REG);
    const auto U247 = ntk.create_not(U222);
    const auto U248_h1 = ntk.create_or(FU3_REG, FU2_REG);
    const auto U248_h2 = ntk.create_or(FU1_REG, FU4_REG);
    const auto U248 = ntk.create_or(U248_h1, U248_h2);
    const auto U254_h = ntk.create_and(U247, U227);
    const auto U254 = ntk.create_not(U254_h);
    const auto U256 = ntk.create_or(RU2_REG, RU3_REG);
    const auto U257_h = ntk.create_and(U221, U256);
    const auto U257 = ntk.create_not(U257_h);
    const auto U291_h = ntk.create_and(FU1_REG, U203);
    const auto U291 = ntk.create_not(U291_h);
    const auto U292_h = ntk.create_and(REQUEST1, U203);
    const auto U292 = ntk.create_not(U292_h);
    const auto U293_h = ntk.create_and(RU2_REG, U228);
    const auto U293 = ntk.create_not(U293_h);
    const auto U294_h1 = ntk.create_and(U219, U246);
    const auto U294_h2 = ntk.create_and(RU4_REG, U243);
    const auto U294_h3 = ntk.create_and(U294_h1, U294_h2);
    const auto U294 = ntk.create_not(U294_h3);
    const auto U303_h = ntk.create_and(GRANT_O_REG_3_, U203);
    const auto U303 = ntk.create_not(U303_h);
    const auto U304_h = ntk.create_and(GRANT_REG_3_, STATO_REG_0_);
    const auto U304 = ntk.create_not(U304_h);
    const auto U305_h = ntk.create_and(GRANT_O_REG_2_, U203);
    const auto U305 = ntk.create_not(U305_h);
    const auto U306_h = ntk.create_and(GRANT_REG_2_, STATO_REG_0_);
    const auto U306 = ntk.create_not(U306_h);
    const auto U307_h = ntk.create_and(GRANT_O_REG_1_, U203);
    const auto U307 = ntk.create_not(U307_h);
    const auto U308_h = ntk.create_and(GRANT_REG_1_, STATO_REG_0_);
    const auto U308 = ntk.create_not(U308_h);
    const auto U309_h = ntk.create_and(GRANT_O_REG_0_, U203);
    const auto U309 = ntk.create_not(U309_h);
    const auto U310_h = ntk.create_and(GRANT_REG_0_, STATO_REG_0_);
    const auto U310 = ntk.create_not(U310_h);
    const auto U311_h = ntk.create_and(REQUEST3, U203);
    const auto U311 = ntk.create_not(U311_h);
    const auto U312_h = ntk.create_and(RU3_REG, STATO_REG_0_);
    const auto U312 = ntk.create_not(U312_h);
    const auto U313_h = ntk.create_and(FU3_REG, U203);
    const auto U313 = ntk.create_not(U313_h);
    const auto U314_h = ntk.create_and(RU3_REG, STATO_REG_0_);
    const auto U314 = ntk.create_not(U314_h);
    const auto U315_h = ntk.create_and(REQUEST4, U203);
    const auto U315 = ntk.create_not(U315_h);
    const auto U316_h = ntk.create_and(RU4_REG, STATO_REG_0_);
    const auto U316 = ntk.create_not(U316_h);
    const auto U317_h = ntk.create_and(FU2_REG, U203);
    const auto U317 = ntk.create_not(U317_h);
    const auto U318_h = ntk.create_and(RU2_REG, STATO_REG_0_);
    const auto U318 = ntk.create_not(U318_h);
    const auto U319_h = ntk.create_and(FU4_REG, U203);
    const auto U319 = ntk.create_not(U319_h);
    const auto U320_h = ntk.create_and(RU4_REG, STATO_REG_0_);
    const auto U320 = ntk.create_not(U320_h);
    const auto U321_h = ntk.create_and(REQUEST2, U203);
    const auto U321 = ntk.create_not(U321_h);
    const auto U322_h = ntk.create_and(RU2_REG, STATO_REG_0_);
    const auto U322 = ntk.create_not(U322_h);
    const auto U204_h = ntk.create_and(U222, U292);
    const auto U204 = ntk.create_not(U204_h);
    const auto U205_h = ntk.create_and(U222, U291);
    const auto U205 = ntk.create_not(U205_h);
    const auto U220_h = ntk.create_and(RU3_REG, U243);
    const auto U220 = ntk.create_not(U220_h);
    const auto U233_h = ntk.create_and(U304, U303);
    const auto U233 = ntk.create_not(U233_h);
    const auto U234_h = ntk.create_and(U306, U305);
    const auto U234 = ntk.create_not(U234_h);
    const auto U235_h = ntk.create_and(U308, U307);
    const auto U235 = ntk.create_not(U235_h);
    const auto U236_h = ntk.create_and(U310, U309);
    const auto U236 = ntk.create_not(U236_h);
    const auto U237_h = ntk.create_and(U312, U311);
    const auto U237 = ntk.create_not(U237_h);
    const auto U238_h = ntk.create_and(U314, U313);
    const auto U238 = ntk.create_not(U238_h);
    const auto U239_h = ntk.create_and(U316, U315);
    const auto U239 = ntk.create_not(U239_h);
    const auto U240_h = ntk.create_and(U318, U317);
    const auto U240 = ntk.create_not(U240_h);
    const auto U241_h = ntk.create_and(U320, U319);
    const auto U241 = ntk.create_not(U241_h);
    const auto U242_h = ntk.create_and(U322, U321);
    const auto U242 = ntk.create_not(U242_h);
    const auto U244_h = ntk.create_and(STATO_REG_1_, U248);
    const auto U244 = ntk.create_not(U244_h);
    const auto U249 = ntk.create_not(U244);
    const auto U250 = ntk.create_not(U220);
    const auto U251_h = ntk.create_and(U250, U245);
    const auto U251 = ntk.create_not(U251_h);
    const auto U252_h1 = ntk.create_and(U294, U293);
    const auto U252_h2 = ntk.create_and(U252_h1, U251);
    const auto U252 = ntk.create_not(U252_h2);
    const auto U253_h1 = ntk.create_and(U221, STATO_REG_0_);
    const auto U253_h2 = ntk.create_and(U253_h1, U252);
    const auto U253 = ntk.create_not(U253_h2);
    const auto U295_h = ntk.create_and(GRANT_REG_3_, U244);
    const auto U295 = ntk.create_not(U295_h);
    const auto U296_h1 = ntk.create_and(U226, U225);
    const auto U296_h2 = ntk.create_and(CODA0_REG_2_, U249);
    const auto U296_h3 = ntk.create_and(U296_h1, U296_h2);
    const auto U296 = ntk.create_not(U296_h3);
    const auto U297_h = ntk.create_and(GRANT_REG_2_, U244);
    const auto U297 = ntk.create_not(U297_h);
    const auto U298_h1 = ntk.create_and(U226, U224);
    const auto U298_h2 = ntk.create_and(CODA0_REG_1_, U249);
    const auto U298_h3 = ntk.create_and(U298_h1, U298_h2);
    const auto U298 = ntk.create_not(U298_h3);
    const auto U299_h = ntk.create_and(GRANT_REG_1_, U244);
    const auto U299 = ntk.create_not(U299_h);
    const auto U300_h1 = ntk.create_and(U225, U224);
    const auto U300_h2 = ntk.create_and(CODA0_REG_0_, U249);
    const auto U300_h3 = ntk.create_and(U300_h1, U300_h2);
    const auto U300 = ntk.create_not(U300_h3);
    const auto U301_h = ntk.create_and(GRANT_REG_0_, U244);
    const auto U301 = ntk.create_not(U301_h);
    const auto U302_h1 = ntk.create_and(CODA0_REG_1_, CODA0_REG_2_);
    const auto U302_h2 = ntk.create_and(CODA0_REG_0_, U249);
    const auto U302_h3 = ntk.create_and(U302_h1, U302_h2);
    const auto U302 = ntk.create_not(U302_h3);
    const auto U223_h1 = ntk.create_and(U254, U244);
    const auto U223_h2 = ntk.create_and(U223_h1, U253);
    const auto U223 = ntk.create_not(U223_h2);
    const auto U229_h = ntk.create_and(U296, U295);
    const auto U229 = ntk.create_not(U229_h);
    const auto U230_h = ntk.create_and(U298, U297);
    const auto U230 = ntk.create_not(U230_h);
    const auto U231_h = ntk.create_and(U300, U299);
    const auto U231 = ntk.create_not(U231_h);
    const auto U232_h = ntk.create_and(U302, U301);
    const auto U232 = ntk.create_not(U232_h);
    const auto U255 = ntk.create_not(U223);
    const auto U260_h = ntk.create_and(CODA0_REG_2_, U255);
    const auto U260 = ntk.create_not(U260_h);
    const auto U263_h = ntk.create_and(CODA0_REG_1_, U255);
    const auto U263 = ntk.create_not(U263_h);
    const auto U266_h = ntk.create_and(CODA0_REG_0_, U255);
    const auto U266 = ntk.create_not(U266_h);
    const auto U269_h = ntk.create_and(U255, CODA1_REG_2_);
    const auto U269 = ntk.create_not(U269_h);
    const auto U272_h = ntk.create_and(CODA1_REG_1_, U255);
    const auto U272 = ntk.create_not(U272_h);
    const auto U275_h = ntk.create_and(CODA1_REG_0_, U255);
    const auto U275 = ntk.create_not(U275_h);
    const auto U278_h = ntk.create_and(CODA2_REG_2_, U255);
    const auto U278 = ntk.create_not(U278_h);
    const auto U281_h = ntk.create_and(CODA2_REG_1_, U255);
    const auto U281 = ntk.create_not(U281_h);
    const auto U284_h = ntk.create_and(CODA2_REG_0_, U255);
    const auto U284 = ntk.create_not(U284_h);
    const auto U286_h = ntk.create_and(CODA3_REG_2_, U255);
    const auto U286 = ntk.create_not(U286_h);
    const auto U288_h = ntk.create_and(CODA3_REG_1_, U255);
    const auto U288 = ntk.create_not(U288_h);
    const auto U290_h = ntk.create_and(CODA3_REG_0_, U255);
    const auto U290 = ntk.create_not(U290_h);
    const auto U201 = ntk.create_and(U223, U218);
    const auto U202 = ntk.create_and(STATO_REG_1_, U223);
    const auto U258_h = ntk.create_and(U201, U257);
    const auto U258 = ntk.create_not(U258_h);
    const auto U259_h = ntk.create_and(CODA1_REG_2_, U202);
    const auto U259 = ntk.create_not(U259_h);
    const auto U261_h1 = ntk.create_and(U220, U221);
    const auto U261_h2 = ntk.create_and(U261_h1, U201);
    const auto U261 = ntk.create_not(U261_h2);
    const auto U262_h = ntk.create_and(CODA1_REG_1_, U202);
    const auto U262 = ntk.create_not(U262_h);
    const auto U264_h1 = ntk.create_and(U221, U243);
    const auto U264_h2 = ntk.create_and(U264_h1, U201);
    const auto U264 = ntk.create_not(U264_h2);
    const auto U265_h = ntk.create_and(CODA1_REG_0_, U202);
    const auto U265 = ntk.create_not(U265_h);
    const auto U267_h = ntk.create_and(CODA2_REG_2_, U202);
    const auto U267 = ntk.create_not(U267_h);
    const auto U268_h = ntk.create_and(CODA0_REG_2_, U201);
    const auto U268 = ntk.create_not(U268_h);
    const auto U270_h = ntk.create_and(CODA2_REG_1_, U202);
    const auto U270 = ntk.create_not(U270_h);
    const auto U271_h = ntk.create_and(CODA0_REG_1_, U201);
    const auto U271 = ntk.create_not(U271_h);
    const auto U273_h = ntk.create_and(CODA2_REG_0_, U202);
    const auto U273 = ntk.create_not(U273_h);
    const auto U274_h = ntk.create_and(CODA0_REG_0_, U201);
    const auto U274 = ntk.create_not(U274_h);
    const auto U276_h = ntk.create_and(CODA3_REG_2_, U202);
    const auto U276 = ntk.create_not(U276_h);
    const auto U277_h = ntk.create_and(CODA1_REG_2_, U201);
    const auto U277 = ntk.create_not(U277_h);
    const auto U279_h = ntk.create_and(CODA3_REG_1_, U202);
    const auto U279 = ntk.create_not(U279_h);
    const auto U280_h = ntk.create_and(CODA1_REG_1_, U201);
    const auto U280 = ntk.create_not(U280_h);
    const auto U282_h = ntk.create_and(CODA3_REG_0_, U202);
    const auto U282 = ntk.create_not(U282_h);
    const auto U283_h = ntk.create_and(CODA1_REG_0_, U201);
    const auto U283 = ntk.create_not(U283_h);
    const auto U285_h = ntk.create_and(CODA2_REG_2_, U201);
    const auto U285 = ntk.create_not(U285_h);
    const auto U287_h = ntk.create_and(CODA2_REG_1_, U201);
    const auto U287 = ntk.create_not(U287_h);
    const auto U289_h = ntk.create_and(CODA2_REG_0_, U201);
    const auto U289 = ntk.create_not(U289_h);
    const auto U206_h = ntk.create_and(U290, U289);
    const auto U206 = ntk.create_not(U206_h);
    const auto U207_h = ntk.create_and(U288, U287);
    const auto U207 = ntk.create_not(U207_h);
    const auto U208_h = ntk.create_and(U286, U285);
    const auto U208 = ntk.create_not(U208_h);
    const auto U209_h1 = ntk.create_and(U283, U282);
    const auto U209_h2 = ntk.create_and(U209_h1, U284);
    const auto U209 = ntk.create_not(U209_h2);
    const auto U210_h1 = ntk.create_and(U280, U279);
    const auto U210_h2 = ntk.create_and(U210_h1, U281);
    const auto U210 = ntk.create_not(U210_h2);
    const auto U211_h1 = ntk.create_and(U277, U276);
    const auto U211_h2 = ntk.create_and(U211_h1, U278);
    const auto U211 = ntk.create_not(U211_h2);
    const auto U212_h1 = ntk.create_and(U274, U273);
    const auto U212_h2 = ntk.create_and(U212_h1, U275);
    const auto U212 = ntk.create_not(U212_h2);
    const auto U213_h1 = ntk.create_and(U271, U270);
    const auto U213_h2 = ntk.create_and(U213_h1, U272);
    const auto U213 = ntk.create_not(U213_h2);
    const auto U214_h1 = ntk.create_and(U268, U267);
    const auto U214_h2 = ntk.create_and(U214_h1, U269);
    const auto U214 = ntk.create_not(U214_h2);
    const auto U215_h1 = ntk.create_and(U265, U264);
    const auto U215_h2 = ntk.create_and(U215_h1, U266);
    const auto U215 = ntk.create_not(U215_h2);
    const auto U216_h1 = ntk.create_and(U262, U261);
    const auto U216_h2 = ntk.create_and(U216_h1, U263);
    const auto U216 = ntk.create_not(U216_h2);
    const auto U217_h1 = ntk.create_and(U259, U258);
    const auto U217_h2 = ntk.create_and(U217_h1, U260);
    const auto U217 = ntk.create_not(U217_h2);

    const auto bridge_out_neg_1 = ntk.create_not(GRANT_O_REG_3_);
    const auto bridge_out_1 = ntk.create_not(bridge_out_neg_1);
    const auto bridge_out_neg_2 = ntk.create_not(GRANT_O_REG_2_);
    const auto bridge_out_2 = ntk.create_not(bridge_out_neg_2);
    const auto bridge_out_neg_3 = ntk.create_not(GRANT_O_REG_1_);
    const auto bridge_out_3 = ntk.create_not(bridge_out_neg_3);
    const auto bridge_out_neg_4 = ntk.create_not(GRANT_O_REG_0_);
    const auto bridge_out_4 = ntk.create_not(bridge_out_neg_4);

    ntk.create_po(bridge_out_1, "GRANT_O_REG_3_");
    ntk.create_po(bridge_out_2, "GRANT_O_REG_2_");
    ntk.create_po(bridge_out_3, "GRANT_O_REG_1_");
    ntk.create_po(bridge_out_4, "GRANT_O_REG_0_");

    ntk.create_ri(U203);
    ntk.create_ri(U217);
    ntk.create_ri(U216);
    ntk.create_ri(U215);
    ntk.create_ri(U214);
    ntk.create_ri(U213);
    ntk.create_ri(U212);
    ntk.create_ri(U211);
    ntk.create_ri(U210);
    ntk.create_ri(U209);
    ntk.create_ri(U208);
    ntk.create_ri(U207);
    ntk.create_ri(U206);
    ntk.create_ri(U229);
    ntk.create_ri(U230);
    ntk.create_ri(U231);
    ntk.create_ri(U232);
    ntk.create_ri(U233);
    ntk.create_ri(U234);
    ntk.create_ri(U235);
    ntk.create_ri(U236);
    ntk.create_ri(U237);
    ntk.create_ri(U205);
    ntk.create_ri(U238);
    ntk.create_ri(U204);
    ntk.create_ri(U239);
    ntk.create_ri(U240);
    ntk.create_ri(U241);
    ntk.create_ri(U242);
    ntk.create_ri(STATO_REG_0_);

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> b06()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto EQL = ntk.create_pi("EQL");
    const auto CONT_EQL = ntk.create_pi("CONT_EQL");
    const auto ACKOUT_REG = ntk.create_ro();
    const auto STATE_REG_2_ = ntk.create_ro();
    const auto STATE_REG_1_ = ntk.create_ro();
    const auto STATE_REG_0_ = ntk.create_ro();
    const auto CC_MUX_REG_2_ = ntk.create_ro();
    const auto CC_MUX_REG_1_ = ntk.create_ro();
    const auto USCITE_REG_2_ = ntk.create_ro();
    const auto USCITE_REG_1_ = ntk.create_ro();
    const auto ENABLE_COUNT_REG = ntk.create_ro();
    const auto U63_h1 = ntk.create_and(STATE_REG_2_, STATE_REG_1_);
    const auto U63 = ntk.create_and(U63_h1, STATE_REG_0_);
    const auto U64 = ntk.create_not(STATE_REG_1_);
    const auto U65 = ntk.create_not(EQL);
    const auto U66_h = ntk.create_and(EQL, STATE_REG_1_);
    const auto U66 = ntk.create_not(U66_h);
    const auto U67 = ntk.create_not(STATE_REG_2_);
    const auto U68 = ntk.create_not(STATE_REG_0_);
    const auto U69 = ntk.create_or(STATE_REG_2_, STATE_REG_0_);
    const auto U70_h1 = ntk.create_and(U64, U67);
    const auto U70_h2 = ntk.create_and(U70_h1, STATE_REG_0_);
    const auto U70 = ntk.create_not(U70_h2);
    const auto U71 = ntk.create_not(U66);
    const auto U72_h1 = ntk.create_and(STATE_REG_2_, U68);
    const auto U72_h2 = ntk.create_and(U64, U65);
    const auto U72_h3 = ntk.create_and(U72_h1, U72_h2);
    const auto U72 = ntk.create_not(U72_h3);
    const auto U73 = ntk.create_not(U69);
    const auto U74 = ntk.create_not(U70);
    const auto U75_h = ntk.create_and(STATE_REG_2_, U71);
    const auto U75 = ntk.create_not(U75_h);
    const auto U76_h = ntk.create_and(STATE_REG_2_, STATE_REG_1_);
    const auto U76 = ntk.create_not(U76_h);
    const auto U77 = ntk.create_or(STATE_REG_1_, STATE_REG_0_);
    const auto U78_h = ntk.create_and(U65, U77);
    const auto U78 = ntk.create_not(U78_h);
    const auto U79_h = ntk.create_and(U74, U65);
    const auto U79 = ntk.create_not(U79_h);
    const auto U80_h = ntk.create_and(STATE_REG_2_, U78);
    const auto U80 = ntk.create_not(U80_h);
    const auto U81_h1 = ntk.create_and(EQL, U67);
    const auto U81_h2 = ntk.create_and(U81_h1, STATE_REG_0_);
    const auto U81 = ntk.create_not(U81_h2);
    const auto U82_h = ntk.create_and(U73, STATE_REG_1_);
    const auto U82 = ntk.create_not(U82_h);
    const auto U83_h1 = ntk.create_and(STATE_REG_1_, U65);
    const auto U83_h2 = ntk.create_and(U83_h1, U73);
    const auto U83 = ntk.create_not(U83_h2);
    const auto U84 = ntk.create_or(CONT_EQL, U63);
    const auto U85_h = ntk.create_and(U71, U68);
    const auto U85 = ntk.create_not(U85_h);
    const auto U86_h = ntk.create_and(STATE_REG_2_, U78);
    const auto U86 = ntk.create_not(U86_h);
    const auto U87_h = ntk.create_and(EQL, U68);
    const auto U87 = ntk.create_not(U87_h);
    const auto U88_h = ntk.create_and(STATE_REG_0_, STATE_REG_2_);
    const auto U88 = ntk.create_not(U88_h);
    const auto U89_h = ntk.create_and(U73, U64);
    const auto U89 = ntk.create_not(U89_h);
    const auto U90_h = ntk.create_and(STATE_REG_0_, STATE_REG_1_);
    const auto U90 = ntk.create_not(U90_h);
    const auto U91_h = ntk.create_and(EQL, U64);
    const auto U91 = ntk.create_not(U91_h);
    const auto U92_h = ntk.create_and(STATE_REG_1_, U87);
    const auto U92 = ntk.create_not(U92_h);
    const auto U54 = ntk.create_and(U90, U89);
    const auto U55_h = ntk.create_and(U54, U78);
    const auto U55 = ntk.create_not(U55_h);
    const auto U56_h1 = ntk.create_and(U82, U81);
    const auto U56_h2 = ntk.create_and(U72, U66);
    const auto U56_h3 = ntk.create_and(U56_h1, U56_h2);
    const auto U56 = ntk.create_not(U56_h3);
    const auto U57_h = ntk.create_and(U80, U79);
    const auto U57 = ntk.create_not(U57_h);
    const auto U58_h1 = ntk.create_and(U88, U69);
    const auto U58_h2 = ntk.create_and(U92, U91);
    const auto U58_h3 = ntk.create_and(U58_h1, U58_h2);
    const auto U58 = ntk.create_not(U58_h3);
    const auto U59_h1 = ntk.create_and(U86, U70);
    const auto U59_h2 = ntk.create_and(U59_h1, U85);
    const auto U59 = ntk.create_not(U59_h2);
    const auto U60_h1 = ntk.create_and(EQL, U76);
    const auto U60_h2 = ntk.create_and(U60_h1, U54);
    const auto U60 = ntk.create_not(U60_h2);
    const auto U61_h = ntk.create_and(U72, U75);
    const auto U61 = ntk.create_not(U61_h);
    const auto U62_h = ntk.create_and(U84, U83);
    const auto U62 = ntk.create_not(U62_h);

    const auto bridge_out_neg_1 = ntk.create_not(CC_MUX_REG_2_);
    const auto bridge_out_1 = ntk.create_not(bridge_out_neg_1);
    const auto bridge_out_neg_2 = ntk.create_not(CC_MUX_REG_1_);
    const auto bridge_out_2 = ntk.create_not(bridge_out_neg_2);
    const auto bridge_out_neg_3 = ntk.create_not(USCITE_REG_2_);
    const auto bridge_out_3 = ntk.create_not(bridge_out_neg_3);
    const auto bridge_out_neg_4 = ntk.create_not(USCITE_REG_1_);
    const auto bridge_out_4 = ntk.create_not(bridge_out_neg_4);
    const auto bridge_out_neg_5 = ntk.create_not(ENABLE_COUNT_REG);
    const auto bridge_out_5 = ntk.create_not(bridge_out_neg_5);
    const auto bridge_out_neg_6 = ntk.create_not(ACKOUT_REG);
    const auto bridge_out_6 = ntk.create_not(bridge_out_neg_6);

    ntk.create_po(bridge_out_1, "CC_MUX_REG_2_");
    ntk.create_po(bridge_out_2, "CC_MUX_REG_1_");
    ntk.create_po(bridge_out_3, "USCITE_REG_2_");
    ntk.create_po(bridge_out_4, "USCITE_REG_1_");
    ntk.create_po(bridge_out_5, "ENABLE_COUNT_REG");
    ntk.create_po(bridge_out_6, "ACKOUT_REG");

    ntk.create_ri(U62);
    ntk.create_ri(U57);
    ntk.create_ri(U56);
    ntk.create_ri(U55);
    ntk.create_ri(U59);
    ntk.create_ri(U58);
    ntk.create_ri(U61);
    ntk.create_ri(U60);
    ntk.create_ri(U62);

    return ntk;
}

}  // namespace blueprints

#endif  // FICTION_NETWORK_BLUEPRINTS_HPP
