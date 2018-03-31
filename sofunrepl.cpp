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
//compile with performance analysis with -pg and -no-pie and gprof sfrepl | gprof2dot -s -w | dot -Tpng -o output.png

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
typedef vector<char*> stack; //Ein Stack ist ein vector von Strings. Ein echter Stack wäre für die REPL-Funktionen unpraktisch gewesen ;)
typedef pair<stack, stack> fun; //eine funktion ist ein Paar aus ihren Argumenten und dem Stack
typedef pair<long, stack> line_pair;


//----------------------------------------------------global vars
map<char*, fun> funs;  //hier werden die Funktionen unter ihren Namen abgespeichert
bool debug_mode = 0; //jeden Schritt der REPL printen
stack empty_stack {}; //wird bei error zurückgegeben
line_pair empty_pair (0,empty_stack); //wird bei error zurückgegeben
stack main_args {};

//----------------------------------------------------misc

map<char*, char*>::iterator my_find(map<char*, char*> &haystack, char *needle) {
	map<char*, char*>::iterator it = haystack.begin();
	for (it = haystack.begin(); it != haystack.end(); it++){
		if (0 == strcmp(it->first, needle))
			break;
	}
	return it;
}


map<char*, fun>::iterator my_find(map<char*, fun> &haystack, char *needle) {
	map<char*, fun>::iterator it = haystack.begin();
	for (it = haystack.begin(); it != haystack.end(); it++){
		if (0 == strcmp(it->first, needle))
			break;
	}
	return it;
}

stack::iterator my_find(stack &haystack, char *needle, int start_offset = 0, int until_offset = -1) {
	stack::iterator it = haystack.begin() + start_offset;
	if (until_offset >= 0) {
		for (it = haystack.begin() + start_offset; it != haystack.begin() + until_offset; it++){
			if (0 == strcmp(*it, needle))
				break;
		}
	} else {
		for (it = haystack.begin() + start_offset; it != haystack.end(); it++){
			if (0 == strcmp(*it, needle))
				break;
		}
	}
	return it;
}

bool is_numeric(char *inp_string) {
	if (strlen(inp_string) < 1) return true;
	for (unsigned int i = 0; i < strlen(inp_string); i++) {
		char head = inp_string[i];
		if (!isdigit(head) && !(i==0 && head == '-' && strlen(inp_string)>1)) return false;
	}
	return true;
}

bool is_stack(char *inp_string) {return inp_string[0]=='(' && strlen(inp_string)>1;}

stack split(char *inp_string) { //funktion wie das python split()
	stack ret_vector;
	unsigned long length_of_string = strlen(inp_string);
	if (length_of_string<1) return ret_vector; //rekursionsabbruchbedingung
	//ret_vector.reserve(sizeof(inp_string)/4);
	long last_space = -1;
	for (unsigned long i = 0; i <= length_of_string; i++) {
		char x = inp_string[i];
		if (x == ' ' || i == length_of_string) {
			if (i-last_space > 1) {
				char *identifier = (char*) malloc(i - last_space - 1);
				strncpy(identifier, inp_string + last_space + 1, i - last_space - 1);
				identifier[i - last_space - 1 ] = '\0';
				ret_vector.push_back(identifier);
			}
			last_space = i;
		}
	}
	return ret_vector;
}

char *desplit(stack inp_vector, long size = 0) { //pythons join()

	char *ret;
	for  (unsigned long i = 0; i < inp_vector.size(); i++) {
		size += strlen(inp_vector[i]) + 1;
	}
	ret = (char*) malloc(size + 1);
	long pos = 0;
	for (unsigned long i = 0; i < inp_vector.size(); i++) {
		strcpy(pos + ret, inp_vector[i]);
		pos += 1 + strlen(inp_vector[i]);
		ret[pos-1] = ' ';
		//ret += inp_vector[i]+" ";
	}
	ret[size] = '\0';
	return ret;
}


long find_closing_bracket(stack line, unsigned long stack_pointer, stack brackets = empty_stack) {
	if (stack_pointer >= line.size()) return line.size();
	if (0 == strcmp(line[stack_pointer], "(")) brackets.push_back(line[stack_pointer]);
	else if (0 == strcmp(line[stack_pointer], ")")) brackets.pop_back();
	if (brackets.size() == 0) return stack_pointer;
	return find_closing_bracket(line, stack_pointer+1, brackets);
}

