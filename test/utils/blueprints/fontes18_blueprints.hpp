//
// Created by Hien Benjamin on 04.01.2023.
//

#ifndef FICTION_FONTES18_BLUEPRINTS_HPP
#define FICTION_FONTES18_BLUEPRINTS_HPP

#include <mockturtle/views/names_view.hpp>

namespace blueprints
{

template <typename Ntk>
mockturtle::names_view<Ntk> f18_majority()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto pi0 = ntk.create_pi("pi0");
    const auto pi1 = ntk.create_pi("pi1");
    const auto pi2 = ntk.create_pi("pi2");
    const auto pi3 = ntk.create_pi("pi3");
    const auto pi4 = ntk.create_pi("pi4");
    const auto new_n7 = ntk.create_and(pi2, pi4);
    const auto new_n8 = ntk.create_and(pi1, new_n7);
    const auto npi0 = ntk.create_not(pi0);
    const auto nnew_n8 = ntk.create_not(new_n8);
    const auto new_n9 = ntk.create_and(npi0, nnew_n8);
    const auto npi2 = ntk.create_not(pi2);
    const auto npi4 = ntk.create_not(pi4);
    const auto new_n10 = ntk.create_and(npi2, npi4);
    const auto nnew_n10 = ntk.create_not(new_n10);
    const auto new_n11 = ntk.create_and(pi1, nnew_n10);
    const auto nnew_n7 = ntk.create_not(new_n7);
    const auto nnew_n11 = ntk.create_not(new_n11);
    const auto new_n12 = ntk.create_and(nnew_n7, nnew_n11);
    const auto nnew_n9 = ntk.create_not(new_n9);
    const auto nnew_n12 = ntk.create_not(new_n12);
    const auto new_n13 = ntk.create_and(nnew_n9, nnew_n12);
    const auto po0 = ntk.create_or(pi3, new_n13);

    ntk.create_po(po0, "po0");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> f18_b1_r2()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto pi0 = ntk.create_pi("pi0");
    const auto pi1 = ntk.create_pi("pi1");
    const auto pi2 = ntk.create_pi("pi2");
    const auto new_n8 = ntk.create_and(pi0, pi1);
    const auto npi0 = ntk.create_not(pi0);
    const auto npi1 = ntk.create_not(pi1);
    const auto new_n9 = ntk.create_and(npi0, npi1);
    const auto nnew_n8 = ntk.create_not(new_n8);
    const auto nnew_n9 = ntk.create_not(new_n9);
    const auto po1 = ntk.create_and(nnew_n8, nnew_n9);
    const auto npi2 = ntk.create_not(pi2);
    const auto new_n11 = ntk.create_and(npi2, new_n8);
    const auto new_n12 = ntk.create_and(pi2, new_n9);
    const auto po2 = ntk.create_or(new_n11, new_n12);
    const auto po3 = ntk.create_not(pi2);

    ntk.create_po(pi2, "po0");
    ntk.create_po(po1, "po1");
    ntk.create_po(po2, "po2");
    ntk.create_po(po3, "po3");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> f18_onebitAdderAOIG()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto x = ntk.create_pi("x");
    const auto y = ntk.create_pi("y");
    const auto z = ntk.create_pi("z");
    const auto ny = ntk.create_not(y);
    const auto nz = ntk.create_not(z);
    const auto new_n6 = ntk.create_and(ny, nz);
    const auto new_n7 = ntk.create_and(y, z);
    const auto nnew_n6 = ntk.create_not(new_n6);
    const auto nnew_n7 = ntk.create_not(new_n7);
    const auto new_n8 = ntk.create_and(nnew_n6, nnew_n7);
    const auto nx = ntk.create_not(x);
    const auto nnew_n8 = ntk.create_not(new_n8);
    const auto new_n9 = ntk.create_and(nx, nnew_n8);
    const auto new_n10 = ntk.create_and(x, new_n8);
    const auto nnew_n9 = ntk.create_not(new_n9);
    const auto nnew_n10 = ntk.create_not(new_n10);
    const auto h = ntk.create_and(nnew_n9, nnew_n10);
    const auto i = ntk.create_or(new_n7, new_n10);

