/**
 * Reading a text file into memory
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <tclap/CmdLine.h>

//using namespace std;


class ReaderExample {
  public: 
    ReaderExample();
    ~ReaderExample(); 
    std::vector<std::string> *read(std::string filename);
};

ReaderExample:: ReaderExample() {
    std::cout << "Constructor ...\n";

}

ReaderExample::~ReaderExample() {
    std::cout << "Destructor ...\n";
}

std::vector<std::string> *ReaderExample::read(std::string filename) {
    std::vector<std::string> *lines = new std::vector<std::string>();
    std::ifstream infile(filename);
    
    if (!infile.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return NULL;
    }
    std::string line;
    while (getline(infile, line)) {
        lines->push_back(line);
    }
    // lines-> push_back("Hello, world!");
    infile.close();
    return lines;
}

bool getArgs(int argc, char* argv[], std::string* filename, bool* verbose) {
     try {
        std::cout << "Processing command line arguments" << std::endl;
        TCLAP::CmdLine cmd(argv[0], ' ', "1.0");
        TCLAP::ValueArg<std::string> filenameArg("f", "filename", "name of file to read", true, "sample.csv", "string");
        cmd.add(filenameArg);
        TCLAP::SwitchArg verboseArg ("v", "verbose", "detail debugging output", false);
        cmd.add(verboseArg);

        cmd.parse(argc, argv);
        *filename = filenameArg.getValue();
        *verbose = verboseArg.getValue();

        std::cout << "filename: " << *filename << std::endl;
        return true;
    }
    catch (TCLAP::ArgException& e) {
        std::cerr << "error: " << e.error() << "for arg " << e.argId() << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Program begins ...\n";
    std:: string filename = "";
    bool verbose = false;
    if (!getArgs(argc, argv, &filename, &verbose)) {
        std::cout << "Errors in providing parameters/arguments. Program exits." << std::endl;
    }
    std::cout << "verbose: " << (verbose ? "True" : "False") << std::endl;

    ReaderExample *re = new ReaderExample();
    std::vector<std::string> *lines = re->read(filename);
    std::cout << "line 0: " << lines->at(0) << std::endl;
    std::cout << "There are " << lines->size() << " lines from file " << filename << std::endl;
    delete(re);
    std::cout << "Program ends ...\n";

}
