#include <iostream>
#include <fstream>
#include <string>

int main (int argc, char* argv[]) {

    if (argc < 2) {
        std:: cout << "no file mentioned" << std:: endl;
        exit(0);
    }

    std:: string test_dir_path= "/home/professor/Programming/C_C++/codeTestingTool/";
    std:: string fileName= argv[1];

    fout.open(test_dir_path+fileName, std::ios::out);

    // create cpp file
    if (fileName.substr(fileName.find(".")+1) == "cpp") {
        std:: fstream fout;
        fout << "#include <iostream>\n\n#define log(x) std:: cout << x\n#define read(x) std:: cin >> x\n\nint main () {\n\nreturn 0;\n}";
        std:: cout << "cpp file" << std:: endl;
        fout.close();
    }
    // create python file
    else if (fileName.substr(fileName.find(".")+1) == "py") {
        std:: cout << "python file" << std:: endl;
    }
    // wrong file extension mentioned
    else {
        std:: cout << "wrong file type" << std:: endl;
        exit(0);
    }

return 0;
}