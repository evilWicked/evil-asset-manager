#pragma once

#include "../test-data-table.h"

#include <vector>
#include <string>

namespace evil {

template<typename T>
TestDataTable<T>::TestDataTable(int cols) :miCols(cols) {
	miColIndex = 0;
	miRowIndex = 0;
};

template<typename T>	
int TestDataTable<T>::Cols() { return miCols; }
	
template<typename T>
int TestDataTable<T>::Rows() { return (int)mTable.size(); }

	
		///inserts a new row of T
template<typename T>
void TestDataTable<T>::insertRow(){
	//display_row r(miCols);
	mTable.emplace_back(miCols);
}


template<typename T>
int TestDataTable<T>::insertCell(T s) {
	if (miColIndex < miCols) {
		mTable[miRowIndex][miColIndex] = s;
	}

	return nextCol();
};

		
template<typename T>
int TestDataTable<T>::nextCol() {
	miColIndex++;
	if (miColIndex >= miCols) {
		miColIndex = 0;
	}
	return miColIndex;
}


template<typename T>
int TestDataTable<T>::nextRow() {
	if (miRowIndex < mTable.size()) {
		miRowIndex++;
		miColIndex = 0;
	}
	return miRowIndex;
}

template<typename T>
void TestDataTable<T>::newRow() {
	insertRow();		
}

template<typename T>
void TestDataTable<T>::reset() {
	miColIndex = 0;
	miRowIndex = 0;
}

template<typename T>
T TestDataTable<T>::get(int row, int col) {
	//do we need to check bounds??
	return mTable[row][col];
}

template<typename T>
void TestDataTable<T>::set(int row, int col, T s) {
	mTable[row][col] = s;
}

template<typename T>
void TestDataTable<T>::clear() {
	mTable.clear();
	reset();
}


}