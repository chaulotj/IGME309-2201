
#include "queue.h"
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

void wrapper() {
	//My three queues
	Queue<int> q1 = Queue<int>();
	Queue<char> q2 = Queue<char>();
	Queue<double> q3 = Queue<double>();
	//Pushing a bunch of ints to q1
	q1.Push(23);
	q1.Push(15);
	q1.Push(4);
	q1.Push(16);
	q1.Push(42);
	q1.Push(8);
	q1.Print(); //Prints q1 out, should say 4 8 15 16 23 42
	cout << q1.GetSize() << endl; //Prints out the size of q1, should be 6
	//Popping two out of q1
	q1.Pop();
	q1.Pop();
	q1.Print(); //Prints q1 out, should say 15 16 23 42
	cout << q1.GetSize() << endl; //Prints out the size of q1, should be 4
	//Pushing a bunch of chars to q1
	q2.Push('C');
	q2.Push('o');
	q2.Push('r');
	q2.Push('o');
	q2.Push('n');
	q2.Push('a');
	q2.Print(); //Prints q2 out, should say C a n o o r
	//Popping two out of q2
	q2.Pop();
	q2.Pop();
	q2.Print(); //Prints q2 out, should say n o o r
	//Pushing a bunch of doubles to q3
	q3.Push(1.25);
	q3.Push(5.8);
	q3.Push(9.345);
	q3.Push(69.69);
	q3.Push(420.666);
	q3.Push(1.01);
	q3.Print(); //Prints q3 out, should say 1.01 1.25 5.8 9.345 69.69 420.666
	//Popping two out of q3
	q3.Pop();
	q3.Pop();
	q3.Push(15.16); //Pushes a new double into q3
	q3.Print(); //Prints q3 out, should say 5.8 9.345 15.16 69.69 420.666
	Queue<int> q4(q1); //Uses copy constructor to make q4 the same as q1
	q4.Print(); //Prints q4 out, should say 15 16 23 42
	//Popping two out of q4
	q4.Pop();
	q4.Pop();
	q4.Print(); //Prints q4 out, should say 23 42
	//Popping the final two out of q4
	q4.Pop();
	q4.Pop();
	if (q4.IsEmpty()) { //If q4 is empty (which it is) this should print
		cout << "You should be seeing this because q4 is currently empty!\n";
	}
	q4 = q1; //Uses the copy assignment operator to set q4 equal to q1
	q4.Print(); //Prints q4 out, should say 15 16 23 42
}

int main() {
	wrapper(); //Executes all the stuff in wrapper
	_CrtDumpMemoryLeaks(); //Checks for memory leaks
}