/*
	Работа с PESETTINGS, храняемых в реестре. Избавляет от необходимости хранить данные в оверлее.
*/
#pragma once

#include "core.h"

namespace PeSettings
{
	/*
		Инициализация.
	*/
	void init();

	/*
		Деинициализация.
	*/
	void uninit();

	/*
		Получение конфигурации в память.

		Return - указатель на PESETTINGS (освободить вызовом Mem::free), или NULL в случае ошибки.
	*/
	PESETTINGS* getCurrent();

	/*
		Получение конфигурации в память, с блокировкой для других процессов + потоков.

		Return - указатель на PESETTINGS (освободить вызовом Mem::free), или NULL в случае ошибки.
	*/
	PESETTINGS* beginReadWrite();

	/*
		Сохранение конфигурации в память, с блокировкой для других процессов + потоков.
		
		IN PESETTINGS* - указатель на конфигурацию.

		Return - false в случае ошибки, true - конфиг сохранен.
    */
	bool endReadWrite(PESETTINGS* config);

	bool setCurrent(void* data, DWORD size);
};