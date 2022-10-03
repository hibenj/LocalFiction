//
// Created by Hien Benjamin on 12.09.2022.
//
#include "catch.hpp"
#include "fiction/algorithms/network_transformation/fanout_inverter_balancing.hpp"
#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/networks/views/topo_view_input_sort.hpp"
#include "mockturtle/networks/sequential.hpp"
#include "mockturtle/views/topo_view.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/networks/technology_network.hpp>

using namespace fiction;

TEST_CASE("sequ_net", "[ortho_sequential]")
{

    auto seq_one = blueprints::seq_one<mockturtle::sequential<technology_network>>();

    auto seq_two = topo_view_input_sort{mockturtle::fanout_view{inverter_balancing(fanout_substitution<mockturtle::names_view<mockturtle::sequential<fiction::technology_network>>>(seq_one))}};
    seq_one.set_network_name("seq_one");

    //std::cout<<typeid(seq_one).name()<<std::endl;

    static_assert(mockturtle::has_create_ro_v<class mockturtle::names_view<class mockturtle::sequential<class fiction::technology_network> >>, "NtkDest does not implement the create_ro function");
    static_assert(mockturtle::has_foreach_ro_v<mockturtle::topo_view<class mockturtle::names_view<class mockturtle::sequential<class fiction::technology_network> >>>, "NtkDest does not implement the create_ro function");

    std::cout<<"Combinational check "<<seq_one.is_combinational()<<std::endl;

    seq_two.foreach_node(
        [&](const auto& n)
        {
            std::cout<<"Node "<<n<<std::endl;
        });

    seq_two.foreach_pi(
        [&](const auto& n)
        {
            std::cout<<"Pis "<<n<<std::endl;
        });

    seq_two.foreach_ci(
        [&](const auto& n)
        {
            std::cout<<"Cis "<<n<<std::endl;
        });

    seq_two.foreach_ro(
        [&](const auto& n)
        {
            std::cout<<"Ros "<<n<<std::endl;
        });

    seq_two.foreach_node(
        [&](const auto& n)
        {
            if(seq_two.is_and(n))
                std::cout<<"And "<<n<<std::endl;
        });

    seq_two.foreach_node(
        [&](const auto& n)
        {
            if(seq_two.is_inv(n))
                std::cout<<"Inv "<<n<<std::endl;
        });

    seq_two.foreach_node(
        [&](const auto& n)
        {
            if(seq_two.is_buf(n))
                std::cout<<"Buf "<<n<<std::endl;
        });

    seq_two.foreach_co(
        [&](const auto& n)
        {
            std::cout<<"Co "<<n<<std::endl;
        });

    seq_two.foreach_po(
        [&](const auto& n)
        {
            std::cout<<"Po "<<n<<std::endl;
        });
    seq_two.foreach_ri(
        [&](const auto& n)
        {
            std::cout<<"Ri "<<n<<std::endl;
        });

}

TEST_CASE("fo_sub and ntk_conv for sequential", "[ortho_sequential]")
{

    auto seq_one = blueprints::seq_one<mockturtle::sequential<mockturtle::aig_network>>();

    auto seq_two = fanout_substitution<mockturtle::names_view<mockturtle::sequential<fiction::technology_network>>>(seq_one);
    seq_one.set_network_name("seq_one");

    //std::cout<<typeid(seq_one).name()<<std::endl;

    static_assert(mockturtle::has_create_ro_v<class mockturtle::names_view<class mockturtle::sequential<class fiction::technology_network> >>, "NtkDest does not implement the create_ro function");
    static_assert(mockturtle::has_foreach_ro_v<mockturtle::topo_view<class mockturtle::names_view<class mockturtle::sequential<class fiction::technology_network> >>>, "NtkDest does not implement the create_ro function");

    std::cout<<"Combinational check "<<seq_one.is_combinational()<<std::endl;

    seq_two.foreach_node(
        [&](const auto& n)
        {
            std::cout<<"Node "<<n<<std::endl;
        });

    seq_two.foreach_pi(
        [&](const auto& n)
        {
            std::cout<<"Pis "<<n<<std::endl;
        });

    seq_two.foreach_ci(
        [&](const auto& n)
        {
            std::cout<<"Cis "<<n<<std::endl;
        });

    seq_two.foreach_ro(
        [&](const auto& n)
        {
            std::cout<<"Ros "<<n<<std::endl;
        });

    seq_two.foreach_node(
        [&](const auto& n)
        {
            if(seq_two.is_and(n))
                std::cout<<"And "<<n<<std::endl;
        });

    seq_two.foreach_ri(
        [&](const auto& n)
        {
            std::cout<<"Ri "<<n<<std::endl;
        });

    seq_two.foreach_node(
        [&](const auto& n)
        {
            if(seq_two.is_buf(n))
                std::cout<<"Buf "<<n<<std::endl;
        });

    seq_two.foreach_co(
        [&](const auto& n)
        {
            std::cout<<"Co "<<n<<std::endl;
        });

}

TEST_CASE("sequ_net_one", "[ortho_sequential]")
{

    auto seq_one = blueprints::seq_one<mockturtle::sequential<mockturtle::aig_network>>();

    seq_one.set_network_name("seq_one");

    std::cout<<typeid(seq_one).name()<<std::endl;

    //static_assert(mockturtle::has_create_ro_v<mockturtle::topo_view<class mockturtle::names_view<class mockturtle::sequential<class fiction::technology_network,0> >>>, "NtkDest does not implement the create_ro function");

    std::cout<<"Combinational check "<<seq_one.is_combinational()<<std::endl;


    seq_one.foreach_node(
        [&](const auto& n)
        {
            std::cout<<"Node "<<n<<std::endl;
        });


    seq_one.foreach_node(
    [&](const auto& n)
    {
            if(seq_one.is_constant(n))
            std::cout<<"Constants "<<n<<std::endl;
        });

    seq_one.foreach_pi(
        [&](const auto& n)
        {
            std::cout<<"Pis "<<n<<std::endl;
        });

    seq_one.foreach_node(
        [&](const auto& n)
        {
            if(seq_one.is_ci(n))
                std::cout<<"Cis "<<n<<std::endl;
        });

    seq_one.foreach_ro(
        [&](const auto& n)
        {
            std::cout<<"Ros "<<n<<std::endl;
        });

    seq_one.foreach_node(
        [&](const auto& n)
        {
            if(seq_one.is_xor(n))
                std::cout<<"Xor "<<n<<std::endl;
        });

    seq_one.foreach_node(
        [&](const auto& n)
        {
            if(seq_one.is_and(n))
                std::cout<<"And "<<n<<std::endl;
        });

    /*seq_one.foreach_node(
        [&](const auto& n)
        {
            if(seq_one.is_not(n))
                std::cout<<"Xor "<<n<<std::endl;
        });*/

    /*seq_one.foreach_ri(
        [&](const auto& n)
        {
            std::cout<<"Ri "<<n<<std::endl;
        });

    seq_one.foreach_co(
        [&](const auto& n)
        {
            std::cout<<"Co "<<n<<std::endl;
        });*/

    std::cout<<"Ro at 0 "<<seq_one.ro_at(0)<<std::endl;

    std::cout<<"Num Reg "<<seq_one.num_registers()<<std::endl;

    std::cout<<"Num Gates "<<seq_one.num_gates()<<std::endl;

    std::cout<<"Num Cis "<<seq_one.num_cis()<<std::endl;

    std::cout<<"Num Cos "<<seq_one.num_cos()<<std::endl;

}