stack make_stack_string(stack line, long stack_pointer) {
	if (0 == strcmp(line[stack_pointer], "()")) cout << "no space between brackets of empty stack " << desplit(line) << line[stack_pointer];
	else if (strlen(line[stack_pointer]) == 1) { //wenns nur die Klammer ist
		unsigned long stack_end = find_closing_bracket(line, stack_pointer); //wo ist das Ende
		if (stack_end < line.size()) {
			stack inner_stack (line.begin()+stack_pointer, line.begin()+stack_end+1);
			char *inner_stack_string = desplit(inner_stack);
			inner_stack_string[strlen(inner_stack_string)-1] = '\0';
			line.erase(line.begin()+stack_pointer, line.begin()+stack_end+1);
			line.insert(line.begin()+stack_pointer, inner_stack_string);
		} else {
			cout << "opening bracket without closing one " << desplit(line);
			return empty_stack;
		}
		return line;
	} 
	return line;
}

char *is_empty_stack(char *stack_string) {
	if (split(stack_string).size() <= 2) return "1";
	else return "0";
}

char *pop_stack(char *stack_string) {
	stack extracted_stack = split(stack_string);
	if (extracted_stack.size() <= 2) return "";
	for (unsigned long i = 1; i < extracted_stack.size(); i++) {
		if (extracted_stack[i][0] == '(') {
			extracted_stack = make_stack_string(extracted_stack, i);
		}
	}
	extracted_stack.pop_back(); //Die Klammer
	return extracted_stack.back();
}

char *popped_stack(char *stack_string) {
	stack extracted_stack = split(stack_string);
	if (extracted_stack.size() <= 2) {
		cout << "can't pop empty stack ";
		return "( )";
	}
	for (unsigned long i = 1; i < extracted_stack.size(); i++) {
		if (extracted_stack[i][0] == '(') {
			extracted_stack = make_stack_string(extracted_stack, i);
		}
	}
	extracted_stack.pop_back(); //die Klammer
	extracted_stack.pop_back();
	extracted_stack.push_back( ")");
	char *ret = desplit(extracted_stack);
	return ret;
}

char *push_stack(char *stack_string, char *to_push) {
	stack extracted_stack = split(stack_string);
	for (unsigned long i = 1; i < extracted_stack.size(); i++) {
		if (extracted_stack[i][0] == '(') {
			extracted_stack = make_stack_string(extracted_stack, i);
		}
	}
	extracted_stack.pop_back(); //Die Klammer
	extracted_stack.push_back(to_push);
	extracted_stack.push_back( ")");

	char *ret = desplit(extracted_stack);
	return ret;
}

//----------------------------------------------------built-ins
line_pair bi_math(stack line, long stack_pointer, char *operation) { //built-in math functions
	if (stack_pointer<2) {
		cout << "not enough arguments for function " << operation << endl;
		return empty_pair;
	}
	char *arg1 = line[stack_pointer-2]; char *arg2 = line[stack_pointer-1];
	if (!is_numeric(arg1) || !is_numeric(arg2)) {
		cout << "wrong arguments for function " << operation << arg1 << " " << arg2 << " " << endl;
		return empty_pair;
	}
	char *result;
	int size;
	long arg1_int = strtol(arg1, NULL, 10);
	long arg2_int = strtol(arg2, NULL, 10);

	if (0 == strcmp(operation, "+")) {
		size = snprintf(NULL, 0, "%ld", arg1_int + arg2_int);
		result = (char*) malloc(size + 1);
		sprintf(result, "%ld", arg1_int + arg2_int);
	} else if (0 == strcmp(operation, "-")) {
		size = snprintf(NULL, 0, "%ld", arg1_int - arg2_int);
		result = (char*) malloc(size + 1);
		sprintf(result, "%ld", arg1_int - arg2_int);
	} else if (0 == strcmp(operation, "*")) {
		size = snprintf(NULL, 0, "%ld", arg1_int * arg2_int);
		result = (char*) malloc(size + 1);
		sprintf(result, "%ld", arg1_int * arg2_int);
	} else if (0 == strcmp(operation, "/")) {
		size = snprintf(NULL, 0, "%ld", arg1_int / arg2_int);
		result = (char*) malloc(size + 1);
		sprintf(result, "%ld", arg1_int / arg2_int);
	} else if (0 == strcmp(operation, "%")) {
		size = snprintf(NULL, 0, "%ld", arg1_int % arg2_int);
		result = (char*) malloc(size + 1);
		sprintf(result, "%ld", arg1_int % arg2_int);
	} else cout << "something went awfully wrong" << endl;
	line.erase(line.begin()+stack_pointer-2, line.begin()+stack_pointer+1);
	line.insert(line.begin()+stack_pointer-2, result);
	line_pair ret_pair (stack_pointer-2, line);
	return ret_pair;
}

