//g++ -o sofuni sofuni.cpp -lstdc++ -std=c++11
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <utility>
#include <fstream>
#include <readline/readline.h>
#include <readline/history.h>
//compile with g++ -o sfrepl sofunrepl.cpp -lreadline -lstdc++ -std=c++11

//-------------------------------------------------noise
static char *line_read = (char *)NULL; //warum, C?
char * rl_gets () { //basic functionality of the readline library, copied from it's website
  if (line_read) {
      free (line_read);
      line_read = (char *)NULL;
    }
  line_read = readline("sofun>: ");
  if (line_read && *line_read) add_history(line_read);
  return (line_read);
}

using namespace std;

//----------------------------------------------------defs
typedef vector<string> stack; //Ein Stack ist ein vector von Strings. Ein echter Stack wäre für die REPL-Funktionen unpraktisch gewesen ;)
typedef pair<stack, stack> fun; //eine funktion ist ein Paar aus ihren Argumenten und dem Stack


//----------------------------------------------------global vars
map<string, fun> funs;  //hier werden die Funktionen unter ihren Namen abgespeichert
bool debug_mode = 0; //jeden Schritt der REPL printen
stack empty_stack {}; //wird bei error zurückgegeben

//----------------------------------------------------misc

bool is_numeric(string inp_string, bool with_digits = false) {
	if (inp_string.size() < 1) return true;
	char head = inp_string.back(); inp_string.pop_back();
	if (with_digits && head == '-' && inp_string.size() < 1) return true;
	else if (isdigit(head)) return is_numeric(inp_string, true);
	return false;
}

bool is_stack(string inp_string) {return inp_string[0]=='(' && inp_string.size()>1;}

stack split(string inp_string) { //funktion wie das python split()
	if (inp_string.size()<1) {stack ret_vector {}; return ret_vector;} //rekursionsabbruchbedingung
	char head = inp_string.back(); inp_string.pop_back(); //pop
	string identifier = "";
	while (head!=' ') { //schleife durch den String um den identifier bis zum leerzeichen zu erhalten
		if (inp_string.size()<1) {identifier.insert(identifier.begin(), head); break;}
		identifier.insert(identifier.begin(), head);
		head = inp_string.back(); inp_string.pop_back(); //pop
	}
	stack ret_vector = split(inp_string); //Rekursion
	if (identifier!="") ret_vector.push_back(identifier); //wenn es tatsächlich ein identifier ist, dann hänge ihn ran
	return ret_vector;
}

string desplit(stack inp_vector) { //pythons join()
	if (inp_vector.size()<1) return "";
	string identifier=inp_vector.back(); inp_vector.pop_back();
	return desplit(inp_vector)+identifier+" "; //rekursion
}

string pop_stack(string stack_string) {return split(stack_string)[1];}

string popped_stack(string stack_string) {
	stack extracted_stack = split(stack_string);
	extracted_stack.erase(extracted_stack.begin()+1);
	return desplit(extracted_stack);
}

string is_empty_stack(string stack_string) {
	if (split(stack_string).size()<=2) return "1";
	else return "0";
}

string push_stack(string stack_string, string to_push) {
	stack extracted_stack = split(stack_string);
	extracted_stack.insert(extracted_stack.begin()+1, to_push);
	return desplit(extracted_stack);
}

//----------------------------------------------------built-ins
stack bi_math(stack line, long stack_pointer, string operation) { //built-in math functions
	if (stack_pointer<2) {
		cout << "not enough arguments for function " << operation << endl;
		return empty_stack;
	}
	string arg1 = line[stack_pointer-2]; string arg2 = line[stack_pointer-1];
	if (!is_numeric(arg1) || !is_numeric(arg2)) {
		cout << "wrong arguments for function " << arg1 << " " << arg2 << " " << stack_pointer << endl;
		return empty_stack;
	}
	long result = 0;
	if (operation == "+") result = stol(arg1) + stol(arg2);
	else if (operation == "-") result = stol(arg1) - stol(arg2);
	else if (operation == "*") result = stol(arg1) * stol(arg2);
	else if (operation == "/") result = stol(arg1) / stol(arg2);
	else if (operation == "%") result = stol(arg1) % stol(arg2);
	else cout << "something went awfully wrong" << endl;
	line.erase(line.begin()+stack_pointer-2, line.begin()+stack_pointer+1);
	line.insert(line.begin()+stack_pointer-2, to_string(result));
	return line;
}

stack bi_condition(stack line, long stack_pointer, string operation) { //built-in compare functions
	int arg_num = 2;
	if (operation=="~") arg_num = 1;
	if (stack_pointer<arg_num) {
		cout << "not enough arguments for function " << operation << endl;
		return empty_stack;
	}
	stack args;
	for (int i = arg_num; i>=1; i--) {
		args.push_back(line[stack_pointer-i]);
		if (!is_numeric(line[stack_pointer-i])) {
			cout << "wrong arguments for function " << desplit(args) << stack_pointer << endl;
			return empty_stack;
		}
	}	
	long result = 0;
	if (operation == "<") result = stol(args[0]) < stol(args[1]);
	else if (operation == "=") result = stol(args[0]) == stol(args[1]);
	else if (operation == ">") result = stol(args[0]) > stol(args[1]);
	else if (operation == "|") result = stol(args[0]) || stol(args[1]);
	else if (operation == "&") result = stol(args[0]) && stol(args[1]);
	else if (operation == "~") result = !stol(args[0]);
	else cout << "something went awfully wrong" << endl;
	line.erase(line.begin()+stack_pointer-arg_num, line.begin()+stack_pointer+1);
	line.insert(line.begin()+stack_pointer-arg_num, to_string(result));
	return line;
}

