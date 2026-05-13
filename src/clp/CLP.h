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
    std::string m_path_to_thrust;
    std::string m_path_to_thottle;

public:
    CLP(int argc, char** argv);

    double h() const { return m_h; }
    std::string method() const { return m_method; }
    std::string output() const { return m_output; }
    std::string thrust() const { return m_path_to_thrust; }
    std::string thottle() const { return m_path_to_thottle; }
};


#endif //ARTEMISII_CLP_H