line_pair bi_condition(stack line, long stack_pointer, char *operation) { //built-in compare functions
	int arg_num = 2;
	if (0 == strcmp(operation, "~")) arg_num = 1;
	if (stack_pointer<arg_num) {
		cout << "not enough arguments for function " << operation << endl;
		return empty_pair;
	}
	stack args;
	for (int i = arg_num; i>=1; i--) {
		args.push_back(line[stack_pointer-i]);
		if (!is_numeric(line[stack_pointer-i])) {
			cout << "wrong arguments for function " << operation << desplit(args) << endl;
			return empty_pair;
		}
	}	
	long result = 0;
	if (0 == strcmp(operation, "<")) result = strtol(args[0], NULL, 10) < strtol(args[1], NULL, 10);
	else if (0 == strcmp(operation, "=")) result = strtol(args[0], NULL, 10) == strtol(args[1], NULL, 10);
	else if (0 == strcmp(operation, ">")) result = strtol(args[0], NULL, 10) > strtol(args[1], NULL, 10);
	else if (0 == strcmp(operation, "|")) result = strtol(args[0], NULL, 10) || strtol(args[1], NULL, 10);
	else if (0 == strcmp(operation, "&")) result = strtol(args[0], NULL, 10) && strtol(args[1], NULL, 10);
	else if (0 == strcmp(operation, "~")) result = !strtol(args[0], NULL, 10);
	else cout << "something went awfully wrong" << endl;
	line.erase(line.begin()+stack_pointer-arg_num, line.begin()+stack_pointer+1);

	//int result_size = snprintf(NULL, 0, "%ld", result);
	char *result_string = (char *) malloc(2);
	sprintf(result_string, "%ld", result);
	line.insert(line.begin()+stack_pointer-arg_num, result_string);
	line_pair ret_pair (stack_pointer-arg_num, line);
	return ret_pair;
}

line_pair bi_stack(stack line, long stack_pointer, char *operation) { //built-in stack functions
	int arg_num = 1;
	if (0 == strcmp(operation, "push")) arg_num = 2;
	if (stack_pointer<arg_num) {
		cout << "not enough arguments for function " << operation << endl;
		return empty_pair;
	}
	stack args;
	for (int i = arg_num; i>=1; i--) {
		if (i == arg_num && !is_stack(line[stack_pointer-i])) {
			cout << "wrong arguments for function " << operation << desplit(args) << endl;
			return empty_pair;
		} else args.push_back(line[stack_pointer-i]);
	}	
	char *result;
	if (0 == strcmp(operation, "pop")) result = pop_stack(args[0]);
	else if (0 == strcmp(operation, "popped")) result = popped_stack(args[0]);
	else if (0 == strcmp(operation, "is_empty")) result = is_empty_stack(args[0]);
	else if (0 == strcmp(operation, "push")) result = push_stack(args[0], args[1]);
	else cout << "something went awfully wrong" << endl;
	line.erase(line.begin()+stack_pointer-arg_num, line.begin()+stack_pointer+1);
	line.insert(line.begin()+stack_pointer-arg_num, result);
	line_pair ret_pair (stack_pointer-arg_num, line);
	return ret_pair;
}

//-------------------------------------------------------core
line_pair parse_function(stack, long, char*); //macht Funktion zu evaluierbarem Stack
void eval_file(string, bool);

