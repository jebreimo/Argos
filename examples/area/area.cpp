//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-09-05.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <cmath>
#include <iostream>
#include <Argos/Argos.hpp>

struct Point
{
    double x, y;
};

double calculate_area(Point a, Point b, Point c)
{
    auto dx1 = b.x - a.x, dy1 = b.y - a.y;
    auto dx2 = c.x - a.x, dy2 = c.y - a.y;
    return std::abs(dx1*dy2 - dx2*dy1) / 2;
}

double calculate_area(Point a, Point b, Point c, Point d)
{
    auto dx1 = c.x - a.x, dy1 = c.y - a.y;
    auto dx2 = d.x - b.x, dy2 = d.y - b.y;
    return std::abs(dx1*dy2 - dx2*dy1) / 2;
}

int main(int argc, char* argv[])
{
    using argos::ArgumentParser, argos::Arg;
    auto args = ArgumentParser("example2")
        .about("Computes the area of a triangle or quadrilateral.")
        .add(Arg("X,Y")
            .count(3, 4)
            .help("The coordinates of the corners in a triangle"
                  " or quadrilateral."))
        .parse(argc, argv);

    std::vector<Point> corners;
    for (auto value : args.values("X,Y"))
    {
        auto coordinates = value.split_n(',', 2).as_doubles();
        corners.push_back({coordinates[0], coordinates[1]});
    }

    if (corners.size() == 3)
    {
        std::cout << calculate_area(corners[0], corners[1], corners[2])
                  << "\n";
    }
    else
    {
        std::cout << calculate_area(corners[0], corners[1],
                                    corners[2], corners[3])
                  << "\n";
    }

    return 0;
}