    ntk.create_po(h, "h");
    ntk.create_po(i, "i");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> f18_onebitAdderMaj()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");
    const auto cin = ntk.create_pi("cin");
    const auto new_n5 = ntk.create_and(a, b);
    const auto new_n6 = ntk.create_and(a, cin);
    const auto nnew_n5 = ntk.create_not(new_n5);
    const auto nnew_n6 = ntk.create_not(new_n6);
    const auto new_n7 = ntk.create_and(nnew_n5, nnew_n6);
    const auto new_n8 = ntk.create_and(b, cin);
    const auto nnew_n8 = ntk.create_not(new_n8);
    const auto new_n9 = ntk.create_and(new_n7, nnew_n8);
    const auto new_n10 = ntk.create_and(a, new_n9);
    const auto new_n11 = ntk.create_and(b, new_n9);
    const auto nnew_n10 = ntk.create_not(new_n10);
    const auto nnew_n11 = ntk.create_not(new_n11);
    const auto new_n12 = ntk.create_and(nnew_n10, nnew_n11);
    const auto new_n13 = ntk.create_and(nnew_n5, new_n12);
    const auto nnew_n9 = ntk.create_not(new_n9);
    const auto nnew_n13 = ntk.create_not(new_n13);
    const auto new_n14 = ntk.create_and(nnew_n9, nnew_n13);
    const auto ncin = ntk.create_not(cin);
    const auto new_n15 = ntk.create_and(ncin, nnew_n13);
    const auto new_n16 = ntk.create_and(ncin, nnew_n9);
    const auto nnew_n15 = ntk.create_not(new_n15);
    const auto nnew_n16 = ntk.create_not(new_n16);
    const auto new_n17 = ntk.create_and(nnew_n15, nnew_n16);
    const auto nnew_n17 = ntk.create_not(new_n17);
    const auto m3 = ntk.create_or(new_n14, new_n17);

    ntk.create_po(m3, "m3");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> f18_twobitAdderMaj()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");
    const auto c = ntk.create_pi("c");
    const auto d = ntk.create_pi("d");
    const auto e = ntk.create_pi("e");
    const auto new_n8 = ntk.create_and(a, b);
    const auto new_n9 = ntk.create_and(a, c);
    const auto nnew_n8 = ntk.create_not(new_n8);
    const auto nnew_n9 = ntk.create_not(new_n9);
    const auto new_n10 = ntk.create_and(nnew_n8, nnew_n9);
    const auto new_n11 = ntk.create_and(b, c);
    const auto nnew_n11 = ntk.create_not(new_n11);
    const auto new_n12 = ntk.create_and(new_n10, nnew_n11);
    const auto new_n13 = ntk.create_and(nnew_n9, nnew_n11);
    const auto new_n14 = ntk.create_and(nnew_n8, new_n13);
    const auto nnew_n12 = ntk.create_not(new_n12);
    const auto nnew_n14 = ntk.create_not(new_n14);
    const auto new_n15 = ntk.create_and(nnew_n12, nnew_n14);
    const auto new_n16 = ntk.create_and(c, nnew_n12);
    const auto nnew_n15 = ntk.create_not(new_n15);
    const auto nnew_n16 = ntk.create_not(new_n16);
    const auto new_n17 = ntk.create_and(nnew_n15, nnew_n16);
    const auto new_n18 = ntk.create_and(c, nnew_n14);
    const auto nnew_n17 = ntk.create_not(new_n17);
    const auto m3 = ntk.create_or(new_n17, new_n18);
    const auto new_n20 = ntk.create_and(d, e);
    const auto new_n21 = ntk.create_and(d, nnew_n12);
    const auto nnew_n20 = ntk.create_not(new_n20);
    const auto nnew_n21 = ntk.create_not(new_n21);
    const auto new_n22 = ntk.create_and(nnew_n20, nnew_n21);
    const auto new_n23 = ntk.create_and(e, nnew_n12);
    const auto nnew_n23 = ntk.create_not(new_n23);
    const auto new_n24 = ntk.create_and(new_n22, nnew_n23);
    const auto new_n25 = ntk.create_and(nnew_n20, nnew_n23);
    const auto new_n26 = ntk.create_and(nnew_n21, new_n25);
    const auto nnew_n24 = ntk.create_not(new_n24);
    const auto nnew_n26 = ntk.create_not(new_n26);
    const auto new_n27 = ntk.create_and(nnew_n24, nnew_n26);
    const auto new_n28 = ntk.create_and(nnew_n12, nnew_n24);
    const auto new_n29 = ntk.create_and(nnew_n12, nnew_n26);
    const auto nnew_n28 = ntk.create_not(new_n28);
    const auto nnew_n29 = ntk.create_not(new_n29);
    const auto new_n30 = ntk.create_and(nnew_n28, nnew_n29);
    const auto nnew_n30 = ntk.create_not(new_n30);
    const auto m6 = ntk.create_or(new_n27, new_n30);

