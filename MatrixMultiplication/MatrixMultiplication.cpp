// MatrixMultiplication.cpp 
// author: Денисова Екатерина

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <random>
#include <chrono>

using namespace std;

const int MATRIX_SIZE = 1000; // размер матрицы
const int NUM_THREADS = 6; // количество потоков

mutex mtx; // мьютекс для синхронизации доступа к общей памяти


// функция для заполнения матрицы случайными числами
void fillMatrixRandom(vector<vector<int>>& matrix)
{
    for (size_t i = 0; i < matrix.size(); ++i)
    {
        for (size_t j = 0; j < matrix[0].size(); ++j)
        {
            matrix[i][j] = rand() % 10 + 1; // генерация случайного числа от 1 до 100
        }
    }
}


// функция для перемножения подматриц
// матрицы matrixA и matrixB - исходные матрицы 
// результат перемножения записывается в матрицу result
// startRow - индекс строки, с которой начинается умножение подматриц
// endRow - индекс строки, на которой заканчивается умножение подматриц (не включительно)
void multiplyMatrices(vector<vector<int>>& matrixA, vector<vector<int>>& matrixB, vector<vector<int>>& result, int startRow, int endRow)
{

    int cols_A = matrixA[0].size(); // количество столбцов в матрице matrixA
    int cols_B = matrixB[0].size(); // количество столбцов в матрице matrixB

    // цикл по строкам подматриц
    for (int i = startRow; i < endRow; ++i)
    {
        // Цикл по столбцам матрицы B
        for (int j = 0; j < cols_B; ++j)
        {
            int sum = 0;

            // Цикл перемножения элементов подматрицы
            for (int k = 0; k < cols_A; ++k)
            {
                sum += matrixA[i][k] * matrixB[k][j];
            }

            mtx.lock(); // блокировка мьютекса
            result[i][j] = sum;
            mtx.unlock(); // разблокировка мьютекса
        }
    }
}

// Функция для вывода матрицы
void printMatrix(const vector<vector<int>>& matrix)
{
    for (const auto& row : matrix)
    {
        for (int num : row)
        {
            cout << num << " ";
        }
        cout << endl;
    }
}


int main()
{
    setlocale(LC_ALL, "Russian");

    // Инициализация генератора случайных чисел
    srand(time(nullptr));

    // массив из потоков
    vector<thread> threads;

    // создание матриц
    vector<vector<int>> matrixA(MATRIX_SIZE, vector<int>(MATRIX_SIZE));
    vector<vector<int>> matrixB(MATRIX_SIZE, vector<int>(MATRIX_SIZE));
    vector<vector<int>> result(MATRIX_SIZE, vector<int>(MATRIX_SIZE));

    // Инициализация матриц matrixA и matrixB (заполнение случайными числами)
    fillMatrixRandom(matrixA);
    fillMatrixRandom(matrixB);

    // определяем количество строк на поток
    int rowsPerThread = MATRIX_SIZE / NUM_THREADS;

    // начало измерения времени
    auto startTime = chrono::high_resolution_clock::now();

    // цикл по всем потокам
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        // вычисляется начальная строка
        int startRow = i * rowsPerThread;

        // вычисляется конечная строка
        // если текущий поток последний, то конечная строка будет равна MATRIX_SIZE,
        // иначе конечная строка будет равна начальной строке следующего потока
        int endRow = (i == NUM_THREADS - 1) ? MATRIX_SIZE : (i + 1) * rowsPerThread;

        // создаём поток и добавляем его в массив потоков
        // multiplyMatrices - выполняемая функция
        threads.push_back(thread(multiplyMatrices, ref(matrixA), ref(matrixB), ref(result), startRow, endRow));
    }

    // ожидание выполнения всех потоков (основной поток заблокирован, пока не выполнятся все отдельные потоки)
    for (auto& th : threads)
    {
        th.join();
    }

    // конец измерения времени
    auto endTime = chrono::high_resolution_clock::now();

    // рассчитываем время выполнения перемножения
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
    
    // Вывод результата
    cout << "Время перемножения для " << NUM_THREADS << " потоков: " << duration << " миллисекунд" << endl;
    
    /*cout << "Ok";
    printMatrix(matrixA);
    cout << endl;
    printMatrix(matrixB);
    cout << endl;
    printMatrix(result);*/

}

