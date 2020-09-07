#ifndef QUEUE_H
#define QUEUE_H
#include <iostream>
using namespace std;
template <class T>
class Queue
{
private:
	//My private array, number of elements, and length of the underlying array
	T* arr;
	int len;
	int arrLen;
public:
	Queue() {
		//Initializing the queue with no elements and one free spot in memory
		arr = new T[1];
		len = 0;
		arrLen = 1;
	}
	~Queue() {
		//Freeing the memory
		delete[] arr;
		arr = nullptr;
	}
	Queue(const Queue& queue) {
		//Changing our len and arrLen to fit the other queue's, then copying the elements of its array over
		len = queue.len;
		arrLen = queue.arrLen;
		T* newArr = new T[arrLen];
		for (int c = 0; c < len; c++) {
			newArr[c] = queue.arr[c];
		}
		//Deleting our current array's memory space and setting it equal to the new array
		delete[] arr;
		arr = newArr;
	}
	Queue& operator=(const Queue& queue) {
		if (this != &queue) {
			//The exact same as the above method
			len = queue.len;
			arrLen = queue.arrLen;
			T* newArr = new T[arrLen];
			for (int c = 0; c < len; c++) {
				newArr[c] = queue.arr[c];
			}
			delete[] arr;
			arr = newArr;
		}
		//Except it returns a value
		return *this;
	}
	void Sort(T data, T sortArr[]) {
		//Takes an array and some data, and sorts the data into the array
		for (int c = 0; c < len; c++) {
			if (data < sortArr[c]) {
				//If it's found the right spot, shifts everything to the right and slots it in
				for (int d = len; d > c; d--) {
					sortArr[d] = sortArr[d - 1];
				}
				sortArr[c] = data;
				return;
			}
		}
		//Or if the data is "bigger" than everything else, puts it at the end
		sortArr[len] = data;
	}
	void Push(T data) {
		if (len < arrLen) {
			//If there's still room in the array, sorts the new data in
			Sort(data, arr);
		}
		else {
			//Otherwise, makes a new array of twice the size, fills in the appropriate values, and adds the data
			arrLen *= 2;
			T* newArr = new T[arrLen];
			for (int c = 0; c < len; c++) {
				newArr[c] = arr[c];
			}
			//Sorts the data in
			Sort(data, newArr);
			//Frees our old array and sets it equal to the new one
			delete[] arr;
			arr = newArr;
		}
		len++;
	}
	T Pop() {
		if (len > 0) {
			len--;
			//If the array is not empty, removes and returns the first element
			T data = arr[0];
			arr[0] = NULL;
			//Shifts everything to the left
			for (int c = 0; c < len; c++) {
				arr[c] = arr[c + 1];
			}
			arr[len] = NULL;
			return data;
		}
		return NULL;
	}
	void Print() {
		//Prints out everything in the array
		for (int c = 0; c < len; c++) {
			cout << arr[c] << " ";
		}
		cout << endl;
	}
	int GetSize() {
		//Returns the length of the array
		return len;
	}
	bool IsEmpty() {
		//Returns whether the array has any elements
		if (len > 0) {
			return false;
		}
		return true;
	}

};

#endif //QUEUE_H