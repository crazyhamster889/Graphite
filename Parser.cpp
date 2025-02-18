
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

// Parser entry point.
float parser::EvaluateExpression(const string& exp)
{
    errormsg[0] = '\0';
    double result;

    // Copy the expression to EQUATION and ensure null-termination
    strncpy_s(equationCharArray, exp.c_str(), sizeof(equationCharArray) - 1);
    equationCharArray[sizeof(equationCharArray) - 1] = '\0';

    exp_ptr = equationCharArray;
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
	char temp_token[80];
	if (tok_type == VARIABLE)
	{
		// save old token
		char* t_ptr = exp_ptr;
		strncpy_s(temp_token, token, 20);
		// compute the index of the variable
		slot = *token - 'A';
		RegisterToken();
		if (*token != '=')
		{
			exp_ptr = t_ptr; // return current token
			strncpy_s(token, temp_token, 100); // restore old token
			tok_type = VARIABLE;
		}
		else {
			RegisterToken(); // get next part of exp
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
	register char op;
	double temp;
	EvaluateMultiplication(result);
	while ((op = *token) == '+' || op == '-')
	{
		RegisterToken();
		EvaluateMultiplication(temp);
		switch (op)
		{
		case '-':
			result = result - temp;
			break;
		case '+':
			result = result + temp;
			break;
		}
	}
}
// Multiply or divide two factors.
void parser::EvaluateMultiplication(double& result)
{
	register char op;
	double temp;
	ExponentEvaluation(result);
	while ((op = *token) == '*' || op == '/')
	{
		RegisterToken();
		ExponentEvaluation(temp);
		switch (op)
		{
		case '*':
			result = result * temp;
			break;
		case '/':
			result = result / temp;
			break;
		}
	}
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
	register char op;
	op = 0;
	if ((tok_type == DELIMITER) && *token == '+' || *token == '-')
	{
		op = *token;
		RegisterToken();
	}
	EvaluateFunction(result);
	if (op == '-')
		result = -result;
}


void parser::EvaluateFunction(double& result) {
	bool isfunc = (tok_type == FUNCTION);
	string temp_token = token;

	if (isfunc) {
		RegisterToken();
	}

	if (*token == '(') {
		RegisterToken();
		EvaluateAddition(result);

		if (isfunc) {
			static const unordered_map<string, function<double(double)>> func_map = {
				{"SIN",   [](double x) { return Algorithms::SinExpansion(x, 5); }},
				{"COS",   [](double x) { return Algorithms::CosExpansion(x, 5); }},
				{"TAN",   [](double x) { return Algorithms::SinExpansion(x, 5) / Algorithms::CosExpansion(x, 5); }},
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

			auto it = func_map.find(temp_token);
			if (it != func_map.end()) {
				result = it->second(result);
			}
		}
		RegisterToken();
	}
	else {
		switch (tok_type) {
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
	register char* temp;
	tok_type = 0;
	temp = token;
	*temp = '\0';
	if (!*exp_ptr)  // at end of expression
		return;
	while (isspace(*exp_ptr))  // skip over white space
		++exp_ptr;

	if (strchr("+-*/%^=()", *exp_ptr))
	{
		tok_type = DELIMITER;
		*temp++ = *exp_ptr++;  // advance to next char
	}

	else if (isalpha(*exp_ptr))
	{
		while (!strchr(" +-/*%^=()\t\r", *exp_ptr) && (*exp_ptr))
			*temp++ = toupper(*exp_ptr++);
		while (isspace(*exp_ptr))  // skip over white space
			++exp_ptr;
		tok_type = (*exp_ptr == '(') ? FUNCTION : VARIABLE;
	}

	else if (isdigit(*exp_ptr) || *exp_ptr == '.')
	{
		while (!strchr(" +-/*%^=()\t\r", *exp_ptr) && (*exp_ptr))
			*temp++ = toupper(*exp_ptr++);
		tok_type = NUMBER;
	}

	*temp = '\0';
}

