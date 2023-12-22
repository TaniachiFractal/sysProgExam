#include <windows.h> 
#include <iostream> 
using namespace std;
// главная функция с аргументами wchar_t
int wmain(int argc, wchar_t* argv[])
{
	
	/*
	
	ПОЛУЧЕНИЕ РАЗМЕРА МАССИВА
	
	*/
	// Получаем handlы каналов
	HANDLE ReadPipe = (HANDLE)_wtoi64(argv[1]);
	HANDLE WritePipe = (HANDLE)_wtoi64(argv[2]);

	DWORD BytesWritten, BytesRead;

	Sleep(500);

	long n = 0;
	WriteFile(WritePipe, &n, sizeof(n), &BytesWritten, nullptr);

	int arrLength = 0;

	// Ожидаем записи в канал
	WaitForSingleObject(ReadPipe, INFINITE);

	// Читаем значение
	ReadFile(ReadPipe, &arrLength, sizeof(arrLength), &BytesRead, nullptr);

	// Выводим его в консоль
	cout << "The array length is " << arrLength << endl;

	HANDLE hStdOut; // дескриптор стандартного вывода 

	cout << endl << endl;

	/*

	УСТАНОВКА КУРСОРА

	*/
	{
		CONSOLE_CURSOR_INFO cci; // информация о курсоре 
		// читаем дескриптор стандартного вывода 
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		// читаем новый размер курсора 
		cout << "Input a new size of cursor (1-100): ";
		cin >> cci.dwSize;
		// устанавливаем новый размер курсора 
		if (!SetConsoleCursorInfo(hStdOut, &cci))
			cout << "Set console cursor info failed." << endl;
	}

	cout << endl << endl;
	/*

	РАЗМЕР БУФЕРА ЭКРАНА

	*/
	{
		COORD coord; // для размера буфера экрана 
		// читаем дескриптор стандартного вывода 
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		// вводим новый размер буфера экрана 
		cout << "Enter new screen buffer size." << endl;
		cout << "A number of columns (120+): ";
		cin >> coord.X;
		cout << "A number of rows (120+): ";
		cin >> coord.Y;
		// устанавливаем новый размер буфера экрана 
		if (!SetConsoleScreenBufferSize(hStdOut, coord))
		{
			cout << "Set console screen buffer size failed." << endl;
			return GetLastError();
		}
	}

	float* arr = new float[arrLength];

	cout << endl << endl;
	/*

	ГЕНЕРАЦИЯ ЧИСЕЛ

	*/
	{
		int LO = 0;
		int HI = 100;

		cout << endl << endl;
		cout << "Generating array: ";
		srand(static_cast <unsigned> (time(0)));
		for (int i = 0; i < arrLength; i++)
		{
			arr[i] = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			cout << arr[i] << "    ";
		}
	}
	cout << endl << endl;
	/*

	ОТПРАВКА ЧИСЕЛ

	*/
	{
		cout << endl << endl;
		cout << endl << "Send started" << endl;
		for (int i = 0; i < arrLength; i++)
		{
			WriteFile(WritePipe, &arr[i], sizeof(arr[i]), &BytesWritten, nullptr);
			cout << "Sent " << arr[i] << "; ";
		}
		cout << endl << "Send done." << endl;
	}


	cout << endl << endl;
	system("pause");
	return 0;
}
