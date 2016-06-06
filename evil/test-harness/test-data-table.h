#pragma once

#include <vector>

namespace evil {

	///@brief A data table used to capture test results in memory.
	///
	///A data table to capture the results of testing in memory so that delays due to simultaneous access
	///of the terminal don't impact on results.  In simle terms we collect all results and dump at the end 
	///rather than while testing is proceeding. 
template<typename T>
	class TestDataTable {

		typedef std::vector<T> display_row;
		typedef std::vector<display_row> display_table;
		
		display_table mTable;
		
		const int miCols;
		int miColIndex;
		int miRowIndex;

	public:
		///create a data table with no rows in it
		TestDataTable(int cols);

		int Cols();
		int Rows();
	
		///inserts a new row of T
		void insertRow();

		///insert a T into the current column of the current row and then calls nextCol
		int insertCell(T s);
		
		///If on the last element of a row resets the columm index to 0 and returns it 
		///otherwise increments the column index and returns it. Does not add a new row.
		int nextCol();

		///move to the next row, set the col index to 0 and return the new row index 
		///if on the last row returns the same row index the previous iteration and doesnt change the column
		///the idea being you can check for row==row_previous and add a new row if needed
		int nextRow();

		///adds a new row at the end of the table does not change the row index
		void newRow();

		///sets the indexes to the top of the table
		void reset();

		///returns the element in a position
		T get(int row, int col);

		///set the element in a position
		void set(int row, int col, T s);

		///empties the table
		void clear();

	public:
		//Hide and disable/default all constructors and destructors unless specifically overridden
		//this is a mnemonic to force me to think about things. if overridden they are commented
		//TestDataTable() = default;
		~TestDataTable() = default;
		TestDataTable(const TestDataTable& rhs) = delete;
		TestDataTable& operator=(const TestDataTable& rhs) = delete;
		TestDataTable(TestDataTable&& other) = delete;
		TestDataTable& operator=(TestDataTable&& other) = delete;
	};
}

