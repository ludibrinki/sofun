//g++ -o sofuni sofuni.cpp -lstdc++ -std=c++11
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <utility>
using namespace std;


//----------------------------------------------------defs
typedef vector<string> stack; //Ein Stack ist ein vector von Strings. Ein echter Stack wäre für die REPL-Funktionen unpraktisch gewesen ;)
typedef pair<stack, stack> fun; //eine funktion ist ein Paar aus ihren Argumenten und dem Stack


//----------------------------------------------------global vars
map<string, fun> funs;  //hier werden die Funktionen unter ihren Namen abgespeichert
bool debug_mode = 0; //jeden Schritt der REPL printen
stack empty_stack {}; //wird bei error zurückgegeben

//----------------------------------------------------misc
bool is_numeric(string inp_string) {
	if (inp_string.size() < 1) return true;
	char head = inp_string.back(); inp_string.pop_back();
	if (isdigit(head)) return is_numeric(inp_string);
	else return false;
}

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


//----------------------------------------------------built-ins
stack bi_math(stack line, int stack_pointer, string operation) {
	if (stack_pointer<2) {
		cout << "not enough arguments for function " << operation << endl;
		return empty_stack;
	}
	string arg1 = line[stack_pointer-1]; string arg2 = line[stack_pointer-2];
	if (!is_numeric(arg1) || !is_numeric(arg2)) {
		cout << "wrong arguments for function " << arg1 << " " << arg2 << " " << stack_pointer << endl;
		return empty_stack;
	}
	int result = 0;
	if (operation == "+") result = stoi(arg1) + stoi(arg2);
	else if (operation == "-") result = stoi(arg1) - stoi(arg2);
	else if (operation == "*") result = stoi(arg1) * stoi(arg2);
	else if (operation == "/") result = stoi(arg1) / stoi(arg2);
	else if (operation == "%") result = stoi(arg1) % stoi(arg2);
	else cout << "something went awfully wrong" << endl;
	line.erase(line.begin()+stack_pointer-2, line.begin()+stack_pointer+1);
	line.insert(line.begin()+stack_pointer-2, to_string(result));
	return line;
}


//-------------------------------------------------------core
stack parse_function(stack, int, string); //macht Funktion zu evaluierbarem Stack

bool parse_condition(stack);

stack evaluate(stack line, int stack_pointer = 0) { //evaluiert den Stack (führt Funktionen aus)
	if (debug_mode) cout << "evaluating " << desplit(line) << endl;
	if (stack_pointer >= line.size()) return line; 
	string token = line[stack_pointer];
	if (is_numeric(token)) {
		if (debug_mode) cout << "numeric " << token << endl;
		stack_pointer += 1;
	} else if (funs.find(token) != funs.end()) { //wenn der token eine bekannte Funktion ist
		if (debug_mode) cout << "known function " << token << endl;
		line = parse_function(line, stack_pointer, token);
		stack_pointer = 0;
	} else { //sonst built-in-funktion oder unexpected identifier
		if (debug_mode) cout << "built-in or unknown " << token << endl;
		if (token == "+" || token == "-" || token == "*" || token == "/" || token == "%") 
			line = bi_math(line, stack_pointer, token);
		else {
			cout << "unexpected token " << token << endl;
			return empty_stack;
		}
		stack_pointer = 0;
	}
	return evaluate(line, stack_pointer);
}

stack parse_function(stack line, int stack_pointer, string fun_name) {
	if (debug_mode) cout << "parsing function " << desplit(line) << endl;
	fun called_fun = funs[fun_name];
	stack arg_names = called_fun.first;
	if (debug_mode) cout << desplit(arg_names) << "args" << endl;
	stack fun_stack = called_fun.second;
	int args_num = arg_names.size();
	if (stack_pointer < args_num) {
		cout << "not enough arguments for function " << fun_name << endl;
		return empty_stack;
	}
	map<string, string> args; //map für Namen und tatsächliche Werte der Argumente
	for (int i = 1; i <= args_num; i++) { 
		args[arg_names[args_num-i]] = line[stack_pointer - i]; //das Argument sei das n-te Element vorm Funktionsnamen
	}
	for (int i = 0; i < fun_stack.size(); i++) { //dann iteriere über den tail
			if (args.find(fun_stack[i]) != args.end()) fun_stack[i]=args[fun_stack[i]]; //und ersetze die Argumente mit ihren Werten
	}
	if (debug_mode) cout << desplit(fun_stack) << endl;
	stack::iterator exclamation_mark = find(fun_stack.begin(), fun_stack.end(), "!"); //ist es ein verzweigter Stack, wo ist das otherwise?
	if (exclamation_mark != fun_stack.end()) { //wenn es ein Ausrufezeichen gibt, es also ein verzweigter Stack ist
		vector<stack> branch;
		int last_found = 0;
		for (int i = 0; i <= fun_stack.size(); i++) {
			if ( i == fun_stack.size() || fun_stack[i] == "?" || fun_stack[i] == "!") {
				stack::iterator fun_sign = find(fun_stack.begin()+last_found, fun_stack.begin()+i, ":"); //wo ist in der Branch das Funktionszeichen?
				stack branch_head (fun_stack.begin()+last_found, fun_sign);
				stack branch_tail;
				if (fun_sign != fun_stack.end()) branch_tail.insert(branch_tail.begin(), fun_sign+1, fun_stack.begin()+i);
				else branch_tail.insert(branch_tail.begin(), fun_stack.begin()+last_found+1, fun_stack.end());
				if (i == fun_stack.size() || parse_condition(branch_head)) { //wenn die branch-condition zutriffz, für den branch_stack aus
					fun_stack = evaluate(branch_tail);
				}
				last_found = i;
			}
		}
		
	} else { //wenn es kein verzweigter Stack ist
		fun_stack = evaluate(fun_stack);
	}
	line.erase(line.begin()+stack_pointer-args_num, line.begin()+stack_pointer+1);
	line.insert(line.begin()+stack_pointer-args_num, fun_stack.begin(), fun_stack.end());
	return line;
}

