# SOFUN
  Sofun (Abk.: Stack-Oriented FUNctional language) ist in erster Linie eine stackorientierte Sprache: Alle Ausdrücke (d.h. Werte und Funktionen) werden vom Interpreter in Stacks aufgelöst und als einzige Datenstruktur stehen ebenfalls Stacks zur Verfügung. Funktionen können dabei als - teilweise argumentabhängige - Repräsentationen von Abschnitten des aufrufenden Stacks angesehen werden: Sie werden vom Interpreter, wie Inline-Funktionen in imperativen Sprachen, durch die repräsentierten Stacks ersetzt. Nach dem Parsing besteht also das ganze Programm aus einem einzigen Stack aus Elementarfunktionen - dem Mainstack. Dieser wird evaluiert und ergibt den Rückgabewert des Programms.
  
  Obwohl die Sprache also einige Eigenschaften einer funktionalen Sprache teilt, ist sie nicht rein funktional. 
  Hier ein Beispiel des Syntax:

    num sums ? 1000 num = : 0 ? num 3 % ~ num 5 % ~ | : num num 1 + sums + ? num 1 + sums