#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

struct Token {
    std::string type;
    std::string value;
};

// Tokenizer
std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    std::string buffer;

    auto flushBuffer = [&](const std::string& type = "IDENTIFIER") {
        if (!buffer.empty()) {
            tokens.push_back({type, buffer});
            buffer.clear();
        }
    };

    for (size_t i = 0; i < input.size(); ++i) {
        char ch = input[i];

        if (isspace(ch)) {
            flushBuffer();
        }
        else if (isalpha(ch) || isdigit(ch) || ch == '_') {
            buffer += ch;
        }
        else {
            flushBuffer();

            std::string sym(1, ch);
            if (ch == ';') {
                tokens.push_back({"SEMICOLON", ";"});
            } else if (ch == '{') {
                tokens.push_back({"LBRACE", "{"});
            } else if (ch == '}') {
                tokens.push_back({"RBRACE", "}"});
            } else if (ch == '(') {
                tokens.push_back({"LPAREN", "("});
            } else if (ch == ')') {
                tokens.push_back({"RPAREN", ")"});
            }
        }
    }
    flushBuffer(); // flush any remaining characters

    // Fix token types (now that everything is split cleanly)
    for (auto& token : tokens) {
        if (token.value == "class") token.type = "CLASS";
        else if (token.value == "public:") token.type = "PUBLIC";
        else if (token.value == "void") token.type = "VOID";
        else if (token.value == "int" || token.value == "float" || token.value == "string" || token.value == "bool" || token.value == "double")
            token.type = "TYPE";
    }

    return tokens;
}


// Data structure
struct ClassInfo {
    std::string name;
    std::vector<std::pair<std::string, std::string>> attributes;
    std::vector<std::string> methods;
};

// Parser
ClassInfo parseClass(const std::vector<Token>& tokens) {
    ClassInfo info;
    int i = 0;

    if (tokens[i].type == "CLASS") {
        info.name = tokens[i + 1].value;
        i += 3; // skip "class", name, and "{"
    }

    if (tokens[i].type == "PUBLIC") {
        i++; // skip "public:"
    }

    while (i < tokens.size()) {
        if (tokens[i].type == "TYPE") {
            std::string type = tokens[i].value;
            std::string name = tokens[i + 1].value;
            info.attributes.push_back({type, name});
            i += 3; // skip type, name, semicolon
        } else if (tokens[i].type == "VOID") {
            std::string methodName = tokens[i + 1].value;
            info.methods.push_back(methodName); // DO NOT add "()" here
            i += 5; // skip void, name, (, ), ;
        } else {
            i++;
        }
    }

    return info;
}

// Python Code Generator
std::string generatePython(const ClassInfo& info) {
    std::ostringstream py;

    py << "class " << info.name << ":\n";
    py << "    def __init__(self):\n";
    for (const auto& [type, name] : info.attributes) {
        py << "        self." << name << " = ";
        if (type == "int" || type == "float" || type == "double") py << "0\n";
        else if (type == "bool") py << "False\n";
        else py << "\"\"\n";
    }
    if (info.attributes.empty()) {
        py << "        pass\n";
    }

    py << "\n";
    for (const auto& method : info.methods) {
        py << "    def " << method << "(self):\n";
        py << "        pass\n\n";
    }

    return py.str();
}

// Main
int main() {
    std::ifstream inputFile("input.txt");
    std::ofstream outputFile("output.py");

    if (!inputFile || !outputFile) {
        std::cerr << "Error opening files.\n";
        return 1;
    }

    std::string line, all;
    while (getline(inputFile, line)) {
        all += line + " ";
    }

    auto tokens = tokenize(all);
    auto classInfo = parseClass(tokens);
    std::string pythonCode = generatePython(classInfo);

    outputFile << pythonCode;
    std::cout << "Python class generated in output.py \n";
    return 0;
}
