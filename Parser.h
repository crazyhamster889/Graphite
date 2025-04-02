#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cmath>
using namespace std;


#define PI 3.14159265358979323846 

enum types { DELIMITER = 1, VARIABLE, NUMBER, FUNCTION };
const int NUMVARS = 26;

class parser {
private:
    char* expressionPointer; // points to the expression
    char token[256]; // holds current token
    char tokenIdentifier; // holds token's type
    double vars[NUMVARS]; // holds variable's values
    char equationCharArray[1024];
    void VariableAssignment(double& result);
    void EvaluateAddition(double& result);
    void EvaluateMultiplication(double& result);
    void ExponentEvaluation(double& result);
    void UnaryEvaluation(double& result);
    void EvaluateFunction(double& result);
    void RegisterToken();
public:
    float EvaluateExpression(const string& exp);
    char errormsg[64];
};

#endif 