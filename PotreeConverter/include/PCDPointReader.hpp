/*!
 * @file
 * @author Marcus Meeßen
 * @copyright Copyright (c) 2018      Marcus Meeßen
 * @copyright Copyright (c) 2018      MASKOR Institute FH Aachen
 */

// further documentation about the pcd format can be found here
// http://pointclouds.org/documentation/tutorials/pcd_file_format.php

#ifndef POTREE_PCD_POINT_READER_HPP
#define POTREE_PCD_POINT_READER_HPP

#include "Point.h"
#include "PointReader.h"
#include "PotreeException.h"

#include <functional>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::function;
using std::ifstream;
using std::istringstream;
using std::vector;

namespace Potree {
    class PCDPointReader :
            public PointReader {
    public:
        explicit PCDPointReader(string const &file) :
                stream(file, std::ios::in | std::ios::binary) {
            // processing head
            string line;
            while (!stream.eof()) {
                getline(stream, line);
                istringstream lineStream(line);
                string keyword;
                lineStream >> keyword;

                if (keyword[0] == '#') {
                    continue; // skip comments
                } else if (keyword == "VERSION") {
                    string version;
                    lineStream >> version;
                    vector<string> versionSplit = split(version, std::vector<char>{'.'});
                    if (versionSplit.size() < 2) {
                        std::cerr << "Wrong version format." << std::endl;
                        break;
                    }
                    if (atoi(versionSplit[0].c_str()) != 0 || atoi(versionSplit[1].c_str()) < 7) {
                        std::cerr << "Unsupported .pcd version." << std::endl;
                        break;
                    }

                } else if (keyword == "FIELDS") {
                    string field;
                    while (lineStream >> field) {
                        fields.push_back(field);
                    }
                } else if (keyword == "SIZE") {
                    uint8_t size;
                    while (lineStream >> size) {
                        sizes.push_back(size);
                    }
                } else if (keyword == "TYPE") {
                    char type;
                    while (lineStream >> type) {
                        types.push_back(type);
                    }
                } else if (keyword == "COUNT") {
                    uint8_t count;
                    while (lineStream >> count) {
                        counts.push_back(count);
                    }
                } else if (keyword == "WIDTH") {
                    lineStream >> width;
                } else if (keyword == "HEIGHT") {
                    lineStream >> height;
                } else if (keyword == "VIEWPOINT") {

                } else if (keyword == "POINTS") {
                    lineStream >> points;
                } else if (keyword == "DATA") {
                    lineStream >> data;
                    break; // end of header as per format definition
                } else {
                    std::cerr << "Unknown keyword :" << keyword << std::endl;
                }
            }

            // do some integrity checks
            if (fields.size() != sizes.size() || sizes.size() != types.size()) {
                throw PotreeException("Integrity check failed.");
            }

            std::cout << byteReader<double, float>();

            // construct stream pipeline
            if (data == "binary") {
                for (size_t i = 0; i < fields.size(); i++) {
                    switch (sizes.at(i)) {
                        case 1:
                            switch (types.at(i)) {
                                case 'I':
                                    break;
                                case 'U':
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case 2:
                            switch (types.at(i)) {
                                case 'I':
                                    break;
                                case 'U':
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case 4:
                            switch (types.at(i)) {
                                case 'I':
                                    break;
                                case 'U':
                                    break;
                                case 'F':
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case 8:
                            switch (types.at(i)) {
                                case 'F':
                                    break;
                                default:
                                    break;
                            }
                            break;
                    }

                }
            }

            std::cout << "Reading .pcd file with " << points << " points in " << data << " mode. (" << width << "x"
                      << height << ")";

        }

        ~PCDPointReader() override = default;

        bool readNextPoint() override {
            static int count = 0;

            if (data == "binary") {
                readNextPointBinary();
            }

            return count++ < 1000;
        }

        Point getPoint() override {
            return point;
        }

        AABB getAABB() override {
            return AABB();
        }

        long long int numPoints() override {
            return (long long) points;
        }

        void close() override {
            stream.close();
        }

    private:
        void readNextPointBinary() {
            Point newPoint;

            if (!stream.good() && stream.eof()) {
                std::cerr << "File error";
                return;
            }

            byteReader<double, float>();

            // Vector3<double> position{0};
            // Vector3<unsigned char> color{255};
            // Vector3<float> normal{0};
            // unsigned short intensity = 0;
            // unsigned char classification = 0;
            // unsigned char returnNumber = 0;
            // unsigned char numberOfReturns = 0;
            // unsigned short pointSourceID = 0;
            // double gpsTime = 0.0;

        }

        void readNextPointAscii() {
            // TODO: binary first
        }

        template<typename Return, typename Read>
        Return byteReader() {
            char bytes[sizeof(Read)];
            stream.read(bytes, std::streamsize(sizeof(Read)));
            Read data;
            memcpy(&bytes, &data, sizeof(Read));
            std::cout << bytes << " " << data << " " << sizeof(Read) << std::endl;

            return static_cast<Return>(data);
        }

    private:
        Point point;
        ifstream stream;
        uint64_t height{0}; // height of the point cloud in the number of points (important for ordered point clouds)
        uint64_t width{0}; // width of the point cloud in the number of points (important for ordered point clouds)
        uint64_t points{0}; // total number of points in the point cloud
        vector<string> fields; // names of dimensions (x/y/z, rgb, normal_x/y/z, ...)
        vector<uint8_t> sizes; // sizes of dimensions in bytes
        vector<char> types; // types of dimensions (I=signed integral, U=unsigned integral, F=floating point)
        vector<uint8_t> counts; // number of elements per dimension
        string data; // data type that the point cloud data is stored in (ascii, binary)

    };
}

#endif // POTREE_PCD_POINT_READER_HPP
