
#include "paragraph.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>

using namespace std;

TEST_CASE("Moving paragraph")
{
    LegacyCode::Paragraph txt("***");
    LegacyCode::Paragraph target_txt = std::move(txt);

    REQUIRE(target_txt.get_paragraph() == string("***"));
    REQUIRE(txt.get_paragraph() == nullptr);
}

TEST_CASE("Moving text shape")
{
    Text txt{10, 20, "text"};
    Text target_txt = std::move(txt);

    REQUIRE(target_txt.text() == string("text"));
    REQUIRE(txt.text() == string());
}

TEST_CASE("ShapeGroup")
{
    ShapeGroup sg;
    // sg.add(std::make_unique<Text>(10, 20, "text")); // uncomment this line

    REQUIRE(sg.shapes.size() == 1);

    Text& t = dynamic_cast<Text&>(*sg.shapes[0]);
    REQUIRE(t.text() == "text"s);
}