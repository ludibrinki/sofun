# SOFUNLANG -- a Stack-Oriented FUNctional LANGuage

KOMPILIEREN MIT g++ -o sfrepl sofunrepl.cpp -lstdc++ -std=c++11

SOFUNLANG ist in erster Linie stackorientiert - das heißt alles ist ein Stack. Erst an zweiter
Stelle ist es auch eine funktionale Sprache. Völlig pur funktional ist sie darum nicht:
Funktionen returnen Stacks aus mehreren Elementen statt nur einem Element. Die Menge der 
returnten Elemente kann variieren. 
Insgesamt kann eine Funktion sozusagen als bedingter Teil des mainstacks 
betrachtet werden:
Haben wir einen Stack 
	1 2 3 a
wobei a eine Funktion ist, die zwei Argumente nimmt und sie 
in umgekehrter Reihenfolge zurückgibt (e2 e1 a : e1 e2), wird der Stack danach folgendermaßen
aussehen: 
	1 3 2.
Funktionen werden also sozusagen argumentabhängig zu einem Stack der dann anstelle der Funktion
im mainstack erscheint. Das funktionale Prinzip, dass Funktionen beim selben Input immer denselben
output returnen müssen, ist so auch in SOFUNLANG gewährleistet und auch SOFUNLANG beruht
vollständig auf Funktionen: Die Sprache funktional zu nennen ist also trotz der schwachen Typisierung berechtigt.

SOFUNLANG wird in reverse polish notation geschrieben, d.h. 2 + 3 wird in SOFUNLANG "3 2 +" 
geschrieben. VORSICHT! Das erste Argument der Funktion ist (da in Reihenfolge vom mainstack gepopt wird) das der Funktion 
nächste, also das rechteste, nicht das linkeste. Darauf sollte für guten Stil auch bei eigenen Funktionen geachtet
werden.
Eine zweite Folge von RPN ist, dass jeder Token der Sprache, sowohl Funktionen als auch einfache Werte, durch Leerzeichen
voneinander getrennt sein müssen.

Funktionen werden mit ":" deklariert. Eine Funktion, die eine Zahl im Quadrat zurückgibt, schreibt
man also: 
	a quad : a a *
wobei auch Rekursion möglich ist.
Komplexere Funktionen benötigen natürlich Verzweigungen, in SOFUNLANG werden Verzweigungen mit einem "?"
ausgedrückt. Verzweigte Funktion muss mindestens zwei Fragezeichen enthalten: Jedes Fragezeichen in einer Funktion,
bis auf das letzte sind gefolgt von einer Kondition, der Stack nach dem letzten Fragezeichen wird zurückgegeben, wenn
keine der vorherigen Konditionen zutrifft. Dieses letzte Fragezeichen ist Pflicht, da eine Funktion immer einen 
Rückgabewert besitzen muss. Es kann allerdings ein leerer Stack zurückgegeben werden.
Wollen wir eine Funktion für alle Potenzen definieren tun wir das also mit;
	exp base ^ ? 1 exp < : 1 ? base 1 exp - base ^ *
Hier wird solange base mit sich selbst multipliziert, bis exp, von dem bei jeder 
Rekursionsstufe 1 abgezogen wird, kleiner als 1 wird. So multiplizieren wir genau exp mal.

## Built-in-Funktionen

Logische Operationen:		

"<" (Kleiner als)				  
"=" (ist gleich)				  
">" (größer als)				  
"|" (oder)					      
"&" (und)						      
"~" (nicht)

Rechenoperationen:

"+" (plus)
"-" (minus)
"\*" (mal)
"/" (geteilt)
"%" (modulo)

