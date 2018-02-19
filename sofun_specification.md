# SOFUNLANG - a Stack Oriented FUNctional LANGuage

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
in umgekehrter Reihenfolge zurückgibt (e1 e2 a : e2 e1), wird der Stack danach folgendermaßen
aussehen: 

	1 3 2
	
Funktionen werden also sozusagen argumentabhängig zu einem Stack der dann anstelle der Funktion
im mainstack erscheint. Das funktionale Prinzip, dass Funktionen beim selben Input immer denselben
output returnen müssen, ist so auch in SOFUNLANG gewährleistet und auch SOFUNLANG beruht
vollständig auf Funktionen: Die Sprache funktional zu nennen ist also trotz der schwachen Typisierung berechtigt.

SOFUNLANG wird in reverse polish notation geschrieben, d.h. `2 + 3` wird in SOFUNLANG `2 3 +` 
geschrieben. Eine Folge von RPN ist, dass jeder Token der Sprache, sowohl Funktionen als auch einfache Werte, durch Leerzeichen voneinander getrennt sein müssen.

Funktionen werden mit ":" deklariert. Eine Funktion, die eine Zahl im Quadrat zurückgibt, schreibt
man also: 

	a quad : a a *
	
wobei auch Rekursion möglich ist.
Komplexere Funktionen benötigen natürlich Verzweigungen, in SOFUNLANG werden Verzweigungen mit einem "?"
ausgedrückt. Verzweigte Funktion muss mindestens zwei Fragezeichen enthalten: Jedes Fragezeichen in einer Funktion,
bis auf das letzte sind gefolgt von einer Kondition, der Stack nach dem letzten Fragezeichen wird zurückgegeben, wenn
keine der vorherigen Konditionen zutrifft. Dieses letzte Fragezeichen ist Pflicht, da eine Funktion immer einen 
Rückgabewert besitzen muss. Es kann allerdings ein leerer Stack zurückgegeben werden.
Wollen wir eine Funktion für alle Potenzen definieren tun wir das also mit:

	exp base ^ ? 1 exp < : 1 ? base exp 1 - base ^ *
	
Hier wird solange base mit sich selbst multipliziert, bis exp, von dem bei jeder 
Rekursionsstufe 1 abgezogen wird, kleiner als 1 wird. So multiplizieren wir genau exp mal.

## Built-in-Funktionen

Logische Operationen | Mathematische Operationen
------------ | -------------
< (kleiner als) | + (plus)
\> (größer als) | - (minus)			  
= (ist gleich) | \* (mal)			      
\| (oder) | / (geteilt)				      
& (und) | % (modulo)
~ (nicht) |

## Die REPL
Die Repl hat zusätzlich zu den reinen Sprachbefehlen noch einige andere Befehle, diese werden mit einem Doppelpunkt eingeleitet:


:quit oder :q verlässt die REPL.

:load oder :l lässt dich files laden. Die darin deklarierten Funktionen sind daraufhin in der REPL verfügbar.

:debug oder :d lässt dich durch viel, viel output den Interpretierprozess nachvollziehen. Im Normalfall hoffentlich nur für den Entwichkler interessant

## Struktur eines Programms:

Ein Programm in SOFUNLANG besteht also aus folgendem:

- Ein Mainstack: Ein einfacher Stack. Hier werden definierte Funktionen mit Argumenten aufgerufen und mit RPN evaluiert. Beispiel: `2 3 +` evaluiert zu 5. 
- Beliebig viele Funktionen. Hier ist zwischen verschiedenen Typen zu unterscheiden:
  - Namen: Die einfachste Funktion ist einfach, einem Stack einen Namen zu geben. Beispiel: `c : 2 3 +`. Ruft man diesen Namen mit `c` auf, wird er `5` zurückgeben. *Formal:* `Name : Stack` 
  - Einfache Funktionen: Diese Funktionen nehmen Argumente, die vom Hauptstack gepopt werden. Beispiel `a b c : a b +`. Ruft man diese Funktion mit zwei Argumenten auf (`2 3 c`) gibt sie 5 zurück. *Formal:* `Argument_n ... Argument_1 Name : Stack`
  - Verzweigte Funktionen: Diese Funktionen haben abhängig von ihrern Argumenten unterschiedliche Arten, den Rückgabewert zu berechnen. Das wird vor allem bei rekursiven Funktionen wichtig, ermöglich aber Beispielsweise auch schon die Definition von der "kleiner gleich"-Funktion in der Standardbibliothek: `a b <= ? a b < : 1 ? a b = : 1 ? 0`. *Formal:* `Argument_n ... Argument_1 Name ? Bedingung_1 : Stack ... ? Bedingung_n : Stack ? Stack`
