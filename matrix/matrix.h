#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "../common/common_objs.h"

class Matrix {

	std::unique_ptr<int[]> data;
	int rows; // row count
	int columns; // column count

public:

	Matrix(const int _n, const int _m) : rows(_n), columns(_m)
	{
		assert(rows >= 0 && columns >= 0);
		data = std::make_unique<int[]>(rows * columns);  // NOLINT(bugprone-implicit-widening-of-multiplication-result)
		//set the array to 0
		std::fill_n(data.get(), rows*columns, 0);
	}

	// i -> 0 to n-1
	// j -> 0 to m-1
	// ReSharper disable once CppMemberFunctionMayBeConst
	void set_value(const int i, const int j, const int value)
	{
		data.get()[i * columns + j] = value;
	}

	// ReSharper disable once CppMemberFunctionMayBeConst
	void set_all(const int value)
	{
		std::fill_n(data.get(), rows * columns, value);
	}

	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	static void multiply(const Matrix* x, const Matrix* y, Matrix* results)
	{
		//check the matrix sizes are correct to multiply
		// ReSharper disable once CppRedundantComplexityInComparison
		if (!(x->columns == y->rows) || !((x->rows == results->rows) && (y->columns == results->columns)))
		{
			std::cout << " ERROR : Invalid matrix sizes for multiplication \n";
			return;
		}

		// r = result_size
		const int r = results->rows * results->columns;

		for (int i = 0; i < r; i++)
		{
			for (int j = 0; j < x->columns; j++)
			{
				results->data.get()[i] += x->data.get()[ (i / results->columns) * x->columns + j ] 
					* y->data.get()[ i % results->rows + j*y->columns ];
			}	
		}
	}

	static void parallel_multiply(const Matrix* x, const Matrix* y, Matrix* results)
	{
		struct process_data_chunk
		{
			// ReSharper disable once CppParameterMayBeConstPtrOrRef
			void operator()(Matrix* results, const Matrix* x, const Matrix* y, const int start_index, const int end_index) const
			{
				for (int i = start_index; i < end_index; i++)
				{
					for (int j = 0; j < x->columns; j++)
					{
						results->data.get()[i] += x->data.get()[(i / results->columns) * x->columns + j]
							* y->data.get()[i % results->rows + j * y->columns];
					}
				}
			}

		};
		
		//check the matrix sizes are correct to multiply
		if (!((x->rows == results->rows) && (y->columns == results->columns)) || (x->columns != y->rows))
		{
			std::cout << " ERROR : Invalid matrix sizes for multiplication \n";
		}

		// r = result_size
		const int length = results->rows * results->columns;

		if (!length)
			return;

		constexpr int min_per_thread = 10'000;
		const int max_threads = (length + min_per_thread - 1) / min_per_thread;
		const int hardware_threads = static_cast<int>(std::thread::hardware_concurrency());
		const int num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
		const int block_size = length / num_threads;

		{
			std::vector<std::thread> threads(num_threads - 1);
			int block_start = 0;
			int block_end = 0;
			join_threads joiner(threads);

			for (int i = 0; i < (num_threads - 1); i++)
			{
				block_start = i * block_size;
				block_end = block_start + block_size;
				threads[i] = std::thread(process_data_chunk(), results, x, y, block_start, block_end);
			}

			// perform the find operation for final block in this thread.
			process_data_chunk()(results, x, y, block_end, length);
		}
	}

	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	static void transpose(const Matrix* x,  Matrix* results)
	{
		//check the matrix sizes are correct to multiply
		if ( !((x->columns == results->rows) && (x->rows == results->columns)) )
		{
			std::cout << " ERROR : Invalid matrix sizes for transpose \n";
			return;
		}

		// r = result_size
		const int r = results->rows * results->columns;

		for (int i = 0; i < r; i++)
		{
			//get the current row and column count
			const int result_row = i / results->columns;
			const int result_column = i % results->columns;

			//flipped the columns and row for input
			const int input_row = result_column;
			const int input_column = result_row;

			//store the corresponding element from input to the results
			results->data.get()[i] = x->data.get()[input_row * x->columns + input_column];
		}
	}

	static void parallel_transpose(const Matrix* x,  Matrix* results)
	{
		struct process_data_chunk
		{
			// ReSharper disable once CppParameterMayBeConstPtrOrRef
			void operator()(Matrix* results, const Matrix* x, const unsigned start_index, const unsigned end_index) const
			{
				for (unsigned i = start_index; i < end_index; i++)
				{
					const unsigned result_row = i / results->columns;
					const unsigned result_column = i % results->columns;

					const unsigned input_row = result_column;
					const unsigned input_column = result_row;

					results->data.get()[i] = x->data.get()[input_row * x->columns + input_column];
				}
			}

		};

		//check the matrix sizes are correct to multiply
		if (!((x->columns == results->rows) && (x->rows == results->columns)))
		{
			std::cout << " ERROR : Invalid matrix sizes for transpose \n";
			return;
		}

		// r = result_size
		const int length = results->rows * results->columns;

		if (!length)
			return;

		constexpr int min_per_thread = 10'000;
		const unsigned max_threads = (length + min_per_thread - 1) / min_per_thread;
		const unsigned hardware_threads = std::thread::hardware_concurrency();
		const unsigned num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
		const unsigned block_size = length / num_threads;

		{
			std::vector<std::thread> threads(num_threads - 1);
			unsigned block_start = 0;
			unsigned block_end = 0;
			join_threads joiner(threads);

			for (unsigned long i = 0; i < (num_threads - 1); i++)
			{
				block_start = i * block_size;
				block_end = block_start + block_size;
				threads[i] = std::thread(process_data_chunk(), results, x, block_start, block_end);
			}

			// perform the find operation for final block in this thread.
			process_data_chunk()(results, x, block_end, length);
		}
	}

	void print() const
	{
		if ( rows < 50 && columns < 50 )
		{
			for (int i = 0; i < rows; i++)
			{
				for (int j = 0; j < columns; j++)
				{
					std::cout << data.get()[j + i * columns] << " ";
				}

				std::cout << "\n";
			}
			std::cout << std::endl;
		}
	}

};