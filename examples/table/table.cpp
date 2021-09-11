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
               " demonstrates how option and argument callbacks can be used.")
        .add(Argument("TEXT").count(1, UINT16_MAX)
            .callback([&](auto, auto text, auto)
                      {
                          cells.push_back({std::string(text), row, col});
                          ++col;
                          return true;
                      })
            .help("Text of the next table cell."))
        .add(Option{"-o", "--output"}.argument("FILE")
            .help("File name for output. stdout is used by default."))
        .add(Option{"-r", "--row"}
            .callback([&](auto, auto, auto){++row; col = 0; return true;})
            .help("Next file will be placed at the beginning of a new row."))
        .add(Option{"-c", "--column"}
            .callback([&](auto, auto, auto){++col; return true;})
            .help("Skip one column forward."))
        .add(Option{"--borders"}.help("Print borders between cells."))
        .parse(argc, argv);

    if (cells.empty())
        args.error("no input");

    auto borders = args.value("--borders").as_bool();

    // Calculate column widths.
    std::vector<size_t> col_widths;
    for (auto cell : cells)
    {
        if (cell.col >= col_widths.size())
            col_widths.resize(cell.col + 1);
        col_widths[cell.col] = std::max(col_widths[cell.col], cell.text.size());
    }
    auto table_width = std::accumulate(col_widths.begin(), col_widths.end(), 0)
                       + col_widths.size() - 1;

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
                stream << std::setw(col_widths[j]) << "";
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
            stream << std::setw(spaces) << "";
        }
        stream << '\n';
    }

    return 0;
}
