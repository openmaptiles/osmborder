/*

  Copyright 2012-2016 Jochen Topf <jochen@topf.org>.

  This file is part of OSMBorder.

  OSMBorder is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OSMBorder is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OSMBorder.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <getopt.h>

#include "return_codes.hpp"
#include "options.hpp"

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

Options::Options(int argc, char* argv[]) :
    inputfile(),
    debug(false),
    output_file(),
    overwrite_output(false),
    verbose(false) {
    static struct option long_options[] = {
        {"debug",                 no_argument, 0, 'd'},
        {"help",                  no_argument, 0, 'h'},
        {"output-file", required_argument, 0, 'o'},
        {"overwrite",             no_argument, 0, 'f'},
        {"verbose",               no_argument, 0, 'v'},
        {"version",               no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };

    while (1) {
        int c = getopt_long(argc, argv, "dho:fvV", long_options, 0);
        if (c == -1)
            break;

        switch (c) {
            case 'd':
                debug = true;
                std::cerr << "Enabled debug option\n";
                break;
            case 'h':
                print_help();
                std::exit(return_code_ok);
            case 'o':
                output_file = optarg;
                break;
            case 'f':
                overwrite_output = true;
                break;
            case 'v':
                verbose = true;
                break;
            case 'V':
                std::cout << "osmborder version " OSMBORDER_VERSION "\n"
                          << "Copyright (C) 2012-2016  Jochen Topf <jochen@topf.org>\n"
                          << "License: GNU GENERAL PUBLIC LICENSE Version 3 <http://gnu.org/licenses/gpl.html>.\n"
                          << "This is free software: you are free to change and redistribute it.\n"
                          << "There is NO WARRANTY, to the extent permitted by law.\n";
                std::exit(return_code_ok);
            default:
                std::exit(return_code_cmdline);
        }
    }

    if (optind != argc - 1) {
        std::cerr << "Usage: " << argv[0] << " [OPTIONS] OSMFILE\n";
        std::exit(return_code_cmdline);
    }

    if (output_database.empty()) {
        std::cerr << "Missing --output-database/-o option.\n";
        std::exit(return_code_cmdline);
    }

    inputfile = argv[optind];
}

int Options::get_epsg(const char* text) {
    if (!strcasecmp(text, "WGS84") || !std::strcmp(text, "4326")) {
        return 4326;
    }
    if (!std::strcmp(text, "3857")) {
        return 3857;
    }
    if (!std::strcmp(text, "3785") || !std::strcmp(text, "900913")) {
        std::cerr << "Please use code 3857 for the 'Google Mercator' projection!\n";
        std::exit(return_code_cmdline);
    }
    std::cerr << "Unknown SRS '" << text << "'. Currently only 4326 (WGS84) and 3857 ('Google Mercator') are supported.\n";
    std::exit(return_code_cmdline);
}

void Options::print_help() const {
    std::cout << "osmborder [OPTIONS] OSMFILE\n"
              << "\nOptions:\n"
              << "  -h, --help                 - This help message\n"
              << "  -c, --close-distance=DIST  - Distance between nodes under which open rings\n"
              << "                               are closed (0 - disable closing of rings)\n"
              << "  -b, --bbox-overlap=OVERLAP - Set overlap when splitting polygons\n"
              << "  -i, --no-index             - Do not create spatial indexes in output db\n"
              << "  -d, --debug                - Enable debugging output\n"
              << "  -f, --overwrite            - Overwrite output file if it already exists\n"
              << "  -m, --max-points=NUM       - Split lines/polygons with more than this many\n"
              << "                               points (0 - disable splitting)\n"
              << "  -o, --output-database=FILE - Spatialite database file for output\n"
              << "  -p, --output-polygons=land|water|both|none\n"
              << "                             - Which polygons to write out (default: land)\n"
              << "  -r, --output-rings         - Output rings to database file\n"
              << "  -s, --srs=EPSGCODE         - Set SRS (4326 for WGS84 (default) or 3857)\n"
              << "  -S, --write-segments=FILE  - Write segments to given file\n"
              << "  -v, --verbose              - Verbose output\n"
              << "  -V, --version              - Show version and exit\n"
              << "\n";
}