stack bi_stack(stack line, long stack_pointer, string operation) { //built-in stack functions
	int arg_num = 1;
	if (operation=="push") arg_num = 2;
	if (stack_pointer<arg_num) {
		cout << "not enough arguments for function " << operation << endl;
		return empty_stack;
	}
	stack args;
	for (int i = arg_num; i>=1; i--) {
		if ((i == arg_num && !is_stack(line[stack_pointer-i])) || (i != arg_num && !is_numeric(line[stack_pointer-i]))) {
			cout << "wrong arguments for function " << desplit(args) << stack_pointer << endl;
			return empty_stack;
		} else args.push_back(line[stack_pointer-i]);
	}	
	string result;
	if (operation == "pop") result = pop_stack(args[0]);
	else if (operation == "popped") result = popped_stack(args[0]);
	else if (operation == "is_empty") result = is_empty_stack(args[0]);
	else if (operation == "push") result = push_stack(args[0], args[1]);
	else cout << "something went awfully wrong" << endl;
	line.erase(line.begin()+stack_pointer-arg_num, line.begin()+stack_pointer+1);
	line.insert(line.begin()+stack_pointer-arg_num, result);
	return line;
}

//-------------------------------------------------------core
stack parse_function(stack, long, string); //macht Funktion zu evaluierbarem Stack

stack evaluate(stack line, long stack_pointer = 0) { //evaluiert den Stack (führt Funktionen aus)
	if (debug_mode) cout << "evaluating " << desplit(line) << endl;
	if (stack_pointer >= line.size()) return line; 
	string token = line[stack_pointer];
	if (is_numeric(token)) {
		if (debug_mode) cout << "numeric " << token << endl;
		stack_pointer += 1;
	} else if (token[0] == '(') { //wenn im main stack ein weiterer Stack liegt
		if (token.size() == 1) { //wenns nur die Klammer ist
			stack::iterator stack_end = find(line.begin(), line.end(), ")"); //wo ist das Ende
			if (stack_end != line.end()) { //wenns eine schließende Klammer gibt, tausche den Abschnitt mit einem String vom Abschnitt
				stack inner_stack (line.begin()+stack_pointer, stack_end+1);
				string inner_stack_string = desplit(inner_stack);
				line.erase(line.begin()+stack_pointer, stack_end+1);
				line.insert(line.begin()+stack_pointer, inner_stack_string);
			} else {
				cout << "opening brace without closing in " << desplit(line);
				return empty_stack;
			}
		}
		stack_pointer += 1;
	} else if (funs.find(token) != funs.end()) { //wenn der token eine bekannte Funktion ist
		if (debug_mode) cout << "known function " << token << endl;
		line = parse_function(line, stack_pointer, token);
		stack_pointer = 0;
	} else { //sonst built-in-funktion oder unexpected identifier
		if (debug_mode) cout << "built-in or unknown " << token << endl;
		if (token == "+" || token == "-" || token == "*" || token == "/" || token == "%") 
			line = bi_math(line, stack_pointer, token);
		else if (token == "<" || token == "=" || token == ">" || token == "|" || token == "&" || token == "~") 
			line = bi_condition(line, stack_pointer, token);
		else if (token == "pop" || token == "push" || token == "is_empty" || token == "popped")
			line = bi_stack(line, stack_pointer, token);
		else {
			cout << "unexpected token " << token << endl;
			return empty_stack;
		}
		stack_pointer = 0;
	}
	return evaluate(line, stack_pointer);
}

bool parse_condition(stack condition) {
	if (debug_mode) cout << "parsing condition " << desplit(condition) << endl;
	stack result = evaluate(condition);
	if (result.size() > 1) {
		cout << "Warning: Condition evaluates to a stack with multiple elements: " << desplit(condition) << endl;
		return true;
	} else if (result.size()<1) {
		cout << "Warning: Condition evaluates to a stack with no elements: " << desplit(condition) << endl;
		return false;
	} else {
		if (!is_numeric(result[0])) {
			cout << "Warning: Condition doesn't evaluate to a bool value" << desplit(condition) << endl;
		} else if (stol(result[0]) == 0) return false;
		else return true;
	}
	return false;
}

