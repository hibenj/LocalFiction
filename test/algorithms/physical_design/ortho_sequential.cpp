//
// Created by Hien Benjamin on 12.09.2022.
//
#include "catch.hpp"
#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "mockturtle/networks/sequential.hpp"
#include "mockturtle/views/topo_view.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/networks/technology_network.hpp>

using namespace fiction;

TEST_CASE("sequ_net", "[ortho_sequential]")
{

    /*if constexpr (mockturtle::has_foreach_ro_v<NtkSrc>){
    src.foreach_ro( [&]( auto const& n ) {
      old2new[n] = dest.create_ro();
    } );
  }*/
    auto seq_one = blueprints::seq_one<mockturtle::sequential<fiction::technology_network>>();

    auto seq_two = fanout_substitution<mockturtle::names_view<mockturtle::sequential<fiction::technology_network>>>(seq_one);
    seq_one.set_network_name("seq_one");

    std::cout<<typeid(seq_one).name()<<std::endl;

    static_assert(mockturtle::has_create_ro_v<class mockturtle::names_view<class mockturtle::sequential<class fiction::technology_network> >>, "NtkDest does not implement the create_ro function");
    static_assert(mockturtle::has_foreach_ro_v<mockturtle::topo_view<class mockturtle::names_view<class mockturtle::sequential<class fiction::technology_network> >>>, "NtkDest does not implement the create_ro function");

    std::cout<<"Combinational check "<<seq_one.is_combinational()<<std::endl;

    seq_two.foreach_node(
        [&](const auto& n)
        {
            std::cout<<"Node "<<n<<std::endl;
        });

}

TEST_CASE("sequ_net_one", "[ortho_sequential]")
{

    auto seq_one = blueprints::seq_one<mockturtle::sequential<fiction::technology_network>>();

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

    seq_one.foreach_node(
        [&](const auto& n)
        {
            if(seq_one.is_pi(n))
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

    seq_one.foreach_ri(
        [&](const auto& n)
        {
            std::cout<<"Ri "<<n<<std::endl;
        });

    seq_one.foreach_co(
        [&](const auto& n)
        {
            std::cout<<"Co "<<n<<std::endl;
        });

    std::cout<<"Ro at 0 "<<seq_one.ro_at(0)<<std::endl;

    std::cout<<"Num Reg "<<seq_one.num_registers()<<std::endl;

    std::cout<<"Num Gates "<<seq_one.num_gates()<<std::endl;

    std::cout<<"Num Cis "<<seq_one.num_cis()<<std::endl;

    std::cout<<"Num Cos "<<seq_one.num_cos()<<std::endl;

}