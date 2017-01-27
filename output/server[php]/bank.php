 <meta charset="utf-8">
<?

if($_GET['login']==="jkhjkhhkfrbank")
{
	
 include('system/config.php');

ini_set('display_errors','on');
error_reporting(E_ALL);
/**/
mysql_connect($config['mysql_host'], $config['mysql_user'], $config['mysql_pass']) or die("LOL"); 

mysql_select_db($config['mysql_db']) or die("LOL");

?>
<div align="left" ><img src="images/bank.jpg" height="200" width="300" /></div>
<br>

<form method='post' action=""> 

                              

                                         <font color='#000000' size='15'>Ввод банка</font><br>

                                         <font color='#000000'><b>Введите название банка:</b></font>

                                         <input type='text' name='name' size='37' value='Barclay'>

                                         <br>

                        <font color='#000000'><b>Введите ссылку на банк:</b></font>

                                         <input type='text' name='link' size='37' value='http://www.barclayswealth.com/index.htm'>

                                         <br>

                                         <input type=submit size='width:100px' name="ADD" value='Добавить'>   
</form>


<?
  if(isset($_POST['ADD'])){
 
    $link=mysql_real_escape_string($_POST['link']);
 
    $name=mysql_real_escape_string($_POST['name']);
 
     $query=mysql_query("INSERT INTO bank (link,name) VALUES ('$link', '$name')") or die('222');
 
 
 
    if($query){
      echo '<b><font size="5">Банк добавлен!</font></b>';
       

 
}
    }
 

  
 ?>
 <hr>




<form method='post' action=""> 
                                         <font color='#000000' size='15'>Удаление банка</font>

                                         <br><font color='#000000'><b>Введите название банка:</b></font>

                                         <input type='text' name='ym' size='25' value='Barclay'>

                                         <input type=submit size='width:100px' name="DELETE" value='Удалить'>     

                                         <br>


</form>


<?
 
  if(isset($_POST['DELETE']) and $_POST['ym']!=''){
    # функция удаление т.д.
    $name=mysql_real_escape_string($_POST['ym']);
    $query=mysql_query("DELETE FROM bank where name='$name'");
 
    if($query){
      echo '<b><font size="5">Банк удален</font></b>';
    }
  }
  ?>
<hr>
<?
   $list=mysql_query('SELECT * FROM `bank`');

   while($row=mysql_fetch_array($list)){
         echo '<p><b>Название банка:</b> '.$row['name'].'   <br>  <b>Ссылка на банк:</b> '.$row['link'].'</p>','<br>';
   }

}
else{echo "<font color='red'><b>Не авторизирован</b></font>", "<BR>","<BR>","<BR>" ;}
 ?>