#include <windows.h>
#include <iostream>
using namespace std;
int main()
{
	
	/*
	
	ЗАПРОС РАЗМЕРА МАССИВА
	
	*/
	int arrLength = 0;
	{
		cout << "Input the length of the array: ";
		cin >> arrLength;
	}

	/*
	
	СОЗДАНИЕ МАССИВА
	
	*/
	float* arr = new float[arrLength];
	float* arrCorrect = new float[arrLength];

	cout << endl << endl;
	/*
	
	ОТПРАВКА ДЛИНЫ МАССИВА И ЕГО ПОЛУЧЕНИЕ
	
	*/
	// Наndle каналов чтения и записи
	HANDLE ReadPipe, WritePipe;

	// Атрибуты безопасности, чтобы handlы были наследуемыми
	SECURITY_ATTRIBUTES SecurityAttributes = { 0 };
	SecurityAttributes.nLength = sizeof(SecurityAttributes);
	SecurityAttributes.bInheritHandle = TRUE;
	{
		

		// Создаём каналы чтения и записи, если успешно, продолжаем работу
		if (CreatePipe(&ReadPipe, &WritePipe, &SecurityAttributes, 0))
		{
			// Информация о запускаемом процессе
			STARTUPINFO StartupInfo = { 0 };
			StartupInfo.cb = sizeof(StartupInfo);
			PROCESS_INFORMATION ProcessInfo = { 0 };


			// Путь к исполняемому файлу сервера и командная строка для запуска клиента
			wchar_t Path[MAX_PATH], CmdLine[MAX_PATH];
			// Получаем путь к исполняемому файлу сервера
			GetModuleFileName(nullptr, Path, MAX_PATH);
			// В последний слеш записываем нуль, тем самым усекаем строку
			*wcsrchr(Path, '\\') = 0;
			// получаем командную строку для запуска клиента
			swprintf_s(CmdLine, L"\"%s\\%s\" %jd %jd", Path, L"client.exe", (long long)ReadPipe, (long long)WritePipe);

			// Запускаем процесс клиента, в случае успеха продолжаем работу
			if (CreateProcess(nullptr, CmdLine, nullptr, nullptr, true, CREATE_NEW_CONSOLE,
				nullptr, nullptr, &StartupInfo, &ProcessInfo))
			{
				// Количество прочитанных и записанных байт
				DWORD BytesRead, BytesWritten;

				WaitForSingleObject(ReadPipe, INFINITE);
				long n;
				ReadFile(ReadPipe, &n, sizeof(n), &BytesRead, nullptr);



				// Записываем данные в поток записи
				WriteFile(WritePipe, &arrLength, sizeof(arrLength), &BytesWritten, nullptr);
				
				cout << endl << endl;

				// Ожидаем завершения работы клиента
				WaitForSingleObject(ReadPipe, INFINITE);
				
				// Читаем 
				{
					cout << "Receive started" << endl;
					for (int i = 0; i < arrLength; i++)
					{
						ReadFile(ReadPipe, &arr[i], sizeof(arr[i]), &BytesRead, nullptr);
						cout << "Received " << arr[i] << "; ";
					}
					cout << endl << "Receive done." << endl;
				}
				
				// Ожидаем завершения клиентского процесса
				WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
				
				// Закрываем handlы
				CloseHandle(ProcessInfo.hProcess);
				CloseHandle(ProcessInfo.hThread);
			}
			else // выдаём сообщение об ошибке
				cout << "Process creation failed";

			// закрываем handlы
			CloseHandle(ReadPipe);
			CloseHandle(WritePipe);

		}
		else // или выводим сообщение об ошибке
			cout << "Pipe creation failed";

	}

	cout << endl << "Client's work is done. Booting Part now" << endl;;

	/*
	
	РАБОТА С ПАРТОМ
	
	*/
	int corrSize = 0;
	{

		// Создаём каналы чтения и записи, если успешно, продолжаем работу
		if (CreatePipe(&ReadPipe, &WritePipe, &SecurityAttributes, 0))
		{
			// Информация о запускаемом процессе
			STARTUPINFO StartupInfo = { 0 };
			StartupInfo.cb = sizeof(StartupInfo);
			PROCESS_INFORMATION ProcessInfo = { 0 };


			// Путь к исполняемому файлу сервера и командная строка для запуска клиента
			wchar_t Path[MAX_PATH], CmdLine[MAX_PATH];
			// Получаем путь к исполняемому файлу сервера
			GetModuleFileName(nullptr, Path, MAX_PATH);
			// В последний слеш записываем нуль, тем самым усекаем строку
			*wcsrchr(Path, '\\') = 0;
			// получаем командную строку для запуска клиента
			swprintf_s(CmdLine, L"\"%s\\%s\" %jd %jd", Path, L"part.exe", (long long)ReadPipe, (long long)WritePipe);

			// Запускаем процесс клиента, в случае успеха продолжаем работу
			if (CreateProcess(nullptr, CmdLine, nullptr, nullptr, true, CREATE_NEW_CONSOLE,
				nullptr, nullptr, &StartupInfo, &ProcessInfo))
			{
				// Количество прочитанных и записанных байт
				DWORD BytesRead, BytesWritten;

				WaitForSingleObject(ReadPipe, INFINITE);
				long n;
				ReadFile(ReadPipe, &n, sizeof(n), &BytesRead, nullptr);

				// Записываем данные в поток записи
				WriteFile(WritePipe, &arrLength, sizeof(arrLength), &BytesWritten, nullptr);

				// Пишем массив парту
				cout << endl << "Send started" << endl;
				for (int i = 0; i < arrLength; i++)
				{
					WriteFile(WritePipe, &arr[i], sizeof(arr[i]), &BytesWritten, nullptr);
					cout << "Sent " << arr[i] << "; ";
				}
				cout << endl << "Send done." << endl;

				cout << endl << endl;

				// Ожидаем завершения работы клиента
				WaitForSingleObject(ReadPipe, INFINITE);
				// Читаем длину
				
				ReadFile(ReadPipe, &corrSize, sizeof(corrSize), &BytesRead, nullptr);
				// Читаем 
				{
					cout << "Receive started" << endl;
					for (int i = 0; i < corrSize; i++)
					{
						ReadFile(ReadPipe, &arrCorrect[i], sizeof(arrCorrect[i]), &BytesRead, nullptr);
						cout << "Received " << arrCorrect[i] << "; ";
					}
					cout << endl << "Receive done." << endl;
				}


				// Ожидаем завершения клиентского процесса
				WaitForSingleObject(ProcessInfo.hProcess, INFINITE);

				// Закрываем handlы
				CloseHandle(ProcessInfo.hProcess);
				CloseHandle(ProcessInfo.hThread);
			}
			else // выдаём сообщение об ошибке
				cout << "Process creation failed";

			// закрываем handlы
			CloseHandle(ReadPipe);
			CloseHandle(WritePipe);

		}
		else // или выводим сообщение об ошибке
			cout << "Pipe creation failed";

	}

	cout << endl << endl;
	cout << "Final array: ";
	for (int i = 0; i < corrSize; i++)
		{
		cout << arrCorrect[i]<<" ";
		}


	cout << endl << endl;
	

	cout << "The work is done. Press Enter to exit" << endl;
	system("pause");
	return 0;
}

