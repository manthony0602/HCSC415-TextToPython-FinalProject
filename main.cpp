#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

// Struct to store the class name and its attributes
struct ClassInfo {
    std::string name;
    std::vector<std::string> attributes;
};

// Parses the English input sentence and pulls out the class name and attributes
ClassInfo parseInput(const std::string& input) {
    ClassInfo info;

    size_t classPos = input.find("class called");
    if (classPos == std::string::npos) {
        std::cerr << "Invalid input format (missing 'class called').\n";
        return info;
    }

    size_t withPos = input.find("with", classPos);
    if (withPos == std::string::npos) {
        std::cerr << "Invalid input format (missing 'with').\n";
        return info;
    }

    // Grab the class name
    size_t classNameStart = classPos + std::string("class called").length() + 1;
    info.name = input.substr(classNameStart, withPos - classNameStart);
    info.name.erase(std::remove(info.name.begin(), info.name.end(), ' '), info.name.end());

    // Grab the attributes section
    size_t fieldsPos = input.find("populate", withPos);
    std::string attributesSection = input.substr(withPos + 5, fieldsPos - withPos - 5);

    // Split the attributes by spaces and filter out "and" and commas
    std::istringstream attrStream(attributesSection);
    std::string word;
    while (attrStream >> word) {
        if (word == "and" || word.empty()) continue;

        // Remove any commas stuck to the words
        if (word.back() == ',') {
            word.pop_back();
        }

        info.attributes.push_back(word);
    }

    return info;
}

// Builds the Python class string
std::string generatePython(const ClassInfo& info) {
    std::ostringstream py;
    py << "class " << info.name << ":\n";
    py << "    def __init__(self):\n";
    for (const auto& attr : info.attributes) {
        py << "        self." << attr << " = None\n";
    }
    return py.str();
}

// Builds the C++ class string
std::string generateCPP(const ClassInfo& info) {
    std::ostringstream cpp;
    cpp << "class " << info.name << " {\npublic:\n";
    for (const auto& attr : info.attributes) {
        cpp << "    std::string " << attr << ";\n";
    }
    cpp << "};";
    return cpp.str();
}

// Builds the Java class string
std::string generateJava(const ClassInfo& info) {
    std::ostringstream java;
    java << "public class " << info.name << " {\n";
    for (const auto& attr : info.attributes) {
        java << "    public String " << attr << ";\n";
    }
    java << "}";
    return java.str();
}

int main() {
    // Open the input and output files
    std::ifstream inputFile("input.txt");
    std::ofstream outputFile("output.txt");

    if (!inputFile || !outputFile) {
        std::cerr << "Error opening files.\n";
        return 1;
    }

    // Read the whole input file into a single string
    std::string line, allInput;
    while (getline(inputFile, line)) {
        allInput += line + " ";
    }

    auto classInfo = parseInput(allInput);

    if (classInfo.name.empty() || classInfo.attributes.empty()) {
        std::cerr << "Failed to parse input properly.\n";
        return 1;
    }

    // Print the parse tree so we can see what got extracted
    std::cout << "[Parse Tree]\n";
    std::cout << "  Class Name: " << classInfo.name << "\n";
    std::cout << "  Attributes:\n";
    for (const auto& attr : classInfo.attributes) {
        std::cout << "    - " << attr << "\n";
    }
    std::cout << "\n";

    // Generate the class code for each language
    std::string pythonCode = generatePython(classInfo);
    std::string cppCode = generateCPP(classInfo);
    std::string javaCode = generateJava(classInfo);

    // Write all the code to the output file
    outputFile << "# Python\n";
    outputFile << pythonCode << "\n\n";
    outputFile << "# C++\n";
    outputFile << cppCode << "\n\n";
    outputFile << "# Java\n";
    outputFile << javaCode << "\n";

    std::cout << "Output generated in output.txt\n";
    return 0;
}
