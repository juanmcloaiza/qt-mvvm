#ifndef IOLIB_H
#define IOLIB_H

#include <iterator>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <memory>
#include <string>
#include <map>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <exception>


namespace{
inline std::string trim(const std::string& str,
                        const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);

    if (strBegin == std::string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}
}


void readLineOfDoubles(std::vector<double>& buffer, std::istringstream& iss)
{
    iss.imbue(std::locale::classic());
    std::copy(std::istream_iterator<double>(iss),
              std::istream_iterator<double>(), back_inserter(buffer));
}


std::vector<double> parse_doubles(const std::string& str)
{
    std::vector<double> result;
    std::istringstream iss(str);
    readLineOfDoubles(result, iss);
    if( result.empty() ) {
        std::string out = str;
        const size_t max_string_length(10);
        if(out.size() > max_string_length)
            out.resize(max_string_length, ' ');
        out += " ...";
        throw std::runtime_error("DataFormatUtils::parse_doubles -> Error! Can't parse double "
                                 "values from a string '"+out+"'");
    }
    return result;
}


std::vector<double> readReflectivity(std::istream& fin)
{
    std::string line;
    std::vector<std::vector<double>> vecVec;
    std::map<double,double> QvsR;
    std::map<double,double> QvsDR;
    std::map<double,double> QvsDQ;

    //Read numbers from file:
    while( std::getline(fin, line) ) {
        line = trim(line);
        try {
            std::vector<double> rowVec = parse_doubles(line);
            vecVec.push_back(rowVec);
        } catch (...) {
            continue;
        }
    }

    // validate - There is at least one row and at least two columns
    size_t nrows = vecVec.size();
    if (nrows < 1)
        throw std::runtime_error("Import1dTextData: no numerical values found");
    size_t ncols = vecVec[0].size();
    if (ncols < 2)
        throw std::runtime_error("Import1dTextData: Minimum 2 columns required");

    // Assign Q vs R, dR, dQ:
    for(size_t row=0; row<nrows; row++) {
        if(vecVec[row].size() != ncols)
            throw std::runtime_error("The number of columns varies among the rows");
        double Q = vecVec[row][0];
        switch(ncols){
        case 1:
            break;
        case 2:
            QvsR[Q]  = vecVec[row][1];
            QvsDR[Q] = 0;
            QvsDQ[Q] = 0;
            break;
        case 3:
            QvsR[Q]  = vecVec[row][1];
            QvsDR[Q] = vecVec[row][2];
            QvsDQ[Q] = 0;
            break;
        default:
            QvsR[Q]  = vecVec[row][1];
            QvsDR[Q] = vecVec[row][2];
            QvsDQ[Q] = vecVec[row][3];
            break;
        }
    }


    std::vector<double> qVec;
    std::vector<double> rVec;
    for(auto it = QvsR.begin(); it != QvsR.end(); ++it) {
        if(it->second <= 0)
            continue;
        qVec.push_back(it->first);
        rVec.push_back(it->second);
    }

    return rVec;
}




std::vector<double> getFromFilteredStream(std::istream& input_stream)
{
    boost::iostreams::filtering_streambuf<boost::iostreams::input> input_filtered;
//    if (DataFormatUtils::isGZipped(m_file_name))
//        input_filtered.push(boost::iostreams::gzip_decompressor());
//    else if (DataFormatUtils::isBZipped(m_file_name))
//        input_filtered.push(boost::iostreams::bzip2_decompressor());
    input_filtered.push(input_stream);
    // we use stringstream since it provides random access which is important for tiff files
    std::stringstream strstream;
    boost::iostreams::copy(input_filtered, strstream);
    return readReflectivity(strstream);
}


std::vector<double> getOutputData(const std::string& filename)
{
    std::ifstream fin;
    std::ios_base::openmode openmode = std::ios::in;
//if(isTiffFile(filename) || isCompressed(filename))
//        openmode = std::ios::in | std::ios_base::binary;
//#ifdef _WIN32
//    fin.open(FileSystemUtils::convert_utf8_to_utf16(filename), openmode);
//#else
    fin.open(filename, openmode);
//#endif

//	if(!fin.is_open())
//        throw exception
//            "Error. Can't open file '"+ filename + "' for reading.");
//    if (!fin.good())
//        throw Exceptions::FileIsBadException(
//            "OutputDataReader::getOutputData() -> Error! "
//            "File is not good, probably it is a directory.");
    std::vector<double> result = getFromFilteredStream(fin);
    fin.close();
    return result;
}

#endif // IOLIB_H