stack evaluate(stack line, unsigned long stack_pointer = 0) { //evaluiert den Stack (führt Funktionen aus)
	line_pair ret_pair = empty_pair;
	if (debug_mode) cout << "evaluating " << desplit(line) << endl;
	if (stack_pointer >= line.size()) return line; 
	char *token = line[stack_pointer];
	//cout << "token: " << token << "," << stack_pointer << "," << strcmp(token, "+") << endl;
	if (is_numeric(token)) {
		if (debug_mode) cout << "numeric " << token << endl;
		stack_pointer += 1;
	} else if (token[0] == '(') { //wenn im main stack ein weiterer Stack liegt
		line = make_stack_string(line, stack_pointer);
		stack_pointer += 1;
	} else if (my_find(funs, token) != funs.end()) { //wenn der token eine bekannte Funktion ist
		if (debug_mode) cout << "known function " << token << endl;
		ret_pair = parse_function(line, stack_pointer, token);
		stack_pointer = ret_pair.first; line = ret_pair.second;
	} else { //sonst built-in-funktion oder unexpected identifier
		if (debug_mode) cout << "built-in or unknown " << token << endl;
		if (0 == strcmp(token, "+") || 0 == strcmp(token, "-") || 0 == strcmp(token, "*") || 0 == strcmp(token, "/") || 0 == strcmp(token, "%")){ 
			ret_pair = bi_math(line, stack_pointer, token);
		}
		else if (0 == strcmp(token, "<") || 0 == strcmp(token, "=") || 0 == strcmp(token, ">") || 0 == strcmp(token, "|") || 0 == strcmp(token, "&") || 0 == strcmp(token, "~")) 
			ret_pair = bi_condition(line, stack_pointer, token);
		else if (0 == strcmp(token, "pop") || 0 == strcmp(token, "push") || 0 == strcmp(token, "is_empty") || 0 == strcmp(token, "popped"))
			ret_pair = bi_stack(line, stack_pointer, token);
		else {
			cout << "unexpected token " << token << endl;
			return empty_stack;
		}
		stack_pointer = ret_pair.first; line = ret_pair.second;
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
		} else if (0 == strcmp(result[0], "0")) return false;
		else return true;
	}
	return false;
}

