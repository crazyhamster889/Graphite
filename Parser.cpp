
#include "Parser.h"
#include "Algorithms.h"

#include <unordered_map>
#include <functional>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cmath>
using namespace std;

/* Order of operations
* 
* Variable assignment
* Functions
* Exponent Evaluation
* Multiplication
* Addition and subtraction
* Unary Evaluation 
*/

// Parser entry point.
float parser::EvaluateExpression(const string& exp)
{
    errormsg[0] = '\0';
    double result;

    // Copy the expression to EQUATION and ensure null-termination
    strncpy_s(equationCharArray, exp.c_str(), sizeof(equationCharArray) - 1);
    equationCharArray[sizeof(equationCharArray) - 1] = '\0';

    expressionPointer = equationCharArray;
    RegisterToken();

    // Check if the expression is empty
    if (!*token)
    {
        strncpy_s(errormsg, "No Expression Present", sizeof(errormsg) - 1);
        return 0.0;
    }

    VariableAssignment(result);

    // Check for syntax errors
    if (*token)
    {
        strncpy_s(errormsg, "Syntax Error", sizeof(errormsg) - 1);
    }

    return result;
}
// Process an assignment.
void parser::VariableAssignment(double& result)
{
	int slot;
	char tempToken[80];
	if (tokenIdentifier == VARIABLE)
	{
		// save old token
		char* tokenPointer = expressionPointer;
		strncpy_s(tempToken, token, 20);
		// compute the index of the variable
		slot = *token - 'A';
		RegisterToken();
		if (*token != '=')
		{
			// return current token
			expressionPointer = tokenPointer; 
			// restore old token
			strncpy_s(token, tempToken, 100); 
			tokenIdentifier = VARIABLE;
		}
		else {
			// get next part of expression 
			RegisterToken();
			EvaluateAddition(result);

			vars[slot] = result;
			return;
		}
	}
	EvaluateAddition(result);
}
// Add or subtract two terms.
void parser::EvaluateAddition(double& result)
{
	// create the value stack and the operation stack
	stack<char> operationStack;
	stack<double> valueStack;
	valueStack.push(0);
	EvaluateMultiplication(result);
	valueStack.top() = result;

	// use stacks to push tokens onto the operation stack
	// and push the result of any multiplication onto the value stack
	while (*token == '+' || *token == '-')
	{
		char operation = *token;
		operationStack.push(operation);
		RegisterToken();
		EvaluateMultiplication(result);
		valueStack.push(result);
	}
	// use stacks to pop tokens onto from the operations stack
	// use stacks to pop the left and right results and then apply the operator
	while (!operationStack.empty()) {
		double right = valueStack.top(); 
		valueStack.pop();
		double left = valueStack.top(); 
		valueStack.pop();
		char operation = operationStack.top(); 
		operationStack.pop();

		if (operation == '+')
			left += right;
		else 
			left -= right;

		valueStack.push(left);
	}
	// get the result of the calculations
	result = valueStack.top();
}
// Multiply or divide two factors.
void parser::EvaluateMultiplication(double& result)
{
	stack<char> operationStack;
	stack<double> valueStack;
	valueStack.push(0);
	ExponentEvaluation(result);
	valueStack.top() = result;

	while (*token == '*' || *token == '/')
	{
		char operation = *token;
		operationStack.push(operation);
		RegisterToken();
		ExponentEvaluation(result);
		valueStack.push(result);
	}
	while (!operationStack.empty()) {
		double right = valueStack.top();
		valueStack.pop();
		double left = valueStack.top();
		valueStack.pop();
		char operation = operationStack.top();
		operationStack.pop();

		if (operation == '*')
			left *= right;
		else
			left /= right;

		valueStack.push(left);
	}
	result = valueStack.top();
}
// Process an exponent.
void parser::ExponentEvaluation(double& result)
{
	double temp;
	UnaryEvaluation(result);
	while (*token == '^')
	{
		RegisterToken();
		UnaryEvaluation(temp);
		result = pow(result, temp);
	}
}
// Evaluate a unary + or -.
void parser::UnaryEvaluation(double& result)
{
	register char unaryOperator;
	unaryOperator = 0;
	if ((tokenIdentifier == DELIMITER) && *token == '+' || *token == '-')
	{
		unaryOperator = *token;
		RegisterToken();
	}
	EvaluateFunction(result);
	if (unaryOperator == '-')
		result = -result;
}

