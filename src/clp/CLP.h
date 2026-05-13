#ifndef ARTEMISII_CLP_H
#define ARTEMISII_CLP_H

#include <string>
// Command Line Parcer
class CLP
{
private:
    int argc;
    char** argv;
    double m_h;
    std::string m_method;
    std::string m_output;


public:
    CLP(int argc, char** argv);

    double h() const { return m_h; }
    std::string method() const { return m_method; }
    std::string output() const { return m_output; }
};


#endif //ARTEMISII_CLP_H