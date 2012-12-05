
Copyright (c) 2012 University of Stuttgart,
                   Sergej Geringer, Karsten Schatz, Florian Frieß

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
OTHER DEALINGS IN THE SOFTWARE.


this program was written by students in context of the course
'introduction to artificial intelligence' in winter 2012
at the University of Stuttgart, 
Institute for Visualization and Interactive Systems,
given by Prof. Dr.-Ing. Andrés Bruhn.

the goal was write a simple and relatively fast solver for sudokus. 
this implementation uses a recursive strategy, calling for the solve() 
function and ensuring consistency of the empty and already filled slots 
by keeping track of the possible valid entries. 
this is done in sudoku.update_slots() in each recursive call of solve().

in short: the sudoku Constraint Satisfaction Problem is solved by 
determining the next empty field using a 'Minimum Remaining Values' 
strategy and backtracking the way throught the search-graph.

contact: geringsj@studi.informatik.uni-stuttgart.de

===============================================================================

* Dokumentation: 
	TODO Konzept 

* Zusatzaufgabe: 
	TODO Wenn man fertig ist sich die erste Lösung merken und dann
	weitersuchen als wäre nichts gewesen. Hat allerdings den Nachteil, dass man
	unnötig lange sucht, wenn es nicht mehr Lösungen gibt. 

* Gruppe: 
	Sergej Geringer, Karsten Schatz, Florian Frieß