// Evaluate a function
void parser::EvaluateFunction(double& result) {
    bool isfunction = (tokenIdentifier == FUNCTION);
    string tempToken = token;

    if (isfunction) {
        RegisterToken();
    }

    if (*token == '(') {
        RegisterToken();
        EvaluateAddition(result);

        if (isfunction) {
            // shortcut for mapping strings to program functions
            const map<string, function<double(double)>> functionMap = {
                {"SIN",   [](double x) { Algorithms algorithms; return algorithms.SinExpansion(x, 5); }},
                {"COS",   [](double x) { Algorithms algorithms; return algorithms.CosExpansion(x, 5); }},
                {"TAN",   [](double x) { Algorithms algorithms; return algorithms.SinExpansion(x, 5) / algorithms.CosExpansion(x, 5); }},
                {"ASIN",  [](double x) { return asin(x); }}, {"INT",   [](double x) { return floor(x); }},
                {"ACOS",  [](double x) { return acos(x); }}, {"ATAN",  [](double x) { return atan(x); }},
                {"SINH",  [](double x) { return sinh(x); }}, {"COSH",  [](double x) { return cosh(x); }},
                {"TANH",  [](double x) { return tanh(x); }}, {"ASINH", [](double x) { return asinh(x); }},
                {"ACOSH", [](double x) { return acosh(x); }}, {"ATANH", [](double x) { return atanh(x); }},
                {"SIGN",  [](double x) { return (x > 0) ? 1 : ((x < 0) ? -1 : 0); }}, {"LN",    [](double x) { return log(x); }},
                {"LOG",   [](double x) { return log10(x); }}, {"EXP",   [](double x) { return exp(x); }},
                {"SQRT",  [](double x) { return sqrt(x); }},{"ABS",   [](double x) { return abs(x); }},
                {"SQR",   [](double x) { return x * x; }}, {"ROUND", [](double x) { return round(x); }}
            };

            auto it = functionMap.find(tempToken);
            if (it != functionMap.end()) {
                result = it->second(result);
            }
        }
        RegisterToken();
    }
    else {
        switch (tokenIdentifier) {
        case VARIABLE:
            result = vars[*token - 'A'];
            break;
        case NUMBER:
            result = std::stod(token);
            break;
        default:
            return;
        }
        RegisterToken();
    }
}

// Obtain the next token.
void parser::RegisterToken()
{
	register char* tokenPointer;
	tokenIdentifier = 0;
	tokenPointer = token;
	*tokenPointer = '\0';
	// already at the end of the expression
	if (!*expressionPointer)  
		return;
	// skip over white space
	while (isspace(*expressionPointer))  
		++expressionPointer;

	// if the character in the expression at the pointer is the characters +-*/%^=()
	if (strchr("+-*/%^=()", *expressionPointer))
	{
		tokenIdentifier = DELIMITER;
		// advance to next character
		*tokenPointer++ = *expressionPointer++; 
	}

	else if (isalpha(*expressionPointer))
	{
		while (!strchr(" +-/*%^=()\t\r", *expressionPointer) && (*expressionPointer))
			*tokenPointer++ = toupper(*expressionPointer++);
		// skip over white space
		while (isspace(*expressionPointer))  
			++expressionPointer;
		// if the token is a part of a funcction or a variable, depending on whether a bracket is present
		tokenIdentifier = (*expressionPointer == '(') ? FUNCTION : VARIABLE;
	}
	// if the token is a number then we define it as a number
	else if (isdigit(*expressionPointer) || *expressionPointer == '.')
	{
		while (!strchr(" +-/*%^=()\t\r", *expressionPointer) && (*expressionPointer))
			*tokenPointer++ = toupper(*expressionPointer++);
		tokenIdentifier = NUMBER;
	}

	*tokenPointer = '\0';
}