    ntk.create_po(m3, "m3");
    ntk.create_po(m6, "m6");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> f18_cm82a_5()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto pi0 = ntk.create_pi("pi0");
    const auto pi1 = ntk.create_pi("pi1");
    const auto pi2 = ntk.create_pi("pi2");
    const auto pi3 = ntk.create_pi("pi3");
    const auto pi4 = ntk.create_pi("pi4");
    const auto npi2 = ntk.create_not(pi2);
    const auto new_n9 = ntk.create_and(pi1, npi2);
    const auto npi1 = ntk.create_not(pi1);
    const auto new_n10 = ntk.create_and(npi1, pi2);
    const auto nnew_n9 = ntk.create_not(new_n9);
    const auto nnew_n10 = ntk.create_not(new_n10);
    const auto new_n11 = ntk.create_and(nnew_n9, nnew_n10);
    const auto new_n12 = ntk.create_and(pi0, new_n11);
    const auto npi0 = ntk.create_not(pi0);
    const auto nnew_n11 = ntk.create_not(new_n11);
    const auto new_n13 = ntk.create_and(npi0, nnew_n11);
    const auto po0 = ntk.create_or(new_n12, new_n13);
    const auto npi4 = ntk.create_not(pi4);
    const auto new_n15 = ntk.create_and(pi3, npi4);
    const auto npi3 = ntk.create_not(pi3);
    const auto new_n16 = ntk.create_and(npi3, pi4);
    const auto nnew_n15 = ntk.create_not(new_n15);
    const auto nnew_n16 = ntk.create_not(new_n16);
    const auto new_n17 = ntk.create_and(nnew_n15, nnew_n16);
    const auto new_n18 = ntk.create_and(npi1, npi2);
    const auto nnew_n18 = ntk.create_not(new_n18);
    const auto new_n19 = ntk.create_and(pi0, nnew_n18);
    const auto new_n20 = ntk.create_and(pi1, pi2);
    const auto nnew_n19 = ntk.create_not(new_n19);
    const auto nnew_n20 = ntk.create_not(new_n20);
    const auto new_n21 = ntk.create_and(nnew_n19, nnew_n20);
    const auto nnew_n21 = ntk.create_not(new_n21);
    const auto new_n22 = ntk.create_and(new_n17, nnew_n21);
    const auto nnew_n17 = ntk.create_not(new_n17);
    const auto new_n23 = ntk.create_and(nnew_n17, new_n21);
    const auto po1 = ntk.create_or(new_n22, new_n23);
    const auto new_n25 = ntk.create_and(npi3, npi4);
    const auto new_n26 = ntk.create_and(pi3, pi4);
    const auto nnew_n26 = ntk.create_not(new_n26);
    const auto new_n27 = ntk.create_and(new_n21, nnew_n26);
    const auto nnew_n25 = ntk.create_not(new_n25);
    const auto nnew_n27 = ntk.create_not(new_n27);
    const auto po2 = ntk.create_and(nnew_n25, nnew_n27);

