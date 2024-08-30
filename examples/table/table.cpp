//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-09-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <Argos/Argos.hpp>

struct TableCell
{
    std::string text;
    unsigned row;
    unsigned col;
};

int main(int argc, char* argv[])
{
    unsigned row = 0, col = 0;
    std::vector<TableCell> cells;
    using namespace argos;
    auto args = ArgumentParser(argv[0])
        .about("Prints the arguments as cells in a table. The program also"
               " demonstrates one way to use option and argument callbacks.")
        .add(Arg("TEXT").count(1, UINT16_MAX)
            .callback([&](auto, auto text, auto)
                      {
                          cells.push_back({std::string(text), row, col});
                          ++col;
                      })
            .help("Text of the next table cell."))
        .add(Opt{"-o", "--output"}.argument("FILE")
            .help("File name for output. stdout is used by default."))
        .add(Opt{"-r", "--row"}
            .callback([&](auto, auto, auto){++row; col = 0;})
            .help("Next file will be placed at the beginning of a new row."))
        .add(Opt{"-c", "--column"}
            .callback([&](auto, auto, auto){++col;})
            .help("Skip one column forward."))
        .add(Opt{"--borders"}.help("Print borders between cells."))
        .parse(argc, argv);

    if (cells.empty())
        args.error("no input");

    auto borders = args.value("--borders").as_bool();

    // Calculate column widths.
    std::vector<size_t> col_widths;
    for (const auto& cell : cells)
    {
        if (cell.col >= col_widths.size())
            col_widths.resize(cell.col + 1);
        col_widths[cell.col] = std::max(col_widths[cell.col], cell.text.size());
    }

    // Initialize output stream.
    std::ofstream file;
    if (auto output = args.value("--output"))
        file.open(output.as_string());
    std::ostream& stream = file ? file : std::cout;

    // Print the table.
    auto it = cells.begin();
    for (unsigned i = 0; i <= cells.back().row; ++i)
    {
        // If borders are enabled, print a line of dashes and pluses
        // between the rows of text.
        if (borders && i != 0)
        {
            stream << std::setfill('-');
            for (unsigned j = 0; j < col_widths.size(); ++j)
            {
                if (j != 0)
                    stream << '+';
                stream << std::setw(std::streamsize(col_widths[j])) << "";
            }
            stream << std::setfill(' ') << '\n';
        }

        // Print the row of text.
        for (unsigned j = 0; j < col_widths.size(); ++j)
        {
            // Print a separator between cells.
            if (j != 0)
                stream << (borders ? '|' : ' ');

            // spaces is the width of the column minus the width of the text
            size_t spaces = col_widths[j];
            if (it != cells.end() && it->row == i && it->col == j)
            {
                stream << it->text;
                spaces -= it->text.size();
                ++it;
            }
            stream << std::setw(std::streamsize(spaces)) << "";
        }
        stream << '\n';
    }

    return 0;
}
