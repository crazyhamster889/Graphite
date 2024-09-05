
#include "CMathParser.h"

#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cmath>
using namespace std;

// Parser constructor.
parser::parser()
{
	int i;
	exp_ptr = NULL;
	for (i = 0; i < NUMVARS; i++)
		vars[i] = 0.0;
	errormsg[0] = '\0';
}
// Parser entry point.
double parser::eval_exp(const std::string& exp)
{
	char EQUATION[1024];
	errormsg[0] = '\0';
	double result;
	strncpy_s(EQUATION, exp.c_str(), sizeof(EQUATION) - 1);
	EQUATION[sizeof(EQUATION) - 1] = '\0'; // Ensure null-termination
	exp_ptr = EQUATION;
	get_token();
	if (!*token)
	{
		strncpy_s(errormsg, "No Expression Present", 30); // no expression present
		return (double)0;
	}
	eval_exp1(result);
	if (*token) // last token must be null
		strncpy_s(errormsg, "Syntax Error", 20);
	return result;
}
// Process an assignment.
void parser::eval_exp1(double& result)
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
		get_token();
		if (*token != '=')
		{
			exp_ptr = t_ptr; // return current token
			strncpy_s(token, temp_token, 100); // restore old token
			tok_type = VARIABLE;
		}
		else {
			get_token(); // get next part of exp
			eval_exp2(result);
			vars[slot] = result;
			return;
		}
	}
	eval_exp2(result);
}
// Add or subtract two terms.
void parser::eval_exp2(double& result)
{
	register char op;
	double temp;
	eval_exp3(result);
	while ((op = *token) == '+' || op == '-')
	{
		get_token();
		eval_exp3(temp);
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
void parser::eval_exp3(double& result)
{
	register char op;
	double temp;
	eval_exp4(result);
	while ((op = *token) == '*' || op == '/')
	{
		get_token();
		eval_exp4(temp);
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
void parser::eval_exp4(double& result)
{
	double temp;
	eval_exp5(result);
	while (*token == '^')
	{
		get_token();
		eval_exp5(temp);
		result = pow(result, temp);
	}
}
// Evaluate a unary + or -.
void parser::eval_exp5(double& result)
{
	register char op;
	op = 0;
	if ((tok_type == DELIMITER) && *token == '+' || *token == '-')
	{
		op = *token;
		get_token();
	}
	eval_exp6(result);
	if (op == '-')
		result = -result;
}
// Process a function, a parenthesized expression, a value or a variable
void parser::eval_exp6(double& result)
{
	bool isfunc = (tok_type == FUNCTION);
	char temp_token[80];
	if (isfunc)
	{
		strncpy_s(temp_token, token, 100);
		get_token();
	}
	if ((*token == '('))
	{
		get_token();
		eval_exp2(result);
		if (*token != ')')
			strncpy_s(errormsg, "Unbalanced Parentheses", 30);
		if (isfunc)
		{
			if (!strcmp(temp_token, "SIN"))
				result = sinf(result);
			else if (!strcmp(temp_token, "COS"))
				result = cosf(result);
			else if (!strcmp(temp_token, "TAN"))
				result = tanf(result);
			else if (!strcmp(temp_token, "ASIN"))
				result = asin(result);
			else if (!strcmp(temp_token, "ACOS"))
				result = acosf(result);
			else if (!strcmp(temp_token, "ATAN"))
				result = atanf(result);
			else if (!strcmp(temp_token, "SINH"))
				result = sinh(result);
			else if (!strcmp(temp_token, "COSH"))
				result = cosh(result);
			else if (!strcmp(temp_token, "TANH"))
				result = tanh(result);
			else if (!strcmp(temp_token, "ASINH"))
				result = asinh(result);
			else if (!strcmp(temp_token, "ACOSH"))
				result = acosh(result);
			else if (!strcmp(temp_token, "ATANH"))
				result = atanh(result);
			else if (!strcmp(temp_token, "SIGN"))
				result = (result > 0) ? 1 : ((result < 0) ? -1 : 0);
			else if (!strcmp(temp_token, "LN"))
				result = logf(result);
			else if (!strcmp(temp_token, "LOG"))
				result = log10(result);
			else if (!strcmp(temp_token, "EXP"))
				result = exp(result);
			else if (!strcmp(temp_token, "SQRT"))
				result = sqrt(result);
			else if (!strcmp(temp_token, "ABS"))
				result = abs(result);
			else if (!strcmp(temp_token, "SQR"))
				result = result * result;
			else if (!strcmp(temp_token, "ROUND"))
				result = round(result);
			else if (!strcmp(temp_token, "INT"))
				result = floor(result);
			else
				strncpy_s(errormsg, "Unknown Function", 20);
		}
		get_token();
	}
	else
		switch (tok_type)
		{
		case VARIABLE:
			result = vars[*token - 'A'];
			get_token();
			return;
		case NUMBER:
			result = atof(token);
			get_token();
			return;
		default:
			strncpy_s(errormsg, "Syntax Error", 20);
		}
}
// Obtain the next token.
void parser::get_token()
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
	if ((tok_type == VARIABLE) && (token[1]))
		strncpy_s(errormsg, "Only first letter of variables is considered", 50);
}

