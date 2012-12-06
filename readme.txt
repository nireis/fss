* ***************************************************************************
*
*  Copyright (c) 2012 University of Stuttgart,
*                     Sergej Geringer, Karsten Schatz, Florian Frieß
*  
*  Permission is hereby granted, free of charge, to any person obtaining a 
*  copy of this software and associated documentation files (the "Software"), 
*  to deal in the Software without restriction, including without limitation 
*  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
*  and/or sell copies of the Software, and to permit persons to whom the 
*  Software is furnished to do so, subject to the following conditions:
*  The above copyright notice and this permission notice shall be included 
*  in all copies or substantial portions of the Software.
*  
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
*  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
*  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
*  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
*  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
*  OTHER DEALINGS IN THE SOFTWARE.
*
* ***************************************************************************

* ***************************************************************************
*
*  this program was written by students in context of the course
*  'introduction to artificial intelligence' in winter 2012
*  at the University of Stuttgart, 
*  Institute for Visualization and Interactive Systems,
*  given by Prof. Dr.-Ing. Andrés Bruhn.
*  
*  the goal was to write a simple and relatively fast solver for sudokus. 
*  this implementation uses a recursive strategy, calling for the solve() 
*  function and ensuring consistency of the empty and already filled slots 
*  by keeping track of the possible valid entries. 
*  this is done in sudoku.update_slots() in each recursive call of solve().
* 
*  in short: the sudoku Constraint Satisfaction Problem is solved by 
*  determining the next empty field using a 'Minimum Remaining Values' 
*  strategy and backtracking the way throught the search-graph.
* 
*  contact: geringsj@studi.informatik.uni-stuttgart.de
*  Source Code at: https://github.com/nireis/fss
*
* ***************************************************************************


* Dokumentation: 
	Um die Lösung eines Sudokus zu finden, werden zunächst alle Felder nach der
	Anzahl der Möglichkeiten sortiert, mit denen diese auffüllbar sind. Hierfür 
	wurde eine Eine Datenstruktur (struct slot) angelegt, welche für jede der 
	Möglichkeiten abspeichert, ob diese so eintragbar wären. Diese Slots sind
	in eine Sudoku-Datenstruktur eingebettet, welche aus einem 9x9-Array und
	einigen anderen Werten besteht (struct sudoku). Die Sudokus werden in
	der read()-Methode aus der aufgabe.txt-Datei eingelesen und können, wenn
	sie gelöst wurden, mit der write()-Methode in der Datei loesung.txt
	abgespeichert werden. Beim einlesen werden Minus-Zeichen in Nullen
	umgewandelt, um das Sudoku in einem int-Array abspeichern zu können.
	Die Slots selbst werden in der init()-Methode initialisiert.
	
	Nachdem die Sortierung geschehen ist, wird, bevor man mit dem Backtracking
	beginnt, die "Minimum Remaining Values"-Strategie angewandt. Das soll
	heißen, dass zur Expandierung der Suche dasjenige Feld ausgewählt wird, 
	welches die geringste Anzahl an Möglichkeiten zur Belegung besitzt (diese 
	sollten jedoch ungleich 0 sein). Besitzt ein Feld nur eine Möglichkeit zur
	Belegung, kann diese natürlich sofort eingefüllt werden. Besitzt ein
	Feld keine Belegungsmöglichkeiten, wird der Lösungsversuch an dieser Stelle
	abgebrochen. Die Suche wird bei Fund des Feldes mit der geringsten Anzahl
	an Einfüllmöglichkeiten an genau dieser Stelle expandiert. Hierbei
	handelt es sich um eine Backtracking-Suche, die auch in den Folien zur
	Vorlesung beschrieben wird. Diese wird in der solve()-Methode durchgeführt,
	welche rekursiv arbeitet. Die solve()-Methode ruft die
	updateSlots()-Methode auf, um zu bestimmen, welche Slots wie viele Lösungs-
	möglichkeiten besitzen. Der Slot mit der geringsten Anzahl möglicher
	Belegungen wird ausgewählt und sämtliche Belegungen ausprobiert. Für jeden
	möglichen Wert der Belegung wird solve() für das neue Sudoku Feld aufgerufen, 
	in dem ein leeres Feld weniger vorhanden ist. 
	solve() gibt dann an die nächst höhere Rekursionsebene 'true' zurück, 
	sobald eine Lösung des Teil-Sudokus für eine bestimmte Belegung gefunden
	wurde, oder 'false' wenn das aktuelle Sudoku keine Lösung besitzt. 
	Eine Suche ist nicht erfolgreich, wenn ein Feld exisitiert, 
	welches nicht so ausgefüllt werden kann, dass es die Bedingungen für ein 
	korrektes Sudoku erfüllt. 
	
	Die Erfüllung dieser Bedingungen kann mit der verify()-Methode überprüft
	werden. Diese gibt zurück, ob ein Sudoku valide ist, oder nicht.
	Allerdings wird verify() nicht für die Suche nach Lösungen eingesetzt,
	sondern ist eher für Debugging-Zwecke nützlich. 
	Wenn solve() aus allen Rekursionen ein 'true' zurückgibt, ist das Sudoku bei
	Terminierung des allerersten solve()-Aufrufs mit einer gültigen Belegung
	ausgestattet und somit gelöst.

	Bemerke, dass eine Lösung für ein Sudoku gefunden wurde, sobald irgendein
	solve() ein 'true' zurückgibt, da von diesem Zeitpunkt an keine weiteren
	Werte im Sudoku mehr geändert werden und der Rekursionsbaum direkt bis zur 
	Wurzel zurückgelaufen wird.
	
	Nachdem alle Sudokus gelöst wurden, werden diese in die Datei loesung.txt
	geschrieben.


* Zusatzaufgabe: 
	Will man herausfinden, ob ein Sudoku mehrere Lösungen besitzt, muss
	man weitersuchen, selbst wenn man eine Lösung gefunden hat. 

	Praktisch heisst das, dass man eine 'pseudo-false' Funktionalität für solve()
	implementiert. Beim fund einer Lösung wird eine Sicherung der aktuellen 
	Belegungen durchgeführt, um die aktuelle Lösung nicht verloren gehen zu 
	lassen. Dann werden, statt ein 'true' auszugeben, weitere mögliche Belegungen
	für das aktuelle Feld ausprobiert (somit weitere Lösungen für das Feld und
	das resulierende Unter-Sudoku gesucht und gesichtert), bis alle möglichen
	Belegungen abgearbeitet sind und hierdurch ein 'false' zurückgegeben wird. 
	Hierdurch wird auch der initiale Aufruf von solve() ein 'false' ausgeben.
	Allerdings kann man durch einen Blick in die Sammlung von gesicherten
	Lösungen für dieses Sudoku dann sehen, ob es tatsächlich keine Lösung gibt,
	oder ob mindestens eine Lösung gefunden wurde. 

	Durch dieses Vorgehen verlängert man die Suche nach Lösungen, 
	weil man, sobald man eine Lösung besitzt, trotzdem weiterrechnet. 
	Insbesondere wenn keine weiteren Lösungen existieren, kann dies schnell zum 
	explodieren der Laufzeit führen, da dann der komplette Suchbaum 
	abgearbeitet werden muss. Varianten, in denen man nur nach den ersten k
	Lösungen sucht, und danach nicht mehr weitersucht, können dem traversieren
	des gesamten Suchraums entgegen steuern. 

* Gruppe: 
	Sergej Geringer, Karsten Schatz, Florian Frieß