line_pair parse_function(stack line, long stack_pointer, char *fun_name) {
	if (debug_mode) cout << "parsing function " << desplit(line) << "(" << fun_name << ")" << endl;
	fun called_fun = my_find(funs, fun_name)->second;
	stack arg_names = called_fun.first;
	if (debug_mode) cout << desplit(arg_names) << "args" << endl;
	stack fun_stack = called_fun.second;
	long args_num = arg_names.size();
	if (stack_pointer < args_num) {
		cout << "not enough arguments for function " << fun_name << endl;
		return empty_pair;
	}
	map<char*, char*> args; //map für Namen und tatsächliche Werte der Argumente
	for (long i = 1; i <= args_num; i++) { 
		args[arg_names[args_num-i]] = line[stack_pointer - i]; //das Argument sei das n-te Element vorm Funktionsnamen
	}

	map<char*, char*>::iterator pos;
	for (unsigned long i = 0; i < fun_stack.size(); i++) { //dann iteriere über den tail
		pos = my_find(args, fun_stack[i]);
		if (pos != args.end())
			fun_stack[i] = pos->second; //und ersetze die Argumente mit ihren Werten
	}
	if (debug_mode) cout << "... " << desplit(fun_stack) << endl;
	stack::iterator question_mark = my_find(fun_stack, "?"); //ist es ein verzweigter Stack?
	if (question_mark != fun_stack.end()) { //wenn es ein Fragezeichenzeichen gibt, es also ein verzweigter Stack ist
		vector<stack> branch;
		long last_found = 0;
		for (unsigned long i = 0; i <= fun_stack.size(); i++) {
			if ( i == fun_stack.size() || 0 == strcmp(fun_stack[i], "?")) {
				stack::iterator fun_sign = my_find(fun_stack, ":", last_found, i); //wo ist in der Branch das Funktionszeichen?
				stack branch_head (fun_stack.begin()+last_found+(last_found!=0), fun_sign);
				stack branch_tail;
				if (fun_sign != fun_stack.end()) branch_tail.insert(branch_tail.begin(), fun_sign+1, fun_stack.begin()+i);
				else branch_tail.insert(branch_tail.begin(), fun_stack.begin()+last_found+1, fun_stack.end());
				if (debug_mode && i!=fun_stack.size()) cout << fun_stack.size() << " if " << desplit(branch_head) << "then " << desplit(branch_tail) << endl;
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
	line_pair ret_pair (stack_pointer-args_num, line);
	return ret_pair;
}



stack parse(stack line, bool execute = false) { //syntaktische Analyse (Funktionsdeklaration oder evaluierbarer Stack?)

	stack::iterator fun_sign = my_find(line, ":"); //wo ist das Funktionszeichen
	stack::iterator branched_fun_sign = my_find(line, "?"); //oder das verzweigte Funktionszeichen

	if (fun_sign == line.end() && branched_fun_sign == line.end()) line = evaluate(line); //wenn es eines gibt
	else {
		if (branched_fun_sign != line.end()) fun_sign = branched_fun_sign;
		stack fun_head (line.begin(), fun_sign); //Dann ist alles bis zum Funktionszeichen der Funktionskopf
		stack fun_tail (fun_sign+1, line.end()); //Der Rest der Schwanz
		char *fun_name = fun_head.back(); fun_head.pop_back(); //der Name ist der letzte token vorm Gleichzeichen
		stack fun_args = fun_head; //Der Rest sind Argumente
		fun new_fun (fun_args, fun_tail); //Das kommt in die map -> Argumente und der Funktionsschwanz
		funs[fun_name] = new_fun; //Füge die Funktion zu den derzeitigen Funktionen hinzu
		if (debug_mode) cout << fun_name << " head: " << desplit(fun_head) << "tail: " << desplit(fun_tail) << endl;
		if (execute && 0 == strcmp(fun_name, "main")) { //wenns ein file ist und das die main funktion ist
			stack args_line; //dann bauen wir eine line aus den cmd argumenten
			for (unsigned int i = 0; i<main_args.size(); i++) {
				args_line.push_back(main_args[i]);
			}
			args_line.push_back("main"); //und dem tail von main
			cout << desplit(evaluate(args_line)) << endl;
		} 
		return empty_stack;
	}
	return line;
}

void print_funs() {
	map<char*, fun>::iterator it = funs.begin();
	for (it = funs.begin(); it != funs.end(); it++){
		cout << it->first << ": " << desplit((it->second).first) << "; " << desplit((it->second).second) << endl;
	}

}

bool parse_command(string line) {
	if (line[1] == 'q') return true;
	if (line[1] == 'd') debug_mode = !debug_mode;
	if (line[1] == 'l') eval_file(split((char *)line.c_str())[1], false);
	if (line[1] == 'f') print_funs();
	return false;
}

void eval_file(string name, bool execute = false) {
	ifstream fun_file (name);
	string line;
	if (fun_file.is_open()) {
		while (getline(fun_file,line)) {
			if (line.front() == ':') (parse_command(line));
			else if (line[0]!='#') {
				parse(split((char *)line.c_str()), execute); //wenns kein kommentar ist, führs aus
			}
		}
    	fun_file.close();
  	} else cout << "Unable to open file " << name << endl; 
}

//--------------------------------------------------------main
int main(int argc,  char** argv) {
	eval_file("std.fun"); //standard library ist standard
	if (argc > 1) { 
		for (int i = 1; i < argc-1; i++) {
			main_args.push_back(argv[i]);
		}
		eval_file(argv[argc-1], true); //file ausführen
	} else { //REPL modus
		stack inp_stack; stack outp_stack;
		for (;;) { //Endlosschleife 
			rl_gets();
			string inp_line = line_read;
			if (inp_line.front() == ':') { //wenn es ein repl-befehl ist
				if (parse_command(inp_line)) break;
			} else { //wenn es ein Ausdruck der Sprache ist
				inp_stack.reserve(500);
				inp_stack = split((char *)inp_line.c_str());
				outp_stack = parse(inp_stack);
				cout << desplit(outp_stack) << "\n";
			}
		}
	}
	free (line_read); //ich hasse C
	line_read = (char *)NULL; //ich hasse es wirklich
	//std::cout << desplit(split("aa bbb cc d e4 f g s l")) << std::endl;
	//cout << is_numeric("- 5") << endl;
}
