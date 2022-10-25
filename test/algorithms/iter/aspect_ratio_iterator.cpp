//
// Created by marcel on 13.09.21.
//

#include "catch.hpp"

#include <fiction/algorithms/iter/aspect_ratio_iterator.hpp>
#include <fiction/layouts/coordinates.hpp>

using namespace fiction;

TEST_CASE("Iteration", "[aspect-ratio-iterator]")
{
    aspect_ratio_iterator<offset::ucoord_t> ari{1};

    for (auto i = 0; ari <= 4; ++ari, ++i)
    {
        switch (i)
        {
            case 0:
            {
                CHECK(*ari == offset::ucoord_t{0, 0});
                CHECK(ari == 1);
                CHECK(ari ==
                      aspect_ratio_iterator<offset::ucoord_t>{1});  // equal since both point to the first element
                break;
            }
            case 1:
            {
                CHECK(*ari == offset::ucoord_t{0, 1});
                CHECK(ari == 2);
                CHECK(ari ==
                      aspect_ratio_iterator<offset::ucoord_t>{2});  // equal since both point to the first element
                break;
            }
            case 2:
            {
                CHECK(*ari == offset::ucoord_t{1, 0});
                CHECK(ari == 2);
                CHECK(ari !=
                      aspect_ratio_iterator<offset::ucoord_t>{2});  // not equal since ari points to the second element
                break;
            }
            case 3:
            {
                CHECK(*ari == offset::ucoord_t{0, 2});
                CHECK(ari == 3);
                CHECK(ari ==
                      aspect_ratio_iterator<offset::ucoord_t>{3});  // equal since both point to the first element
                break;
            }
            case 4:
            {
                CHECK(*ari == offset::ucoord_t{2, 0});
                CHECK(ari == 3);
                CHECK(ari !=
                      aspect_ratio_iterator<offset::ucoord_t>{3});  // not equal since ari points to the second element
                break;
            }
            case 5:
            {
                CHECK(*ari == offset::ucoord_t{0, 3});
                CHECK(ari == 4);
                CHECK(ari ==
                      aspect_ratio_iterator<offset::ucoord_t>{4});  // equal since both point to the first element
                break;
            }
            case 6:
            {
                CHECK(*ari == offset::ucoord_t{3, 0});
                CHECK(ari == 4);
                CHECK(ari !=
                      aspect_ratio_iterator<offset::ucoord_t>{4});  // not equal since ari points to the second element
                break;
            }
            case 7:
            {
                CHECK(*ari == offset::ucoord_t{1, 1});
                CHECK(ari == 4);
                CHECK(ari !=
                      aspect_ratio_iterator<offset::ucoord_t>{4});  // not equal since ari points to the third element
                break;
            }
            default:
            {
                CHECK(false);
            }
        }
    }
}
