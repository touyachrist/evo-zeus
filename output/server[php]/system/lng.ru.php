<?php if(!defined('__CP__'))die();

define('LNG_TITLE',          'Админ-панель');
define('LNG_YES',            'Да');
define('LNG_NO',             'Нет');
define('LNG_ACCESS_DEFINED', 'Доступ запрещен.');
define('LNG_ACTION_APPLY',   '&#62;&#62;');

//Форматы данных.
define('LNG_FORMAT_DATE',   'd.m.Y');
define('LNG_FORMAT_TIME',   'H:i:s');
define('LNG_FORMAT_DT',     'd.m.Y H:i:s');
define('LNG_FORMAT_NOTIME', '--:--:--');

//Список ботнетов.
define('LNG_BOTNET_ALL',   '[Полный список]');
define('LNG_BOTNET_APPLY', '&#62;&#62;');

//Список страниц.
define('LNG_PAGELIST_TITLE', 'Страницы:');
define('LNG_PAGELIST_NEXT',  'Вперед');
define('LNG_PAGELIST_PREV',  'Назад');
define('LNG_PAGELIST_FIRST', '&#171;');
define('LNG_PAGELIST_LAST',  '&#187;');

//Меню действий.
define('LNG_MBA_SEPARATOR',         '--------------------------------------');

define('LNG_MBA_FULLINFO',        'Данные бота');
define('LNG_MBA_FULLINFOSS',      'Данные бота и скриншот');
define('LNG_MBA_TODAY_DBREPORTS', 'Захваченные данные сегодня');
define('LNG_MBA_WEEK_DBREPORTS',  'Захваченные данные за неделю');
define('LNG_MBA_FILES',           'Файлы');

define('LNG_MBA_REMOVE',          'Удалить бота');
define('LNG_MBA_REMOVE_REPORTS',  'Удалить бота и отчеты');

define('LNG_MBA_PORT_SOCKS',      'Сокс');
define('LNG_MBA_NEWSCRIPT',       'Создать скрипт');

define('LNG_BA_FULLINFO_TITLE',       'Полная информация о ботах');
define('LNG_BA_FULLINFO_EMPTY',       '-- Информация отсутствует --');
define('LNG_BA_FULLINFO_BOTID',       'Идентификация бота:');
define('LNG_BA_FULLINFO_BOTNET',      'Ботнет:');
define('LNG_BA_FULLINFO_VERSION',     'Текущая версия бота:');
define('LNG_BA_FULLINFO_OS',          'Версия ОС:');
define('LNG_BA_FULLINFO_OSLANG',      'Язык ОС:');
define('LNG_BA_FULLINFO_TIMEBIAS',    'Часовой пояс:');
define('LNG_BA_FULLINFO_COUNTRY',     'Страна:');
define('LNG_BA_FULLINFO_IPV4',        'IPv4:');
define('LNG_BA_FULLINFO_LATENCY',     'Лаг:');
define('LNG_BA_FULLINFO_TCPPORT_S1',  'Socks/LC порт:');
define('LNG_BA_FULLINFO_TFIRST',      'Начало первого отчета:');
define('LNG_BA_FULLINFO_TLAST',       'Начало последнего отчета:');
define('LNG_BA_FULLINFO_TONLINE',     'Время в онлайн:');
define('LNG_BA_FULLINFO_NEW',         'К новым ботам:');
define('LNG_BA_FULLINFO_USED',        'К использованным ботам:');
define('LNG_BA_FULLINFO_COMMENT',     'Комментарий:');
define('LNG_BA_FULLINFO_SCREENSHOT',  'Скриншот:');
define('LNG_BA_FULLINFO_ACTION_SAVE', 'Сохранить');

define('LNG_BA_REMOVE_TITLE',    'Удаление ботов из БД');
define('LNG_BA_REMOVE_Q1',       'Вы действительно хотите удалить выбранных ботов из БД (%u шт.)?');
define('LNG_BA_REMOVE_Q2',       'Вы действительно хотите удалить выбранных ботов из БД (%u шт.), включая отчеты?');
define('LNG_BA_REMOVE_ABORTED',  'Удаление отменено пользователем.');
define('LNG_BA_REMOVE_REMOVED',  'Удалено %u записей.');
define('LNG_BA_REMOVE_FREMOVED', 'Удалено.');
define('LNG_BA_REMOVE_FERROR',   'ОШИБКА.');

define('LNG_BA_PORT_SOCKS_TITLE',     'Проверка соксов');
define('LNG_BA_PORT_SOCKS_CHECKING',  'ПРОВЕРКА...');
define('LNG_BA_PORT_SOCKS_SUCCESS',   'МОЖНО ИСПОЛЬЗОВАТЬ');
define('LNG_BA_PORT_SOCKS_FAILED',    'НЕТ СОЕДИНЕНИЯ');
define('LNG_BA_PORT_SOCKS_ERROR',     'ОШИБКА!');

//Информация.
define('LNG_INFO',          '');
define('LNG_INFO_USERNAME', '');
define('LNG_INFO_DATE',     '');
define('LNG_INFO_TIME',     '');

//Главное меню.
define('LNG_MM_STATS',          '');
define('LNG_MM_STATS_MAIN',     '');
define('LNG_MM_STATS_OS',       '');

define('LNG_MM_BOTNET',         '');
define('LNG_MM_BOTNET_BOTS',    '');
define('LNG_MM_BOTNET_SCRIPTS', '');

define('LNG_MM_REPORTS',        '');
define('LNG_MM_REPORTS_DB',     '');
define('LNG_MM_REPORTS_FILES',  '');
define('LNG_MM_REPORTS_JN',     '');

define('LNG_MM_SYSTEM',         '');
define('LNG_MM_SYSTEM_INFO',    '');
define('LNG_MM_SYSTEM_OPTIONS', '');
define('LNG_MM_SYSTEM_USER',    '');
define('LNG_MM_SYSTEM_USERS',   '');

define('LNG_MM_LOGOUT',         '');

//Типы отчетов.
define('LNG_BLT_UNKNOWN',               'Неизвестный формат');
define('LNG_BLT_COOKIES',               'Кукисы браузеров');
define('LNG_BLT_FILE',                  'Файлы');
define('LNG_BLT_HTTPX_REQUEST',         'HTTP или HTTPS линки');
define('LNG_BLT_HTTP_REQUEST',          'HTTP линки');
define('LNG_BLT_HTTPS_REQUEST',         'HTTPS линки');
define('LNG_BLT_LOGIN_FTP',             'FTP-данные');
define('LNG_BLT_LOGIN_POP3',            'POP3-данные');
define('LNG_BLT_GRABBED_X',             'Любые захваченные данные');
define('LNG_BLT_GRABBED_UI',            'Захваченные данные [UI]');
define('LNG_BLT_GRABBED_HTTP',          'Захваченные данные [HTTP(S)]');
define('LNG_BLT_GRABBED_WSOCKET'  ,     'Захваченные данные [WinSocket]');
define('LNG_BLT_GRABBED_FTPSOFTWARE',   'Захваченные данные [FTP-клиент]');
define('LNG_BLT_GRABBED_EMAILSOFTWARE', 'Захваченные данные [E-mail]');
define('LNG_BLT_GRABBED_OTHER',         'Захваченные данные [Other]');
?>