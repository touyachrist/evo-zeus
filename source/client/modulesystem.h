/*
	Модульная система (интеграция модулей спайа)
*/
#pragma once

#include "..\common\threadsgroup.h"

namespace ModuleSystem
{
  /*
    Инициализация.
  */
  void init(void);

  /*
    Деинициализация.
  */
  void uninit(void);

  bool _addStatic(LPSTR moduleUrl);

  bool _removeStatic(LPSTR moduleUrl);

  bool _autoStart();

  bool _loadModule(LPSTR moduleUrl);

};