stack parse_function(stack line, long stack_pointer, string fun_name) {
	if (debug_mode) cout << "parsing function " << desplit(line) << endl;
	fun called_fun = funs[fun_name];
	stack arg_names = called_fun.first;
	if (debug_mode) cout << desplit(arg_names) << "args" << endl;
	stack fun_stack = called_fun.second;
	long args_num = arg_names.size();
	if (stack_pointer < args_num) {
		cout << "not enough arguments for function " << fun_name << endl;
		return empty_stack;
	}
	map<string, string> args; //map für Namen und tatsächliche Werte der Argumente
	for (long i = 1; i <= args_num; i++) { 
		args[arg_names[args_num-i]] = line[stack_pointer - i]; //das Argument sei das n-te Element vorm Funktionsnamen
	}
	for (long i = 0; i < fun_stack.size(); i++) { //dann iteriere über den tail
			if (args.find(fun_stack[i]) != args.end()) fun_stack[i]=args[fun_stack[i]]; //und ersetze die Argumente mit ihren Werten
	}
	if (debug_mode) cout << "... " << desplit(fun_stack) << endl;
	stack::iterator question_mark = find(fun_stack.begin(), fun_stack.end(), "?"); //ist es ein verzweigter Stack?
	if (question_mark != fun_stack.end()) { //wenn es ein Fragezeichenzeichen gibt, es also ein verzweigter Stack ist
		vector<stack> branch;
		long last_found = 0;
		for (long i = 0; i <= fun_stack.size(); i++) {
			if ( i == fun_stack.size() || fun_stack[i] == "?") {
				stack::iterator fun_sign = find(fun_stack.begin()+last_found, fun_stack.begin()+i, ":"); //wo ist in der Branch das Funktionszeichen?
				stack branch_head (fun_stack.begin()+last_found+(last_found!=0), fun_sign);
				stack branch_tail;
				if (fun_sign != fun_stack.end()) branch_tail.insert(branch_tail.begin(), fun_sign+1, fun_stack.begin()+i);
				else branch_tail.insert(branch_tail.begin(), fun_stack.begin()+last_found+1, fun_stack.end());
				if (debug_mode) cout << fun_stack.size() << " if " << desplit(branch_head) << "then " << desplit(branch_tail) << endl;
				if (i == fun_stack.size() || parse_condition(branch_head)) { //wenn die branch-condition zutriffz, für den branch_stack aus
					fun_stack = branch_tail;
					break;
				}
				last_found = i;
			}
		}
		
	}
	line.erase(line.begin()+stack_pointer-args_num, line.begin()+stack_pointer+1);
	line.insert(line.begin()+stack_pointer-args_num, fun_stack.begin(), fun_stack.end());
	if (debug_mode) cout << "returning " << desplit(line) << endl;
	return line;
}

stack parse(stack line) { //syntaktische Analyse (Funktionsdeklaration oder evaluierbarer Stack?)
	stack::iterator fun_sign = find(line.begin(), line.end(), ":"); //wo ist das Funktionszeichen
	stack::iterator branched_fun_sign = find(line.begin(), line.end(), "?"); //oder das verzweigte Funktionszeichen
	if (fun_sign == line.end() && branched_fun_sign == line.end()) line = evaluate(line); //wenn es eines gibt
	else {
		if (branched_fun_sign != line.end()) fun_sign = branched_fun_sign;
		stack fun_head (line.begin(), fun_sign); //Dann ist alles bis zum Funktionszeichen der Funktionskopf
		stack fun_tail (fun_sign+1, line.end()); //Der Rest der Schwanz
		string fun_name = fun_head.back(); fun_head.pop_back(); //der Name ist der letzte token vorm Gleichzeichen
		stack fun_args = fun_head; //Der Rest sind Argumente
		fun new_fun (fun_args, fun_tail); //Das kommt in die map -> Argumente und der Funktionsschwanz
		funs[fun_name] = new_fun; //Füge die Funktion zu den derzeitigen Funktionen hinzu
		if (debug_mode) cout << fun_name << " head: " << desplit(fun_head) << "tail: " << desplit(fun_tail) << endl;
		return empty_stack;
	}
	return line;
}

void eval_file(string name) {
	ifstream fun_file (name);
	string line;
	if (fun_file.is_open()) {
    while (getline(fun_file,line)) {
		if (line[0]!='#') parse(split(line)); //wenns kein kommentar ist, führs aus
    }
    fun_file.close();
  } else cout << "Unable to open file " << name << endl; 
}

bool parse_command(string line) {
	if (line[1] == 'q') return true;
	if (line[1] == 'd') debug_mode = !debug_mode;
	if (line[1] == 'l') eval_file(split(line)[1]);
	return false;
}

//--------------------------------------------------------main
int main(int argc,  char** argv) {
	stack inp_stack; stack outp_stack;
	for (;;) { //Endlosschleife 
		rl_gets();
		string inp_line = line_read;
		if (inp_line.front() == ':') { //wenn es ein repl-befehl ist
			if (parse_command(inp_line)) break;
		} else { //wenn es ein Ausdruck der Sprache ist
			inp_stack = split(inp_line);
			outp_stack = parse(inp_stack);
			cout << desplit(outp_stack) << "\n";
		}
	}
	free (line_read); //ich hasse C
	line_read = (char *)NULL; //ich hasse es wirklich
}

