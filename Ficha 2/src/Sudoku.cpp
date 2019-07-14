/*
 * Sudoku.cpp
 *
 */

#include "Sudoku.h"

/** Inicia um Sudoku vazio.
 */
Sudoku::Sudoku()
{
	this->initialize();
}

/**
 * Inicia um Sudoku com um conteúdo inicial.
 * Lança excepção IllegalArgumentException se os valores
 * estiverem fora da gama de 1 a 9 ou se existirem números repetidos
 * por linha, coluna ou bloc 3x3.
 *
 * @param nums matriz com os valores iniciais (0 significa por preencher)
 */
Sudoku::Sudoku(int nums[9][9])
{
	this->initialize();

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (nums[i][j] != 0)
			{
				int n = nums[i][j];
				numbers[i][j] = n;
				lineHasNumber[i][n] = true;
				columnHasNumber[j][n] = true;
				block3x3HasNumber[i / 3][j / 3][n] = true;
				countFilled++;
			}
		}
	}
}

void Sudoku::initialize()
{
	numberOfSolutions = 0;

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			for (int n = 0; n < 10; n++)
			{
				numbers[i][j] = 0;
				lineHasNumber[i][n] = false;
				columnHasNumber[j][n] = false;
				block3x3HasNumber[i / 3][j / 3][n] = false;
			}
		}
	}

	this->countFilled = 0;
}

/**
 * Obtem o conteúdo actual (só para leitura!).
 */
int** Sudoku::getNumbers()
{
	int** ret = new int*[9];

	for (int i = 0; i < 9; i++)
	{
		ret[i] = new int[9];

		for (int a = 0; a < 9; a++)
			ret[i][a] = numbers[i][a];
	}

	return ret;
}


/**
 * Verifica se o Sudoku já está completamente resolvido
 */
bool Sudoku::isComplete()
{
	return countFilled == 9 * 9;
}


/**
 * Resolve o Sudoku.
 * Retorna indicação de sucesso ou insucesso (sudoku impossível).
 */
bool Sudoku::solve()
{
	return solve(false);
}
bool Sudoku::solve(bool getSolutionCount)
{
	// Verificar se tá completo
	if (isComplete())
	{
		if (getSolutionCount)		// Com getSolutionCount == true, em vez de parar ao completar a matrix, aumentar o contador de soluções,
		{							// e continuar como se fosse uma falha, o que faz percorrer todas as combinações possiveis.
			numberOfSolutions++;
			return false;
		}
		else
			return true;
	}

	// Procurar a casa com menos opçoes válidas.
	int x, y;
	if(!GetOptimalCell(x, y))
	{
		return false;
	}
	//print();
	//cout << endl << endl;

	// Testar cada numero possivel nessa casa:
		//ver se é valido (nao quebra nenhuma regra) -> guardar info nessa casa -> chamar recursivamente solve() -> se falhar, desfazer mudanças -> tentar outro numero

	for (int k = 1; k <= 9; k++)
	{
		if (!lineHasNumber[x][k] && !columnHasNumber[y][k] && !block3x3HasNumber[x / 3][y / 3][k])
		{
			numbers[x][y] = k;
			lineHasNumber[x][k] = true;
			columnHasNumber[y][k] = true;
			block3x3HasNumber[x / 3][y / 3][k] = true;
			countFilled++;

			if (solve(getSolutionCount))
				return true;

			numbers[x][y] = 0;
			lineHasNumber[x][k] = false;
			columnHasNumber[y][k] = false;
			block3x3HasNumber[x / 3][y / 3][k] = false;
			countFilled--;
		}
	}
	return false;
}


/**
 * Retorna a multiplicidade de soluções de um Sudoku (sem solução, com uma só solução, com mais do que uma solução)
 */
int Sudoku::CheckSolutions()
{
	numberOfSolutions = 0;
	solve(true);
	return numberOfSolutions;
}


/**
 * Retorna a casa com a menor quantidade de numeros válidos.
 */
bool Sudoku::GetOptimalCell(int &x, int &y)
{
	int bestCount = 99;
	int currentCount = 0;
	bool gotCell = false;

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (numbers[i][j] == 0)
			{
				currentCount = 0;
				for (int k = 0; k < 9; k++)
				{
					if(!lineHasNumber[i][k] && !columnHasNumber[j][k] && !block3x3HasNumber[i / 3][j / 3][k])
					{
						currentCount++;
					}
				}

				if (currentCount == 0)
					return false;

				if (currentCount < bestCount)
				{
					gotCell = true;
					bestCount = currentCount;
					x = i;
					y = j;

					if (currentCount == 1)
						return true;
				}
			}
		}
	}
	return gotCell;
}


/**
 * Imprime o Sudoku.
 */
void Sudoku::print()
{
	for (int i = 0; i < 9; i++)
	{
		for (int a = 0; a < 9; a++)
			cout << this->numbers[i][a] << " ";

		cout << endl;
	}
}