    ntk.create_po(po0, "po0");
    ntk.create_po(po1, "po1");
    ntk.create_po(po2, "po2");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> f18_parity()
{
    mockturtle::names_view<Ntk> ntk{};
    const auto pi00 = ntk.create_pi("pi00");
    const auto pi01 = ntk.create_pi("pi01");
    const auto pi02 = ntk.create_pi("pi02");
    const auto pi03 = ntk.create_pi("pi03");
    const auto pi04 = ntk.create_pi("pi04");
    const auto pi05 = ntk.create_pi("pi05");
    const auto pi06 = ntk.create_pi("pi06");
    const auto pi07 = ntk.create_pi("pi07");
    const auto pi08 = ntk.create_pi("pi08");
    const auto pi09 = ntk.create_pi("pi09");
    const auto pi10 = ntk.create_pi("pi10");
    const auto pi11 = ntk.create_pi("pi11");
    const auto pi12 = ntk.create_pi("pi12");
    const auto pi13 = ntk.create_pi("pi13");
    const auto pi14 = ntk.create_pi("pi14");
    const auto pi15 = ntk.create_pi("pi15");
    const auto npi15 = ntk.create_not(pi15);
    const auto new_n18 = ntk.create_and(pi14, npi15);
    const auto npi14 = ntk.create_not(pi14);
    const auto new_n19 = ntk.create_and(npi14, pi15);
    const auto nnew_n18 = ntk.create_not(new_n18);
    const auto nnew_n19 = ntk.create_not(new_n19);
    const auto new_n20 = ntk.create_and(nnew_n18, nnew_n19);
    const auto npi13 = ntk.create_not(pi13);
    const auto new_n21 = ntk.create_and(pi12, npi13);
    const auto npi12 = ntk.create_not(pi12);
    const auto new_n22 = ntk.create_and(npi12, pi13);
    const auto nnew_n21 = ntk.create_not(new_n21);
    const auto nnew_n22 = ntk.create_not(new_n22);
    const auto new_n23 = ntk.create_and(nnew_n21, nnew_n22);
    const auto nnew_n23 = ntk.create_not(new_n23);
    const auto new_n24 = ntk.create_and(new_n20, nnew_n23);
    const auto nnew_n20 = ntk.create_not(new_n20);
    const auto new_n25 = ntk.create_and(nnew_n20, new_n23);
    const auto nnew_n24 = ntk.create_not(new_n24);
    const auto nnew_n25 = ntk.create_not(new_n25);
    const auto new_n26 = ntk.create_and(nnew_n24, nnew_n25);
    const auto npi11 = ntk.create_not(pi11);
    const auto new_n27 = ntk.create_and(pi10, npi11);
    const auto npi10 = ntk.create_not(pi10);
    const auto new_n28 = ntk.create_and(npi10, pi11);
    const auto nnew_n27 = ntk.create_not(new_n27);
    const auto nnew_n28 = ntk.create_not(new_n28);
    const auto new_n29 = ntk.create_and(nnew_n27, nnew_n28);
    const auto npi09 = ntk.create_not(pi09);
    const auto new_n30 = ntk.create_and(pi08, npi09);
    const auto npi08 = ntk.create_not(pi08);
    const auto new_n31 = ntk.create_and(npi08, pi09);
    const auto nnew_n30 = ntk.create_not(new_n30);
    const auto nnew_n31 = ntk.create_not(new_n31);
    const auto new_n32 = ntk.create_and(nnew_n30, nnew_n31);
    const auto nnew_n32 = ntk.create_not(new_n32);
    const auto new_n33 = ntk.create_and(new_n29, nnew_n32);
    const auto nnew_n29 = ntk.create_not(new_n29);
    const auto new_n34 = ntk.create_and(nnew_n29, new_n32);
    const auto nnew_n33 = ntk.create_not(new_n33);
    const auto nnew_n34 = ntk.create_not(new_n34);
    const auto new_n35 = ntk.create_and(nnew_n33, nnew_n34);
    const auto nnew_n35 = ntk.create_not(new_n35);
    const auto new_n36 = ntk.create_and(new_n26, nnew_n35);
    const auto nnew_n26 = ntk.create_not(new_n26);
    const auto new_n37 = ntk.create_and(nnew_n26, new_n35);
    const auto nnew_n36 = ntk.create_not(new_n36);
    const auto nnew_n37 = ntk.create_not(new_n37);
    const auto new_n38 = ntk.create_and(nnew_n36, nnew_n37);
    const auto npi07 = ntk.create_not(pi07);
    const auto new_n39 = ntk.create_and(pi06, npi07);
    const auto npi06 = ntk.create_not(pi06);
    const auto new_n40 = ntk.create_and(npi06, pi07);
    const auto nnew_n39 = ntk.create_not(new_n39);
    const auto nnew_n40 = ntk.create_not(new_n40);
    const auto new_n41 = ntk.create_and(nnew_n39, nnew_n40);
    const auto npi05 = ntk.create_not(pi05);
    const auto new_n42 = ntk.create_and(pi04, npi05);
    const auto npi04 = ntk.create_not(pi04);
    const auto new_n43 = ntk.create_and(npi04, pi05);
    const auto nnew_n42 = ntk.create_not(new_n42);
    const auto nnew_n43 = ntk.create_not(new_n43);
    const auto new_n44 = ntk.create_and(nnew_n42, nnew_n43);
    const auto nnew_n44 = ntk.create_not(new_n44);
    const auto new_n45 = ntk.create_and(new_n41, nnew_n44);
    const auto nnew_n41 = ntk.create_not(new_n41);
    const auto new_n46 = ntk.create_and(nnew_n41, new_n44);
    const auto nnew_n45 = ntk.create_not(new_n45);
    const auto nnew_n46 = ntk.create_not(new_n46);
    const auto new_n47 = ntk.create_and(nnew_n45, nnew_n46);
    const auto npi03 = ntk.create_not(pi03);
    const auto new_n48 = ntk.create_and(pi02, npi03);
    const auto npi02 = ntk.create_not(pi02);
    const auto new_n49 = ntk.create_and(npi02, pi03);
    const auto nnew_n48 = ntk.create_not(new_n48);
    const auto nnew_n49 = ntk.create_not(new_n49);
    const auto new_n50 = ntk.create_and(nnew_n48, nnew_n49);
    const auto npi01 = ntk.create_not(pi01);
    const auto new_n51 = ntk.create_and(pi00, npi01);
    const auto npi00 = ntk.create_not(pi00);
    const auto new_n52 = ntk.create_and(npi00, pi01);
    const auto nnew_n51 = ntk.create_not(new_n51);
    const auto nnew_n52 = ntk.create_not(new_n52);
    const auto new_n53 = ntk.create_and(nnew_n51, nnew_n52);
    const auto nnew_n53 = ntk.create_not(new_n53);
    const auto new_n54 = ntk.create_and(new_n50, nnew_n53);
    const auto nnew_n50 = ntk.create_not(new_n50);
    const auto new_n55 = ntk.create_and(nnew_n50, new_n53);
    const auto nnew_n54 = ntk.create_not(new_n54);
    const auto nnew_n55 = ntk.create_not(new_n55);
    const auto new_n56 = ntk.create_and(nnew_n54, nnew_n55);
    const auto nnew_n56 = ntk.create_not(new_n56);
    const auto new_n57 = ntk.create_and(new_n47, nnew_n56);
    const auto nnew_n47 = ntk.create_not(new_n47);
    const auto new_n58 = ntk.create_and(nnew_n47, new_n56);
    const auto nnew_n57 = ntk.create_not(new_n57);
    const auto nnew_n58 = ntk.create_not(new_n58);
    const auto new_n59 = ntk.create_and(nnew_n57, nnew_n58);
    const auto nnew_n59 = ntk.create_not(new_n59);
    const auto new_n60 = ntk.create_and(new_n38, nnew_n59);
    const auto nnew_n38 = ntk.create_not(new_n38);
    const auto new_n61 = ntk.create_and(nnew_n38, new_n59);
    const auto po0 = ntk.create_or(new_n60, new_n61);

    ntk.create_po(po0, "po0");

    return ntk;
}

}

#endif  // FICTION_FONTES18_BLUEPRINTS_HPP