bool parse_condition(stack condition) {
	if (debug_mode) cout << "parsing condition " << desplit(condition) << endl;
	stack::iterator lt_sign = find(condition.begin(), condition.end(), "<");
	stack::iterator gt_sign = find(condition.begin(), condition.end(), ">");
	stack::iterator eq_sign = find(condition.begin(), condition.end(), "=");
	stack left_stack; stack right_stack;
	if (lt_sign != condition.end()) {
		left_stack.insert(left_stack.begin(), condition.begin(), lt_sign);
		right_stack.insert(right_stack.begin(), lt_sign+1, condition.end());
		left_stack = evaluate(left_stack); right_stack = evaluate(right_stack);
		if (debug_mode) cout << desplit(left_stack) << desplit(right_stack) << "l r" << endl;
		if (left_stack.size() != 1 || right_stack.size() != 1) {
			cout << "wrong stack size in condition after evaluation " << desplit(left_stack) << "<" << desplit(right_stack);
			return false;
		}
		if (!is_numeric(left_stack[0])) {cout << "not numeric in condition " << left_stack[0]; return false;}
		if (!is_numeric(right_stack[0])) {cout << "not numeric in condition " << right_stack[0]; return false;}
		return (stoi(left_stack[0]) < stoi(right_stack[0]));
	} 
	else if (gt_sign != condition.end()) {
		left_stack.insert(left_stack.begin(), condition.begin(), gt_sign);
		right_stack.insert(right_stack.begin(), gt_sign+1, condition.end());
		left_stack = evaluate(left_stack); right_stack = evaluate(right_stack);
		if (left_stack.size() != 1 || right_stack.size() != 1) {
			cout << "wrong stack size in condition after evaluation " << desplit(left_stack) << ">" << desplit(right_stack);
			return false;
		}
		if (!is_numeric(left_stack[0])) {cout << "not numeric in condition " << left_stack[0]; return false;}
		if (!is_numeric(right_stack[0])) {cout << "not numeric in condition " << right_stack[0]; return false;}
		return (stoi(left_stack[0]) > stoi(right_stack[0]));
	} 
	else if (eq_sign != condition.end()) {
		left_stack.insert(left_stack.begin(), condition.begin(), eq_sign);
		right_stack.insert(right_stack.begin(), eq_sign+1, condition.end());
		left_stack = evaluate(left_stack); right_stack = evaluate(right_stack);
		if (left_stack.size() != 1 || right_stack.size() != 1) {
			cout << "wrong stack size in condition after evaluation " << desplit(left_stack) << "=" << desplit(right_stack);
			return false;
		}
		return (left_stack[0] == right_stack[0]);
	} 
	else {
		cout << "condition without comparison " << desplit(condition) << endl;
		return false;
	}
	return false;
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
		if (debug_mode) cout << fun_name << " " << desplit(fun_head) << "& " << desplit(fun_tail) << endl;
	}
	return line;
}

bool parse_command(string line) {
	if (line == ":quit" || line == ":q") return true;
	if (line == ":debug" || line == ":d") debug_mode = !debug_mode;
	return false;
}


//--------------------------------------------------------main
int main(int argc,  char** argv) {
	string inp_line;
	stack inp_stack; stack outp_stack;
	for (;;) { //Endlosschleife
		cout << "sofunrepl0.1 >: ";
		getline(cin, inp_line); 
		
		if (inp_line.front() == ':') { //wenn es ein repl-befehl ist
			if (parse_command(inp_line)) break;
		} else { //wenn es ein Ausdruck der Sprache ist
			inp_stack = split(inp_line);
			outp_stack = parse(inp_stack);
			cout << desplit(outp_stack) << "\n";
		}
	}
}

